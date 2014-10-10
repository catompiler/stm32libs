/**
 * @file cordic.h
 * Библиотека для вычисления тригонометрических функций алгоритмом CORDIC,
 * используя 16-битные числа с фиксированной запятой.
 */
#ifndef CORDIC16_H
#define	CORDIC16_H

#include <stdint.h>
#include <stddef.h>
#include "fixed/fixed16.h"

// Углы.
#define CORDIC16_ANGLE_90 23040

/**
 * Вычисляет угол и радиус от значений X и Y.
 * @param x Координата X.
 * @param y Координата Y.
 * @param angle Результат - угол, может быть передан NULL.
 * @param hyp Результат - радиус, может быть передан NULL.
 */
void cordic16_atan2_hyp(fixed16_t x, fixed16_t y, fixed16_t* angle, fixed16_t* hyp);

/**
 * Вычисляет синус и косинус угла.
 * @param angle Угол.
 * @param sin Результат: синус угла, может быть передан NULL.
 * @param cos Результат: косинус угла, может быть передан NULL.
 */
void cordic16_sincos(fixed16_t angle, fixed16_t* sin, fixed16_t* cos);

#endif	/* CORDIC16_H */

