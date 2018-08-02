#include "stm32f10x.h"
#include <errno.h>
#include <sys/time.h>
#define INLINE_DELAY
#include "utils/delay.h"
#include "usart/usart_bus.h"
#include "modbus/modbus_rtu.h"
#include "bootloader_modbus.h"
#include "flash/flash.h"


//! Счётчик миллисекунд.
static uint32_t counter_ms;

//! Шина USART1.
static usart_bus_t usart_bus;

//! Modbus.
static modbus_rtu_t modbus;
//! Сообщения Modbus.
static modbus_rtu_message_t modbus_rx_msg, modbus_tx_msg;


// Приоритеты прерываний.
#define IRQ_PRIOR_SYSTICK 1
#define IRQ_PRIOR_USART1 8
#define IRQ_PRIOR_DMA_CH4 7
#define IRQ_PRIOR_DMA_CH5 7

/*
 * Параметры USART.
 */
//! Скорость.
#define BOOTLOADER_USART_BAUD 115200

/*
 * Работа загрузчика.
 */

//! Тайм-аут работы без подключения в секундах.
#define BOOTLOADER_TIMEOUT_S 10

//! Тайм-аут работы при подключении в секундах.
#define BOOTLOADER_USED_TIMEOUT_S 100

//! Флаг использования загрузчика - подключения по modbus.
volatile static bool bootloader_used = false;

//! Флаг завершения работы (запуска приложения).
volatile static bool bootloader_done = false;

//! Флаг приёма сообщения modbus.
volatile static bool bootloader_modbus_msg = false;


/*
 * Светодтоды для отладки.
 */

//! Использование светодиодов.
#define BOOTLOADER_USE_LEDS

#ifdef BOOTLOADER_USE_LEDS

#define LED1 4
#define LED2 8

static void init_led(void)
{
    // LED1.
    GPIOA->CRL |=  GPIO_CRL_MODE2_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF2;
    // LED2.
    GPIOA->CRL |=  GPIO_CRL_MODE3_1;
    GPIOA->CRL &= ~GPIO_CRL_CNF3;
}

static void led_on(uint16_t led_pin)
{
    GPIOA->BSRR = led_pin;
}

static void led_off(uint16_t led_pin)
{
    GPIOA->BRR = led_pin;
}

static void led_toggle(uint16_t led_pin)
{
    GPIOA->ODR ^= led_pin;
}

#endif //BOOTLOADER_USE_LEDS


//! Последнее время обмена данными.
static struct timeval bootloader_last_io_time = {0, 0};

/**
 * Сбрасывает последнее время обмена данными до текущего.
 */
static void bootloader_reset_last_io_time(void)
{
    gettimeofday(&bootloader_last_io_time, NULL);
}

/**
 * Получает флаг превышения тайм-аута работы загрузчика.
 */
static bool bootloader_time_expired(void)
{
    struct timeval cur_tv;
    gettimeofday(&cur_tv, NULL);
    
    struct timeval expire_tv  = {
        bootloader_last_io_time.tv_sec,
        bootloader_last_io_time.tv_usec
    };
    
    expire_tv.tv_sec += bootloader_used ? BOOTLOADER_USED_TIMEOUT_S : BOOTLOADER_TIMEOUT_S;
    
    return timercmp(&cur_tv, &expire_tv, >=);
}


/*
 * Обработчики прерываний.
 */

void SysTick_Handler(void)
{
    counter_ms ++;
}

void USART1_IRQHandler(void)
{
    usart_bus_irq_handler(&usart_bus);
}

void DMA1_Channel4_IRQHandler(void)
{
    usart_bus_dma_tx_channel_irq_handler(&usart_bus);
}

void DMA1_Channel5_IRQHandler(void)
{
    usart_bus_dma_rx_channel_irq_handler(&usart_bus);
}

/*
 * Каллбэки.
 */



static void modbus_rs485_set_input(void)
{
    //RS485_IFACE_CTL_GPIO->BRR = RS485_IFACE_CTL_PIN;
}

static void modbus_rs485_set_output(void)
{
    //RS485_IFACE_CTL_GPIO->BSRR = RS485_IFACE_CTL_PIN;
}

static bool usart_rx_byte_callback(uint8_t byte)
{
    return modbus_rtu_usart_rx_byte_callback(&modbus, byte);
}

static bool usart_rx_callback(void)
{
    return modbus_rtu_usart_rx_callback(&modbus);
}

static bool usart_tx_callback(void)
{
    return modbus_rtu_usart_tx_callback(&modbus);
}

static bool usart_tc_callback(void)
{
    modbus_rs485_set_input();
    
    return true;
}

static void modbus_on_msg_recv(void)
{
#ifdef BOOTLOADER_USE_LEDS
    led_on(LED2);
#endif
    
    // Если получено сообщение по Modbus,
    // значит требуется работа загрузчика.
    // Пометим флаг использования (увеличив тем самым тайм-аут).
    bootloader_used = true;
    
    // Установим флаг приёма сообщения.
    bootloader_modbus_msg = true;
}

static void modbus_on_msg_sent(void)
{
#ifdef BOOTLOADER_USE_LEDS
    led_off(LED2);
#endif
    
    bootloader_reset_last_io_time();
}


/*
 * Инициализация периферии.
 */

static void init_remap(void)
{
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}

static void deinit_remap(void)
{
    AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
}

static void init_syscounter(void)
{
    SysTick->VAL = 0;
    SysTick->LOAD = SystemCoreClock / 1000;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

static void deinit_syscounter(void)
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
}


/*
 * Получение текущего времени.
 */
int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
    if(tv == NULL){
        errno = EFAULT;
        return -1;
    }
    
    uint32_t millis = counter_ms;
    
    tv->tv_sec = millis / 1000;
    tv->tv_usec = millis * 1000;
    
    if(tz != NULL){
        tz->tz_minuteswest = 0;
        tz->tz_dsttime = DST_NONE;
    }
    
    return 0;
}

static void init_periph_clock(void)
{
    // GPIO.
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    // USART1.
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    // DMA1.
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
}

static void deinit_periph_clock(void)
{
    // GPIO.
    // GPIOA.
    RCC->APB2RSTR |= RCC_APB2RSTR_IOPARST;
    __NOP();
    RCC->APB2RSTR &= ~RCC_APB2RSTR_IOPARST;
    RCC->APB2ENR &= ~RCC_APB2ENR_IOPAEN;
    
    // USART1.
    RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
    __NOP();
    RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
    RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
    
    // DMA1.
    RCC->AHBENR &= ~RCC_AHBENR_DMA1EN;
}

static void init_interrupts(void)
{
    NVIC_SetPriorityGrouping(0x3);
    
    NVIC_SetPriority(SysTick_IRQn, IRQ_PRIOR_SYSTICK);
    NVIC_SetPriority(USART1_IRQn, IRQ_PRIOR_USART1);
    NVIC_SetPriority(DMA1_Channel4_IRQn, IRQ_PRIOR_DMA_CH4);
    NVIC_SetPriority(DMA1_Channel5_IRQn, IRQ_PRIOR_DMA_CH5);
    
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

static void deinit_interrupts(void)
{
    NVIC_DisableIRQ(USART1_IRQn);
    NVIC_DisableIRQ(DMA1_Channel4_IRQn);
    NVIC_DisableIRQ(DMA1_Channel5_IRQn);
    
    NVIC_SetPriority(SysTick_IRQn, 0);
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_SetPriority(DMA1_Channel4_IRQn, 0);
    NVIC_SetPriority(DMA1_Channel5_IRQn, 0);
    
    NVIC_SetPriorityGrouping(0x0);
}

static void init_usart1_gpio(void)
{
    // TX - GPIOA pin 9.
    // Поднятый TX.
    GPIOA->BSRR = GPIO_BSRR_BS9;
    // Выход 2 МГц.
    GPIOA->CRH |= GPIO_CRH_MODE9_1;
    // AF Push-pull.
    GPIOA->CRH |= GPIO_CRH_CNF9_1;
    GPIOA->CRH &= ~GPIO_CRH_CNF9_0;
    
    // RX - GPIOA pin 10.
    // Не нужно, достаточно состояния по-умолчанию.
}

/*
static void usart_send(USART_TypeDef* usart, const char* str)
{
    while(*str){
        while(!(usart->SR & USART_SR_TXE));
        usart->DR = *str ++;
    }
}
*/

static void init_usart(void)
{
    // Порт ввода-вывода.
    init_usart1_gpio();
    
    USART1->BRR = SystemCoreClock / BOOTLOADER_USART_BAUD;
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

static void init_usart_bus(void)
{
    usart_bus_init_t ub_is;
    
    ub_is.usart_device = USART1;
    ub_is.dma_tx_channel = DMA1_Channel4;
    ub_is.dma_rx_channel = DMA1_Channel5;
    
    usart_bus_init(&usart_bus, &ub_is);
    
    usart_bus_set_rx_callback(&usart_bus, usart_rx_callback);
    usart_bus_set_tx_callback(&usart_bus, usart_tx_callback);
    usart_bus_set_tc_callback(&usart_bus, usart_tc_callback);
    usart_bus_set_rx_byte_callback(&usart_bus, usart_rx_byte_callback);
    
    usart_bus_set_idle_mode(&usart_bus, USART_IDLE_MODE_END_RX);
}

static void init_modbus(void)
{
    modbus_rtu_init_t modbus_is;
    
    modbus_is.usart = &usart_bus;
    modbus_is.mode = MODBUS_RTU_MODE_SLAVE;
    
    /*param_t* address_param = settings_param_by_id(PARAM_ID_MODBUS_ADDRESS);
    if(address_param){
        modbus_is.address = settings_param_valueu(address_param);
    }else{
        modbus_is.address = 0xaa;//0x10;
    }*/
    
    modbus_is.address = 0x1;
    
    modbus_is.rx_message = &modbus_rx_msg;
    modbus_is.tx_message = &modbus_tx_msg;
    
    modbus_rtu_init(&modbus, &modbus_is);
    
    modbus_rtu_set_msg_recv_callback(&modbus, modbus_on_msg_recv);
    modbus_rtu_set_msg_sent_callback(&modbus, modbus_on_msg_sent);
    
    modbus_rs485_set_input();
}

static void on_bootloader_modbus_run_app(void)
{
    bootloader_done = true;
}

static void init_boot_modbus(void)
{
    bootloader_modbus_init();
    bootloader_modbus_set_run_app_callback(on_bootloader_modbus_run_app);
    bootloader_modbus_setup_modbus(&modbus);
}

/**
 * Инициализация тактирования на 72 МГц.
 */
void system_clock_init(void)
{
    // Предделители шин.
    // AHB - нет предделителя.
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    // APB1 - делитель на 2.
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
    // APB2 - нет предделителя.
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
    // ADC - делитель на 6.
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;
    // USB - делитель на 1.5.
    RCC->CFGR &= ~RCC_CFGR_USBPRE;
    
    // ФАПЧ.
    // Источник.
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE;
    // Делитель частоты внешнего осциллятора.
    RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;
    // Умножитель частоты.
    RCC->CFGR |= RCC_CFGR_PLLMULL9;
    
    // Переход на тактирование от PLL.
    // Включение внешнего осциллятора.
    RCC->CR |= RCC_CR_HSEON;
    // Подождём запуска внешнего осциллятора.
    while(!(RCC->CR & RCC_CR_HSERDY));
    // Запустим ФАПЧ.
    RCC->CR |= RCC_CR_PLLON;
    // Подождём запуска ФАПЧ.
    while(!(RCC->CR & RCC_CR_PLLRDY));
    
    // Настройка чтения флеш-памяти.
    // Запретим доступ по половине цикла тактирования.
    // Доступ по половине цикла тактирования запрещён при загрузке.
    //FLASH->ACR &= ~FLASH_ACR_HLFCYA;
    // Установим задержку чтения из флеш-памяти в 2 такта.
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
    
    // Разрешим буфер предвыборки.
    // Буфер предвыборки включен после загрузки.
    //FLASH->ACR |= FLASH_ACR_PRFTBE;
    // Подождём включения предвыборки.
    //while(!(FLASH->ACR & FLASH_ACR_PRFTBS));
    
    // Перейдём на тактирование от ФАПЧ.
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    // Подождём перехода на ФАПЧ.
    while(!(RCC->CFGR & RCC_CFGR_SWS_PLL));
    
    // Установим значение частоты ядра.
    SystemCoreClock = 72000000;
}

static void system_clock_deinit(void)
{
    // Тактирование.
    // Перейдём на тактирование от внутреннего RC-осциллятора.
    RCC->CFGR &= ~RCC_CFGR_SW;
    // Подождём перехода на внутренний RC-осциллятор.
    while((RCC->CFGR & RCC_CFGR_SWS));
    
    // Флеш-память.
    // Сбросим задержку чтения из флеш-памяти.
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    
    // ФАПЧ и внешний осциллятор.
    // Остановим ФАПЧ.
    RCC->CR &= ~RCC_CR_PLLON;
    // Подождём остановки ФАПЧ.
    while((RCC->CR & RCC_CR_PLLRDY));
    // Выключим внешний осциллятор.
    RCC->CR &= ~RCC_CR_HSEON;
    // Подождём выключения внешнего осциллятора.
    while((RCC->CR & RCC_CR_HSERDY));
    
    // Сброс настроек ФАПЧ.
    // Источник.
    RCC->CFGR &= ~RCC_CFGR_PLLSRC;
    // Делитель частоты внешнего осциллятора.
    RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;
    // Умножитель частоты.
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    
    // Предделители шин.
    // AHB.
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    // APB1.
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    // APB2.
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
    // ADC.
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    // USB.
    RCC->CFGR &= ~RCC_CFGR_USBPRE;
    
    // Установим значение частоты ядра.
    SystemCoreClock = 8000000;
}

static void init_periph(void)
{
    system_clock_init();
    init_remap();
    init_syscounter();
    init_periph_clock();
#ifdef BOOTLOADER_USE_LEDS
    init_led();
#endif
    init_usart();
    init_usart_bus();
    init_modbus();
    init_boot_modbus();
    init_interrupts();
    
    //const char* str = "Hello, stm32!\r\n";
    //usart_bus_send(&usart_bus, str, strlen(str));
    //usart_bus_send(&usart_bus, "STM32 Bootloader USART\r\n", 24);
}

static void deinit_periph(void)
{
    deinit_interrupts();
    deinit_periph_clock();
    deinit_syscounter();
    deinit_remap();
    system_clock_deinit();
}

__attribute__((noreturn)) static void start_app(void)
{
    extern char _app_origin;
    
    uint32_t* app_vect = (uint32_t*)&_app_origin;
    
    deinit_periph();
    
    SCB->VTOR = (uint32_t)app_vect;
    __set_MSP(app_vect[0]);
    ((void (*)(void))app_vect[1])();
    
    for(;;);
}


#ifdef BOOTLOADER_USE_LEDS
static void process_led_blink(void)
{
    static struct timeval tv_prev = {0,0};
    static const struct timeval tv_int = {0, 500000};
    struct timeval tv, dt;
    
    gettimeofday(&tv, NULL);
    
    timersub(&tv, &tv_prev, &dt);
    
    if(timercmp(&dt, &tv_int, >=)){
        
        led_toggle(LED1);
        
        tv_prev.tv_sec = tv.tv_sec;
        tv_prev.tv_usec = tv.tv_usec;
    }
}
#endif

int main(void)
{
    init_periph();
    
    // Если микроконтроллер перезагружен программно,
    // значит требуется работа загрузчика.
    // Пометим флаг использования (увеличив тем самым тайм-аут).
    if(RCC->CSR & RCC_CSR_SFTRSTF) bootloader_used = true;
    
#ifdef BOOTLOADER_USE_LEDS
    led_off(LED1);
#endif
    
    for(;!bootloader_done;){
        
        // Если приняли сообщение.
        if(bootloader_modbus_msg){

            bootloader_modbus_msg = false;

            modbus_rs485_set_output();
            if(modbus_rtu_dispatch(&modbus) != E_NO_ERROR){
                modbus_rs485_set_input();
            }
        }
        
#ifdef BOOTLOADER_USE_LEDS
        // Мигание светодиодом.
        process_led_blink();
#endif
        
        // Тайм-аут.
        if(bootloader_time_expired()) break;
    }
    
    start_app();
    
    return 0;
}
