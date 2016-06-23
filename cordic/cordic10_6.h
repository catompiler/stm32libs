/**
 * @file cordic.h
 * Библиотека для вычисления тригонометрических функций алгоритмом CORDIC,
 * используя 10_6-битные числа с фиксированной запятой.
 */
#ifndef CORDIC10_6_H
#define	CORDIC10_6_H

#include <stdint.h>
#include <stddef.h>
#include "fixed/fixed16.h"
#include "defs/defs.h"

// Углы.
#define CORDIC10_6_ANGLE_90 5760
#define CORDIC10_6_ANGLE_180 11520
#define CORDIC10_6_ANGLE_270 17280
#define CORDIC10_6_ANGLE_360 23040

/**
 * Вычисляет угол и радиус от значений X и Y.
 * @param x Координата X.
 * @param y Координата Y.
 * @param angle Результат - угол, может быть передан NULL.
 * @param hyp Результат - радиус, может быть передан NULL.
 */
EXTERN void cordic10_6_atan2_hyp(fixed10_6_t x, fixed10_6_t y, fixed10_6_t* angle, fixed10_6_t* hyp);

/**
 * Вычисляет синус и косинус угла.
 * @param angle Угол.
 * @param sin Результат: синус угла, может быть передан NULL.
 * @param cos Результат: косинус угла, может быть передан NULL.
 */
EXTERN void cordic10_6_sincos(fixed10_6_t angle, fixed10_6_t* sin, fixed10_6_t* cos);

#endif	/* CORDIC10_6_H */
