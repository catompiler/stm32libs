/**
 * @file cordic.h
 * Библиотека для вычисления тригонометрических функций алгоритмом CORDIC,
 * используя 32-битные числа с фиксированной запятой.
 */
#ifndef CORDIC32_H
#define	CORDIC32_H

#include <stdint.h>
#include <stddef.h>
#include "fixed/fixed32.h"
#include "defs/defs.h"

// Углы.
#define CORDIC32_ANGLE_90  5898240
#define CORDIC32_ANGLE_180 11796480
#define CORDIC32_ANGLE_270 17694720
#define CORDIC32_ANGLE_360 23592960

/**
 * Вычисляет угол и радиус от значений X и Y.
 * @param x Координата X.
 * @param y Координата Y.
 * @param angle Результат - угол, может быть передан NULL.
 * @param hyp Результат - радиус, может быть передан NULL.
 */
EXTERN void cordic32_atan2_hyp(fixed32_t x, fixed32_t y, fixed32_t* angle, fixed32_t* hyp);

/**
 * Вычисляет синус и косинус угла.
 * @param angle Угол.
 * @param sin Результат: синус угла, может быть передан NULL.
 * @param cos Результат: косинус угла, может быть передан NULL.
 */
EXTERN void cordic32_sincos(fixed32_t angle, fixed32_t* sin, fixed32_t* cos);

#endif	/* CORDIC32_H */
