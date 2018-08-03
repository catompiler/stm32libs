/**
 * @file gpio.h Библиотека GPIO.
 */
#ifndef GPIO_H_
#define GPIO_H_

#include <stm32f10x.h>
#include <stdint.h>
#include "bits/bits.h"
#include "defs/defs.h"

// Пины.
#define GPIO_PIN_0 BIT(0)
#define GPIO_PIN_1 BIT(1)
#define GPIO_PIN_2 BIT(2)
#define GPIO_PIN_3 BIT(3)
#define GPIO_PIN_4 BIT(4)
#define GPIO_PIN_5 BIT(5)
#define GPIO_PIN_6 BIT(6)
#define GPIO_PIN_7 BIT(7)
#define GPIO_PIN_8 BIT(8)
#define GPIO_PIN_9 BIT(9)
#define GPIO_PIN_10 BIT(10)
#define GPIO_PIN_11 BIT(11)
#define GPIO_PIN_12 BIT(12)
#define GPIO_PIN_13 BIT(13)
#define GPIO_PIN_14 BIT(14)
#define GPIO_PIN_15 BIT(15)

//! Тип пина GPIO.
typedef uint16_t gpio_pin_t;

// Состояние пина.
//! Выключено.
#define GPIO_STATE_OFF 0
//! Включено.
#define GPIO_STATE_ON  1

//! Тип состояния пина.
typedef uint16_t gpio_state_t;

// Режимы GPIO.
//! Вход.
#define GPIO_MODE_IN         0
//! Выход 10 МГц.
#define GPIO_MODE_OUT_10MHz  1
//! Выход 2 МГц.
#define GPIO_MODE_OUT_2MHz   2
//! Выход 50 МГц.
#define GPIO_MODE_OUT_50MHz  3

//! Тип режима GPIO.
typedef uint32_t gpio_mode_t;

// Конфигурация GPIO.
// Входы.
//! Аналоговый вход.
#define GPIO_CONF_IN_ANALOG   0
//! Плавающий вход.
#define GPIO_CONF_IN_FLOATING 1
//! Вход с подтяжкой.
#define GPIO_CONF_IN_PUPD     2
//! Зарезервировано.
#define GPIO_CONF_IN_RESERVED 3
// Выходы.
//! Выход Push-Pull общего назначения.
#define GPIO_CONF_OUT_GP_PP   0
//! Выход Open Drain общего назначения.
#define GPIO_CONF_OUT_GP_OD   1
//! Выход Push-Pull альтернативных функций.
#define GPIO_CONF_OUT_AF_PP   2
//! Выход Open Drain альтернативных функций.
#define GPIO_CONF_OUT_AF_OD   3

//! Тип конфигурации пина.
typedef uint32_t gpio_conf_t;


/**
 * Инициализирует пин.
 * @param GPIO Порт.
 * @param pins Маска пинов.
 * @param mode Режим пина.
 * @param conf Конфигурация пина.
 */
EXTERN void gpio_init(GPIO_TypeDef* GPIO, gpio_pin_t pins, gpio_mode_t mode, gpio_conf_t conf);

/**
 * Устанавливает биты пинов.
 * @param GPIO Порт.
 * @param pins Маска пинов.
 */
ALWAYS_INLINE static void gpio_set(GPIO_TypeDef* GPIO, gpio_pin_t pins)
{
    GPIO->BSRR = pins;
}

/**
 * Сбрасывает биты пинов.
 * @param GPIO Порт.
 * @param pins Маска пинов.
 */
ALWAYS_INLINE static void gpio_reset(GPIO_TypeDef* GPIO, gpio_pin_t pins)
{
    GPIO->BRR = pins;
}

/**
 * Изменяет биты пинов.
 * @param GPIO Порт.
 * @param pins Маска пинов.
 */
ALWAYS_INLINE static void gpio_toggle(GPIO_TypeDef* GPIO, gpio_pin_t pins)
{
    GPIO->ODR ^= pins;
}

/**
 * Устанавливает подтяжку пина к питанию.
 * @param GPIO Порт.
 * @param pins Маска пинов.
 */
ALWAYS_INLINE static void gpio_set_pullup(GPIO_TypeDef* GPIO, gpio_pin_t pins)
{
    GPIO->BSRR = pins;
}

/**
 * Сбрасывает бит пина.
 * @param GPIO Порт.
 * @param pins Маска пинов.
 */
ALWAYS_INLINE static void gpio_set_pulldown(GPIO_TypeDef* GPIO, gpio_pin_t pins)
{
    GPIO->BRR = pins;
}

/**
 * Получает входное состояние пина.
 * @param GPIO Порт.
 * @param pin Пин.
 * @return Входное состояние пина.
 */
ALWAYS_INLINE static gpio_state_t gpio_input_state(GPIO_TypeDef* GPIO, gpio_pin_t pin)
{
    return __USAT(GPIO->IDR & pin, 1);
}

/**
 * Получает выходное состояние пина.
 * @param GPIO Порт.
 * @param pin Пин.
 * @return Выходное состояние пина.
 */
ALWAYS_INLINE static gpio_state_t gpio_output_state(GPIO_TypeDef* GPIO, gpio_pin_t pin)
{
    return __USAT(GPIO->ODR & pin, 1);
}

/**
 * Получает входное значение пинов.
 * @param GPIO Порт.
 * @param pins Маска пинов.
 * @return Входное значение пина.
 */
ALWAYS_INLINE static uint16_t gpio_input_value(GPIO_TypeDef* GPIO, gpio_pin_t pins)
{
    return GPIO->IDR & pins;
}

/**
 * Получает выходное значение пинов.
 * @param GPIO Порт.
 * @param pins Маска пинов.
 * @return Выходное значение пинов.
 */
ALWAYS_INLINE static uint16_t gpio_output_value(GPIO_TypeDef* GPIO, gpio_pin_t pins)
{
    return GPIO->ODR & pins;
}

/**
 * Устанавливает выходное значение пинов.
 * @param GPIO Порт.
 * @param pins Маска пинов.
 * @param val Выходное значение пинов.
 */
ALWAYS_INLINE static void gpio_set_output_value(GPIO_TypeDef* GPIO, gpio_pin_t pins, uint16_t val)
{
    GPIO->BSRR = pins & val;
    GPIO->BRR  = pins & ~val;
}

#endif /* GPIO_H_ */
