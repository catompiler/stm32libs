#include "system.h"
#include <stm32f10x.h>


/**
 * Определение переменной со значением частоты ядра.
 * Частота по-умолчанию при включении равна 8 МГц.
 */
uint32_t SystemCoreClock = 8000000;


/**
 * Инициализация тактирования на 72 МГц
 * при частоте кварца 8 МГц.
 */
void init_clock_72_hse_8(void)
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

/**
 * Сбрасывает тактирование на состояние после сброса -
 * 8 МГц внутренний RC-осциллятор.
 */
static void deinit_clock(void)
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

void SystemInit(void)
{
    init_clock_72_hse_8();
}

void SystemDeInit(void)
{
    deinit_clock();
}
