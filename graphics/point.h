/**
 * @file rect.h Библиотека точки.
 */

#ifndef POINT_H
#define	POINT_H

#include "graphics.h"
#include "defs/defs.h"
#include <stdbool.h>

//! Структура точки.
typedef struct _Point {
    graphics_pos_t x; //!< Координата X.
    graphics_pos_t y; //!< Координата Y.
} point_t;

//! Инициализирует структуру точки.
#define MAKE_POINT(arg_x, arg_y)\
        { .x = arg_x, .y = arg_y }

/**
 * Инициализирует точку.
 * @param point Точка.
 */
ALWAYS_INLINE static void point_init(point_t* point)
{
    point->x = 0;
    point->y = 0;
}

/**
 * Инициализирует точку с заданной позицией.
 * @param point Точка.
 * @param x Координата X.
 * @param y Координата Y.
 */
ALWAYS_INLINE static void point_init_position(point_t* point, graphics_pos_t x, graphics_pos_t y)
{
    point->x = x;
    point->y = y;
}

/**
 * Устанавливает позицию точки.
 * @param point Точка.
 * @param x Координата X.
 * @param y Координата Y.
 */
ALWAYS_INLINE static void point_set_position(point_t* point, graphics_pos_t x, graphics_pos_t y)
{
    point->x = x;
    point->y = y;
}

/**
 * Получает координату X точки.
 * @param point Точка.
 * @return Координата X.
 */
ALWAYS_INLINE static graphics_pos_t point_x(point_t* point)
{
    return point->x;
}

/**
 * Устанавливает координату X точки.
 * @param point Точка.
 * @param x Координата X.
 */
ALWAYS_INLINE static void point_set_x(point_t* point, graphics_pos_t x)
{
    point->x = x;
}

/**
 * Получает координату Y точки.
 * @param point Точка.
 * @return Координата Y.
 */
ALWAYS_INLINE static graphics_pos_t point_y(point_t* point)
{
    return point->y;
}

/**
 * Устанавливает координату Y точки.
 * @param point Точка.
 * @param y Координата Y.
 */
ALWAYS_INLINE static void point_set_y(point_t* point, graphics_pos_t y)
{
    point->y = y;
}

/**
 * Перемещает точку в заданную позицию.
 * @param point Точка.
 * @param x Координата X.
 * @param y Координата Y.
 */
ALWAYS_INLINE static void point_move(point_t* point, graphics_pos_t x, graphics_pos_t y)
{
    point->x = x;
    point->y = y;
}

#endif	/* POINT_H */
