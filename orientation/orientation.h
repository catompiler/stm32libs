/**
 * @file orientation.h
 * Библиотека для вычисления ориентации.
 */

#ifndef ORIENTATION_H
#define	ORIENTATION_H

#include <stdint.h>
#include "defs/defs.h"
#include "fixed/fixed32.h"
#include "counter/counter.h"
#include "gyro6050/gyro6050.h"

/**
 * Структура ориентации.
 */
typedef struct _Orientation {
    //! Гироскоп.
    const gyro6050_t* gyro;
    //! Предыдущее значение угловой скорости по оси X.
    fixed32_t gyro_w_x_prev;
    //! Предыдущее значение угловой скорости по оси Y.
    fixed32_t gyro_w_y_prev;
    //! Предыдущее значение угловой скорости по оси Z.
    fixed32_t gyro_w_z_prev;
    
    //! Счётчик времени.
    counter_t counter;
    
    //! Вес угла по данным акселерометра.
    uint8_t accel_angle_weight;
    
    //! Угол по оси X.
    fixed32_t angle_x;
    //! Угол по оси Y.
    fixed32_t angle_y;
    //! Угол по оси Z.
    fixed32_t angle_z;
} orientation_t;

#define ORIENTATION_ACCEL_ANGLE_WEIGHT_MAX 100
#define ORIENTATION_ACCEL_ANGLE_WEIGHT_DEFAULT 4

/**
 * Инициализирует ориентацию.
 * @param orientation Ориентация.
 */
extern void orientation_init(orientation_t* orientation, const gyro6050_t* gyro);

/**
 * Получает вес угла по данным акселерометра.
 * @param orientation Ориентация.
 * @return Вес угла по данным акселерометра.
 */
extern uint8_t orientation_accel_angle_weight(orientation_t* orientation);

/**
 * Устанавливает вес угла по данным акселерометра.
 * @param orientation Ориентация.
 * @param weight Вес, значение осекается в промежуток от 0 до 100.
 */
extern void orientation_set_accel_angle_weight(orientation_t* orientation, uint8_t weight);

/**
 * Вычисляет ориентацию.
 * @param orientation Ориентация.
 */
extern void orientation_calculate(orientation_t* orientation);

/**
 * Получает угол по оси X.
 * @param orientation Ориентация.
 * @return Угол по оси X.
 */
static ALWAYS_INLINE fixed32_t orientation_angle_x(orientation_t* orientation)
{
    return orientation->angle_x;
}

/**
 * Получает угол по оси Y.
 * @param orientation Ориентация.
 * @return Угол по оси Y.
 */
static ALWAYS_INLINE fixed32_t orientation_angle_y(orientation_t* orientation)
{
    return orientation->angle_y;
}

/**
 * Получает угол по оси Z.
 * @param orientation Ориентация.
 * @return Угол по оси Z.
 */
static ALWAYS_INLINE fixed32_t orientation_angle_z(orientation_t* orientation)
{
    return orientation->angle_z;
}

#endif	/* ORIENTATION_H */

