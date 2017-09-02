#include "stm32f10x.h"
#define USART_STDIO
#include "usart/usart_buf.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"


#define USART_WRITE_BUFFER_SIZE 128
static uint8_t usart_write_buffer[USART_WRITE_BUFFER_SIZE];

#define USART_READ_BUFFER_SIZE 128
static uint8_t usart_read_buffer[USART_READ_BUFFER_SIZE];

static usart_buf_t usart_buf;


#define BLINK_TASK_PRIORITY 1
#define PRINT_TASK_PRIORITY 1


#define LED_GPIO GPIOA
#define LED0 GPIO_Pin_1
#define LED1 GPIO_Pin_1

static void init_leds(void)
{
    GPIO_InitTypeDef gpio_led =
        {.GPIO_Pin = LED0 | LED1, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_Out_PP};
    
    GPIO_Init(GPIOA, &gpio_led);
    LED_GPIO->ODR |= LED0 | LED1;
}

static void led_off(uint16_t led)
{
    LED_GPIO->ODR |= led;
}

static void led_on(uint16_t led)
{
    LED_GPIO->ODR &= ~led;
}

static void led_toggle(uint16_t led)
{
    LED_GPIO->ODR ^= led;
}

static bool led_status(uint16_t led)
{
    return LED_GPIO->IDR & led;
}


void USART1_IRQHandler(void)
{
    usart_buf_irq_handler(&usart_buf);
}


static void init_periph_clock(void)
{
    // AFIO.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // USART.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

static void init_usart(void)
{
    GPIO_InitTypeDef gpio_tx =
        {.GPIO_Pin = GPIO_Pin_9, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_AF_PP};
    GPIO_InitTypeDef gpio_rx =
        {.GPIO_Pin = GPIO_Pin_10, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_IN_FLOATING};
    GPIO_Init(GPIOA, &gpio_tx);
    GPIO_Init(GPIOA, &gpio_rx);
    
    USART_InitTypeDef usart_is =
        {.USART_BaudRate = 9600, .USART_WordLength = USART_WordLength_8b, .USART_StopBits = USART_StopBits_1,
         .USART_Parity = USART_Parity_No, .USART_Mode = USART_Mode_Rx | USART_Mode_Tx, .USART_HardwareFlowControl = USART_HardwareFlowControl_None};
    USART_Init(USART1, &usart_is);
    USART_Cmd(USART1, ENABLE);
    
    usart_buf_init_t usartb_is = {.usart = USART1,
         .write_buffer = usart_write_buffer, .write_buffer_size = USART_WRITE_BUFFER_SIZE,
         .read_buffer = usart_read_buffer, .read_buffer_size = USART_READ_BUFFER_SIZE};
    usart_buf_init(&usart_buf, &usartb_is);
    usart_setup_stdio(&usart_buf);
    
    NVIC_SetPriority(USART1_IRQn, 12);
    NVIC_EnableIRQ(USART1_IRQn);
}

void blink_led_task(void* param)
{
    for(;;){
        vTaskDelay(500 / portTICK_RATE_MS);
        led_toggle(LED0);
    }
}

void print_msg_task(void* param)
{
    for(;;){
        vTaskDelay(1000 / portTICK_RATE_MS);
        printf("ticks: %d\r\n", (int)xTaskGetTickCount());
    }
}

int main(void)
{
    NVIC_SetPriorityGrouping(0x3);
    
    init_periph_clock();
    init_usart();
    init_leds();
    
    printf("STM32 MCU\r\n");
    
    xTaskCreate(blink_led_task, "blink", configMINIMAL_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);
    xTaskCreate(print_msg_task, "print", configMINIMAL_STACK_SIZE, NULL, PRINT_TASK_PRIORITY, NULL);
    
    vTaskStartScheduler();
    
    for(;;){
    }
    
    return 0;
}
