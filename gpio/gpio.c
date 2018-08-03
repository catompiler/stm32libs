#include "gpio.h"

//! Степень оптимизации установки параметров пина.
#define GPIO_INIT_OPTIMIZE 2

//! Реализация установки параметров одного пина.
//! Жалко выкидывать разные варианты, в т.ч. на assembler.
ALWAYS_INLINE static void gpio_init_pin_impl(GPIO_TypeDef* GPIO, gpio_pin_t pin, gpio_mode_t mode, gpio_conf_t conf)
{
#if GPIO_INIT_OPTIMIZE == 0
    // Вычисление номера пина.
    uint32_t pin_n = __CLZ(__RBIT((uint32_t)pin));
    // Регистр конфигурации пина.
    __IO uint32_t* cr = (pin_n & 0x8) ? &GPIO->CRH : &GPIO->CRL;
    // Номер первого бита полей пина в регистре.
    uint32_t num = ((pin_n & 0x7) << 2);
    // Значение конфигурации для установки.
    uint32_t val = ((mode | (conf << 2)) << num);
    // Маска для сброса конфигурации.
    uint32_t msk = ~((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) << num);
    // Сбросим параметры пина.
    *cr &= msk;
    // Установим новые параметры пина.
    *cr |= val;
#elif GPIO_INIT_OPTIMIZE == 1
    // Вычисление номера пина.
    uint32_t pin_n = __CLZ(__RBIT((uint32_t)pin));
    // Регистр конфигурации пина.
    __IO uint32_t* cr = (pin_n & 0x8) ? &GPIO->CRH : &GPIO->CRL;
    // Номер первого бита полей пина в регистре.
    uint32_t num = ((pin_n & 0x7) << 2);
    // Значение конфигурации для установки.
    uint32_t val = ((mode | (conf << 2)) << num);
    // Маска для сброса конфигурации.
    uint32_t msk = ~((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) << num);
    // Сбросим параметры пина.
    // Установим новые параметры пина.
    *cr = (*cr & msk) | val;
#elif GPIO_INIT_OPTIMIZE == 2
    // Вычисление номера пина.
    uint32_t pin_n = __CLZ(__RBIT((uint32_t)pin));
    // Регистр конфигурации пина.
    __IO uint32_t* cr = &GPIO->CRL + ((pin_n & 0x8) >> 3);
    // Номер первого бита полей пина в регистре.
    uint32_t num = ((pin_n & 0x7) << 2);
    // Значение конфигурации для установки.
    uint32_t val = ((mode | (conf << 2)) << num);
    // Маска для сброса конфигурации.
    uint32_t msk = ~((GPIO_CRL_MODE0 | GPIO_CRL_CNF0) << num);
    // Сбросим параметры пина.
    // Установим новые параметры пина.
    *cr = (*cr & msk) | val;
#elif GPIO_INIT_OPTIMIZE == 3
    __asm__ volatile(
        "rbit %1, %1\n\t"               // pin = __RBIT(pin);
        "clz %1, %1\n\t"                // pin = __CLZ(pin);
        "ubfx r4, %1, #3, #1\n\t"       // tmp = (pin & 0x8) >> 3;
        "add %0, %0, r4, lsl #2\n\t"    // cr* = cr* + tmp;
        "and %1, %1, #7\n\t"            // pin = pin & 0x7;
        "lsl %1, %1, #2\n\t"            // pin = pin << 2;
        "orr %2, %2, %3, lsl #2\n\t"    // mode = mode | (conf << 2);
        "lsl %2, %2, %1\n\t"            // mode = mode << pin;
        "mov %3, #15\n\t"               // conf = 15;
        "lsl %3, %3, %1\n\t"            // conf = 15 << pin;
        "ldr r4, [%0]\n\t"              // tmp = *cr;
        "bic r4, r4, %3\n\t"            // tmp = tmp & conf;
        "orr r4, r4, %2\n\t"            // tmp = tmp | mode;
        "str r4, [%0]\n\t"              // *cr = tmp;
        //"\n\t"
        ::"r"(GPIO), "r"(pin), "r"(mode), "r"(conf):"r4"
         );
#else
#error Unknown GPIO_INIT_OPTIMIZE
#endif
}

void gpio_init(GPIO_TypeDef* GPIO, gpio_pin_t pins, gpio_mode_t mode, gpio_conf_t conf)
{
    // Текущий пин.
    gpio_pin_t pin;
    // Цикл по пинам.
    while(pins){
        // Следующий номер пина.
        pin = __RBIT(BIT(__CLZ((uint32_t)pins)));
        // Инициализируем.
        gpio_init_pin_impl(GPIO, pin, mode, conf);
        // Очищаем номер инициализированного пина.
        pins &= ~pin;
    }
}

