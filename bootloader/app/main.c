#include "stm32f10x.h"


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

static void init_periph_clock(void)
{
    // GPIO.
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
}


int leds_to_on = LED1;
int leds_to_off = LED2;
int leds_to_toggle = LED1 | LED2;


void delay_1s(void)
{
    volatile int i = 8000000;
    while(-- i);
}

/**
 * Инициализация тактирования на 72 МГц.
 */
void init_rcc(void)
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

int main(void)
{
    init_rcc();
    init_periph_clock();
    init_led();
    
    led_on(leds_to_on);
    led_off(leds_to_off);
    
    for(;;){
        delay_1s();
        led_toggle(leds_to_toggle);
    }
    return 0;
}
