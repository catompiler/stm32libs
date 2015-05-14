/**
 * @file pid_controller.h Библиотека ПИД-регулятора.
 */

#ifndef PID_CONTROLLER_H
#define	PID_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "fixed/fixed32.h"
#include "defs/defs.h"

/**
 * Структура ПИД-регулятора.
 */
typedef struct _PidController {
    fixed32_t kp; //!< Пропорциональный коэффициент.
    fixed32_t ki; //!< Интегральный коэффициент.
    fixed32_t kd; //!< Дифференциальный коэффициент.
    fixed32_t prev_e; //!< Предыдущее значение ошибки.
    fixed32_t prev_i; //!< Предыдущее значение интегрального звена.
    fixed32_t clamp_min; //!< Минимальное значение.
    fixed32_t clamp_max; //!< Максимальное значение.
    fixed32_t value; //!< Текущее значение.
} pid_controller_t;

/**
 * Инициализирует ПИД-регулятор.
 * @param controller ПИД-регулятор.
 * @param kp Пропорциональный коэффициент.
 * @param ki Интегральный коэффициент.
 * @param kd Дифференциальный коэффициент.
 */
extern void pid_controller_init(pid_controller_t* controller, fixed32_t kp, fixed32_t ki, fixed32_t kd);

/**
 * Сбрасывает ПИД-регулятор.
 * @param controller ПИД-регулятор.
 */
extern void pid_controller_reset(pid_controller_t* controller);

/**
 * Вычисляет значение регулирования.
 * @param controller ПИД-регулятор.
 * @param e Текущая ошибка.
 * @param dt Время с последнего вычисления.
 * @return Успешность вычисления значения регулирования.
 */
extern bool pid_controller_calculate(pid_controller_t* controller, fixed32_t e, fixed32_t dt);

/**
 * Получает значение регулирования.
 * @param controller ПИД-регулятор.
 * @return Значение регулирования.
 */
ALWAYS_INLINE static fixed32_t pid_controller_value(pid_controller_t* controller)
{
    return controller->value;
}

/**
 * Получает пропорциональный коэффициент.
 * @param controller ПИД-регулятор.
 * @return Пропорциональный коэффициент.
 */
ALWAYS_INLINE static fixed32_t pid_controller_kp(pid_controller_t* controller)
{
    return controller->kp;
}

/**
 * Устанавливает пропорциональный коэффициент.
 * @param controller ПИД-регулятор.
 * @param kp Пропорциональный коэффициент.
 */
ALWAYS_INLINE static void pid_controller_set_kp(pid_controller_t* controller, fixed32_t kp)
{
    controller->kp = kp;
}

/**
 * Получает интегральный коэффициент.
 * @param controller ПИД-регулятор.
 * @return Интегральный коэффициент.
 */
ALWAYS_INLINE static fixed32_t pid_controller_ki(pid_controller_t* controller)
{
    return controller->ki;
}

/**
 * Устанавливает интегральный коэффициент.
 * @param controller ПИД-регулятор.
 * @param kp Интегральный коэффициент.
 */
ALWAYS_INLINE static void pid_controller_set_ki(pid_controller_t* controller, fixed32_t ki)
{
    controller->ki = ki;
}

/**
 * Получает дифференциальный коэффициент.
 * @param controller ПИД-регулятор.
 * @return Дифференциальный коэффициент.
 */
ALWAYS_INLINE static fixed32_t pid_controller_kd(pid_controller_t* controller)
{
    return controller->kd;
}

/**
 * Устанавливает дифференциальный коэффициент.
 * @param controller ПИД-регулятор.
 * @param kp Дифференциальный коэффициент.
 */
ALWAYS_INLINE static void pid_controller_set_kd(pid_controller_t* controller, fixed32_t kd)
{
    controller->kd = kd;
}

/**
 * Устанавливает значения отсечения.
 * @param controller ПИД-регулятор.
 * @param clamp_min Минимальное значение.
 * @param clamp_max Максимальное значение.
 */
ALWAYS_INLINE static void pid_controller_clamp(pid_controller_t* controller, fixed32_t clamp_min, fixed32_t clamp_max)
{
    controller->clamp_min = clamp_min;
    controller->clamp_max = clamp_max;
}

/**
 * Получает ширину отсечения.
 * @param controller ПИД-регулятор.
 * @return Ширина отсечения.
 */
ALWAYS_INLINE static fixed32_t pid_controller_clamp_width(pid_controller_t* controller)
{
    return controller->clamp_max - controller->clamp_min;
}

#endif	/* PID_CONTROLLER_H */

