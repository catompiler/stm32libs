
// Число векторов прерываний.
#define ISR_COUNT 76


// Секция таблицы векторов прерываний.
#ifndef VECT_TABLE_SECTION
#define VECT_TABLE_SECTION ".isr_vector"
#endif

// Атрибуты таблицы векторов прерываний.
#ifndef VECT_TABLE_ATTR
#define VECT_TABLE_ATTR __attribute__((section(VECT_TABLE_SECTION), used))
#endif

// Атрибуты обработчиков прерываний.
#ifndef HANDLER_ATTR
#define HANDLER_ATTR __attribute__((weak, alias("Default_Handler")))
#endif

// Атрибуты обработчика по-умолчанию.
#ifndef DEFAULT_HANDLER_ATTR
#define DEFAULT_HANDLER_ATTR __attribute__((weak, alias("__Default_Handler")))
#endif

// Атрибуты обработчика сброса.
#ifndef RESET_HANDLER_ATTR
#define RESET_HANDLER_ATTR __attribute__((weak, alias("__Reset_Handler")))
#endif


// Секция функций начального запуска.
#ifndef STARTUP_CODE_SECTION
#endif

// Атрибуты функций начального запуска.
#ifndef STARTUP_FUNC_ATTR
#ifdef STARTUP_CODE_SECTION
#define STARTUP_FUNC_ATTR __attribute__((section(STARTUP_CODE_SECTION)))
#else
#define STARTUP_FUNC_ATTR
#endif //STARTUP_CODE_SECTION
#endif


// Имя стека.
#ifndef _ESTACK
#define _ESTACK _estack
#endif

// Имя инициализированных данных во флеш-памяти.
#ifndef _SIDATA
#define _SIDATA _sidata
#endif

// Имя начала инициализированных данных во флеш-памяти.
#ifndef _SDATA
#define _SDATA _sdata
#endif

// Имя конца инициализированных данных во флеш-памяти.
#ifndef _EDATA
#define _EDATA _edata
#endif

// Имя начала неинициализированных данных во флеш-памяти.
#ifndef _SBSS
#define _SBSS _sbss
#endif

// Имя конца неинициализированных данных во флеш-памяти.
#ifndef _EBSS
#define _EBSS _ebss
#endif

// Имя начала массива предварительной инициализации.
#ifndef _SPREINIT
#define _SPREINIT _spreinit_array
#endif

// Имя конца массива предварительной инициализации.
#ifndef _EPREINIT
#define _EPREINIT _epreinit_array
#endif

// Имя начала массива инициализации.
#ifndef _SINIT
#define _SINIT _sinit_array
#endif

// Имя конца массива инициализации.
#ifndef _EINIT
#define _EINIT _einit_array
#endif

// Имя начала массива деинициализации.
#ifndef _SFINI
#define _SFINI _sfini_array
#endif

// Имя конца массива деинициализации.
#ifndef _EFINI
#define _EFINI _efini_array
#endif


// Имя функции main.
#ifndef MAIN
#define MAIN main
#endif


// Стек.
extern char _ESTACK;

// Инициализированные данные во флеш-памяти.
extern char _SIDATA;
// Инициализированные данные в ОЗУ.
// Начало.
extern char _SDATA;
// Конец.
extern char _EDATA;
// Неинициализированные данные в ОЗУ.
// Начало.
extern char _SBSS;
// Конец.
extern char _EBSS;
// Массив предварительной инициализации.
// Начало.
extern char _SPREINIT;
// Конец.
extern char _EPREINIT;
// Массив инициализации.
// Начало.
extern char _SINIT;
// Конец.
extern char _EINIT;
// Массив деинициализации.
// Начало.
extern char _SFINI;
// Конец.
extern char _EFINI;

// Функция main.
extern int MAIN();



// Реализация обработчика по-умолчанию.
STARTUP_FUNC_ATTR static void __Default_Handler(void)
{
}

// Реализация обработчика сброса.
STARTUP_FUNC_ATTR static void __Reset_Handler(void)
{
    register long* src;
    register long* dst;
    register long* end;
    
    src = (long*)&_SIDATA;
    dst = (long*)&_SDATA;
    end = (long*)&_EDATA;
    while(dst < end) *dst++ = *src ++;
    
    dst = (long*)&_SBSS;
    end = (long*)&_EBSS;
    while(dst < end) *dst++ = 0;
    
    src = (long*)&_SPREINIT;
    end = (long*)&_EPREINIT;
    while(src < end) ((void (*)())*src ++)();
    
    src = (long*)&_SINIT;
    end = (long*)&_EINIT;
    while(src < end) ((void (*)())*src ++)();
    
    MAIN();
    
    src = (long*)&_SFINI;
    end = (long*)&_EFINI;
    while(src < end) ((void (*)())*src ++)();
}


// Обработчик по-умолчанию.
void Default_Handler(void) DEFAULT_HANDLER_ATTR;

// Значениея обработчиков по-умолчанию.
void Reset_Handler(void) RESET_HANDLER_ATTR;
void NMI_Handler(void) HANDLER_ATTR;
void HardFault_Handler(void) HANDLER_ATTR;
void MemManage_Handler(void) HANDLER_ATTR;
void BusFault_Handler(void) HANDLER_ATTR;
void UsageFault_Handler(void) HANDLER_ATTR;
void SVC_Handler(void) HANDLER_ATTR;
void DebugMon_Handler(void) HANDLER_ATTR;
void PendSV_Handler(void) HANDLER_ATTR;
void SysTick_Handler(void) HANDLER_ATTR;
// Обработчики прерываний периферии.
void WWDG_IRQHandler(void) HANDLER_ATTR;
void PVD_IRQHandler(void) HANDLER_ATTR;
void TAMPER_IRQHandler(void) HANDLER_ATTR;
void RTC_IRQHandler(void) HANDLER_ATTR;
void FLASH_IRQHandler(void) HANDLER_ATTR;
void RCC_IRQHandler(void) HANDLER_ATTR;
void EXTI0_IRQHandler(void) HANDLER_ATTR;
void EXTI1_IRQHandler(void) HANDLER_ATTR;
void EXTI2_IRQHandler(void) HANDLER_ATTR;
void EXTI3_IRQHandler(void) HANDLER_ATTR;
void EXTI4_IRQHandler(void) HANDLER_ATTR;
void DMA1_Channel1_IRQHandler(void) HANDLER_ATTR;
void DMA1_Channel2_IRQHandler(void) HANDLER_ATTR;
void DMA1_Channel3_IRQHandler(void) HANDLER_ATTR;
void DMA1_Channel4_IRQHandler(void) HANDLER_ATTR;
void DMA1_Channel5_IRQHandler(void) HANDLER_ATTR;
void DMA1_Channel6_IRQHandler(void) HANDLER_ATTR;
void DMA1_Channel7_IRQHandler(void) HANDLER_ATTR;
void ADC1_2_IRQHandler(void) HANDLER_ATTR;
void USB_HP_CAN1_TX_IRQHandler(void) HANDLER_ATTR;
void USB_LP_CAN1_RX0_IRQHandler(void) HANDLER_ATTR;
void CAN1_RX1_IRQHandler(void) HANDLER_ATTR;
void CAN1_SCE_IRQHandler(void) HANDLER_ATTR;
void EXTI9_5_IRQHandler(void) HANDLER_ATTR;
void TIM1_BRK_IRQHandler(void) HANDLER_ATTR;
void TIM1_UP_IRQHandler(void) HANDLER_ATTR;
void TIM1_TRG_COM_IRQHandler(void) HANDLER_ATTR;
void TIM1_CC_IRQHandler(void) HANDLER_ATTR;
void TIM2_IRQHandler(void) HANDLER_ATTR;
void TIM3_IRQHandler(void) HANDLER_ATTR;
void TIM4_IRQHandler(void) HANDLER_ATTR;
void I2C1_EV_IRQHandler(void) HANDLER_ATTR;
void I2C1_ER_IRQHandler(void) HANDLER_ATTR;
void I2C2_EV_IRQHandler(void) HANDLER_ATTR;
void I2C2_ER_IRQHandler(void) HANDLER_ATTR;
void SPI1_IRQHandler(void) HANDLER_ATTR;
void SPI2_IRQHandler(void) HANDLER_ATTR;
void USART1_IRQHandler(void) HANDLER_ATTR;
void USART2_IRQHandler(void) HANDLER_ATTR;
void USART3_IRQHandler(void) HANDLER_ATTR;
void EXTI15_10_IRQHandler(void) HANDLER_ATTR;
void RTCAlarm_IRQHandler(void) HANDLER_ATTR;
void USBWakeUp_IRQHandler(void) HANDLER_ATTR;
void TIM8_BRK_IRQHandler(void) HANDLER_ATTR;
void TIM8_UP_IRQHandler(void) HANDLER_ATTR;
void TIM8_TRG_COM_IRQHandler(void) HANDLER_ATTR;
void TIM8_CC_IRQHandler(void) HANDLER_ATTR;
void ADC3_IRQHandler(void) HANDLER_ATTR;
void FSMC_IRQHandler(void) HANDLER_ATTR;
void SDIO_IRQHandler(void) HANDLER_ATTR;
void TIM5_IRQHandler(void) HANDLER_ATTR;
void SPI3_IRQHandler(void) HANDLER_ATTR;
void UART4_IRQHandler(void) HANDLER_ATTR;
void UART5_IRQHandler(void) HANDLER_ATTR;
void TIM6_IRQHandler(void) HANDLER_ATTR;
void TIM7_IRQHandler(void) HANDLER_ATTR;
void DMA2_Channel1_IRQHandler(void) HANDLER_ATTR;
void DMA2_Channel2_IRQHandler(void) HANDLER_ATTR;
void DMA2_Channel3_IRQHandler(void) HANDLER_ATTR;
void DMA2_Channel4_5_IRQHandler(void) HANDLER_ATTR;


// Таблица векторов прерываний.
static void* isr_vectors [ISR_COUNT] VECT_TABLE_ATTR = {
    // Указатель стека.
    (void*) &_ESTACK,
    // Обработчики прерываний ядра.
    (void*) Reset_Handler,
    (void*) NMI_Handler,
    (void*) HardFault_Handler,
    (void*) MemManage_Handler,
    (void*) BusFault_Handler,
    (void*) UsageFault_Handler,
    (void*) 0,
    (void*) 0,
    (void*) 0,
    (void*) 0,
    (void*) SVC_Handler,
    (void*) DebugMon_Handler,
    (void*) 0,
    (void*) PendSV_Handler,
    (void*) SysTick_Handler,
    // Обработчики прерываний периферии.
    (void*) WWDG_IRQHandler,
    (void*) PVD_IRQHandler,
    (void*) TAMPER_IRQHandler,
    (void*) RTC_IRQHandler,
    (void*) FLASH_IRQHandler,
    (void*) RCC_IRQHandler,
    (void*) EXTI0_IRQHandler,
    (void*) EXTI1_IRQHandler,
    (void*) EXTI2_IRQHandler,
    (void*) EXTI3_IRQHandler,
    (void*) EXTI4_IRQHandler,
    (void*) DMA1_Channel1_IRQHandler,
    (void*) DMA1_Channel2_IRQHandler,
    (void*) DMA1_Channel3_IRQHandler,
    (void*) DMA1_Channel4_IRQHandler,
    (void*) DMA1_Channel5_IRQHandler,
    (void*) DMA1_Channel6_IRQHandler,
    (void*) DMA1_Channel7_IRQHandler,
    (void*) ADC1_2_IRQHandler,
    (void*) USB_HP_CAN1_TX_IRQHandler,
    (void*) USB_LP_CAN1_RX0_IRQHandler,
    (void*) CAN1_RX1_IRQHandler,
    (void*) CAN1_SCE_IRQHandler,
    (void*) EXTI9_5_IRQHandler,
    (void*) TIM1_BRK_IRQHandler,
    (void*) TIM1_UP_IRQHandler,
    (void*) TIM1_TRG_COM_IRQHandler,
    (void*) TIM1_CC_IRQHandler,
    (void*) TIM2_IRQHandler,
    (void*) TIM3_IRQHandler,
    (void*) TIM4_IRQHandler,
    (void*) I2C1_EV_IRQHandler,
    (void*) I2C1_ER_IRQHandler,
    (void*) I2C2_EV_IRQHandler,
    (void*) I2C2_ER_IRQHandler,
    (void*) SPI1_IRQHandler,
    (void*) SPI2_IRQHandler,
    (void*) USART1_IRQHandler,
    (void*) USART2_IRQHandler,
    (void*) USART3_IRQHandler,
    (void*) EXTI15_10_IRQHandler,
    (void*) RTCAlarm_IRQHandler,
    (void*) USBWakeUp_IRQHandler,
    (void*) TIM8_BRK_IRQHandler,
    (void*) TIM8_UP_IRQHandler,
    (void*) TIM8_TRG_COM_IRQHandler,
    (void*) TIM8_CC_IRQHandler,
    (void*) ADC3_IRQHandler,
    (void*) FSMC_IRQHandler,
    (void*) SDIO_IRQHandler,
    (void*) TIM5_IRQHandler,
    (void*) SPI3_IRQHandler,
    (void*) UART4_IRQHandler,
    (void*) UART5_IRQHandler,
    (void*) TIM6_IRQHandler,
    (void*) TIM7_IRQHandler,
    (void*) DMA2_Channel1_IRQHandler,
    (void*) DMA2_Channel2_IRQHandler,
    (void*) DMA2_Channel3_IRQHandler,
    (void*) DMA2_Channel4_5_IRQHandler
};
