/**
 * @file rect.h Библиотека прямоугольной области.
 */

#ifndef RECT_H
#define	RECT_H

#include "graphics.h"
#include <stdbool.h>
#include "defs/defs.h"
#include "point.h"

//! Структура прямоугольной области.
typedef struct _Rect {
    graphics_pos_t left; //!< Лево.
    graphics_pos_t top; //!< Верх.
    graphics_pos_t right; //!< Право.
    graphics_pos_t bottom; //!< Низ.
} rect_t;

//! Инициализирует структуру прямоугольной области.
#define MAKE_RECT(arg_left, arg_top, arg_right, arg_bottom)\
        { .left = arg_left, .top = arg_top, .right = arg_right, .bottom = arg_bottom }

/**
 * Инициализирует прямоугольную область.
 * @param rect Прямоугольная область.
 */
ALWAYS_INLINE static void rect_init(rect_t* rect)
{
    rect->left = 0;
    rect->top = 0;
    rect->right = 0;
    rect->bottom = 0;
}

/**
 * Инициализирует прямоугольную область заданной позиции.
 * @param rect Прямоугольная область.
 * @param left Лево.
 * @param top Верх.
 * @param right Право.
 * @param bottom Низ.
 */
ALWAYS_INLINE static void rect_init_position(rect_t* rect, graphics_pos_t left, graphics_pos_t top, graphics_pos_t right, graphics_pos_t bottom)
{
    rect->left = left;
    rect->top = top;
    rect->right = right;
    rect->bottom = bottom;
}

/**
 * Устанавливает позицию прямоугольной области.
 * @param rect Прямоугольная область.
 * @param left Лево.
 * @param top Верх.
 * @param right Право.
 * @param bottom Низ.
 */
ALWAYS_INLINE static void rect_set_position(rect_t* rect, graphics_pos_t left, graphics_pos_t top, graphics_pos_t right, graphics_pos_t bottom)
{
    rect->left = left;
    rect->top = top;
    rect->right = right;
    rect->bottom = bottom;
}

/**
 * Получает положение левого края прямоугольной области.
 * @param rect Прямоугольная область.
 * @return Положение левого края прямоугольной области.
 */
ALWAYS_INLINE static graphics_pos_t rect_left(const rect_t* rect)
{
    return rect->left;
}

/**
 * Устанавливает положение левого края прямоугольной области.
 * @param rect Прямоугольная область.
 * @param left Положение левого края прямоугольной области.
 */
ALWAYS_INLINE static void rect_set_left(rect_t* rect, graphics_pos_t left)
{
    rect->left = left;
}

/**
 * Получает положение верхнего края прямоугольной области.
 * @param rect Прямоугольная область.
 * @return Положение верхнего края прямоугольной области.
 */
ALWAYS_INLINE static graphics_pos_t rect_top(const rect_t* rect)
{
    return rect->top;
}

/**
 * Устанавливает положение верхнего края прямоугольной области.
 * @param rect Прямоугольная область.
 * @param top Положение верхнего края прямоугольной области.
 */
ALWAYS_INLINE static void rect_set_top(rect_t* rect, graphics_pos_t top)
{
    rect->top = top;
}

/**
 * Получает положение правого края прямоугольной области.
 * @param rect Прямоугольная область.
 * @return Положение правого края прямоугольной области.
 */
ALWAYS_INLINE static graphics_pos_t rect_right(const rect_t* rect)
{
    return rect->right;
}

/**
 * Устанавливает положение правого края прямоугольной области.
 * @param rect Прямоугольная область.
 * @param right Положение правого края прямоугольной области.
 */
ALWAYS_INLINE static void rect_set_right(rect_t* rect, graphics_pos_t right)
{
    rect->right = right;
}

/**
 * Получает положение нижнего края прямоугольной области.
 * @param rect Прямоугольная область.
 * @return Положение нижнего края прямоугольной области.
 */
ALWAYS_INLINE static graphics_pos_t rect_bottom(const rect_t* rect)
{
    return rect->bottom;
}

/**
 * Устанавливает положение нижнего края прямоугольной области.
 * @param rect Прямоугольная область.
 * @param bottom Положение нижнего края прямоугольной области.
 */
ALWAYS_INLINE static void rect_set_bottom(rect_t* rect, graphics_pos_t bottom)
{
    rect->bottom = bottom;
}

/**
 * Получает координату X прямоугольной области.
 * @param rect Прямоугольная область.
 * @return Координата X прямоугольной области.
 */
ALWAYS_INLINE static graphics_pos_t rect_x(const rect_t* rect)
{
    return rect->left;
}

/**
 * Устанавливает координату X прямоугольной области.
 * @param rect Прямоугольная область.
 * @param x Координата X прямоугольной области.
 */
ALWAYS_INLINE static void rect_set_x(rect_t* rect, graphics_pos_t x)
{
    rect->left = x;
}

/**
 * Получает координату Y прямоугольной области.
 * @param rect Прямоугольная область.
 * @return Координата Y прямоугольной области.
 */
ALWAYS_INLINE static graphics_pos_t rect_y(const rect_t* rect)
{
    return rect->top;
}

/**
 * Устанавливает координату Y прямоугольной области.
 * @param rect Прямоугольная область.
 * @param y Координата Y прямоугольной области.
 */
ALWAYS_INLINE static void rect_set_y(rect_t* rect, graphics_pos_t y)
{
    rect->top = y;
}

/**
 * Получает ширину прямоугольной области.
 * @param rect Прямоугольная область.
 * @return Ширина прямоугольной области.
 */
ALWAYS_INLINE static graphics_pos_t rect_width(const rect_t* rect)
{
    return rect->right - rect->left;
}

/**
 * Устанавливает ширину прямоугольной области.
 * @param rect Прямоугольная область.
 * @param width Ширина прямоугольной области.
 */
ALWAYS_INLINE static void rect_set_width(rect_t* rect, graphics_pos_t width)
{
    rect->right = rect->left + width;
}

/**
 * Получает высоту прямоугольной области.
 * @param rect Прямоугольная область.
 * @return Высота прямоугольной области.
 */
ALWAYS_INLINE static graphics_pos_t rect_height(const rect_t* rect)
{
    return rect->bottom - rect->top;
}

/**
 * Устанавливает высоту прямоугольной области.
 * @param rect Прямоугольная область.
 * @param height Высота прямоугольной области.
 */
ALWAYS_INLINE static void rect_set_height(rect_t* rect, graphics_pos_t height)
{
    rect->bottom = rect->top + height;
}

/**
 * Перемещает прямоугольную область.
 * @param rect Прямоугольная область.
 * @param x Новая координата X.
 * @param y Новая координата Y.
 */
ALWAYS_INLINE static void rect_move(rect_t* rect, graphics_pos_t x, graphics_pos_t y)
{
    rect->right = rect->right - rect->left + x;
    rect->bottom = rect->bottom - rect->top + y;
    rect->left = x;
    rect->top = y;
}

/**
 * Изменяет размер прямоугольнуой области.
 * @param rect Прямоугольная область.
 * @param width Новая ширина.
 * @param height Новая высота.
 */
ALWAYS_INLINE static void rect_resize(rect_t* rect, graphics_pos_t width, graphics_pos_t height)
{
    rect->right = rect->left + width;
    rect->bottom = rect->top + height;
}

/**
 * Проверяет принадлежность заданной точки прямоугольной области.
 * @param rect Прямоугольная область.
 * @param x Координата X точки.
 * @param y Координата Y точки.
 * @return Флаг принадлежности заданной точки прямоугольной области.
 */
ALWAYS_INLINE static bool rect_contains(rect_t* rect, graphics_pos_t x, graphics_pos_t y)
{
    return (x >= rect->left) && (x <= rect->right) &&
           (y >= rect->top) && (y <= rect->bottom);
}

/**
 * Проверяет принадлежность заданной точки прямоугольной области.
 * @param rect Прямоугольная область.
 * @param point Точка.
 * @return Флаг принадлежности заданной точки прямоугольной области.
 */
ALWAYS_INLINE static bool rect_contains_point(rect_t* rect, point_t* point)
{
    return (point->x >= rect->left) && (point->x <= rect->right) &&
           (point->y >= rect->top) && (point->y <= rect->bottom);
}

/**
 * Отсекает прямоугольную область по заданной границе.
 * @param rect Прямоугольная область.
 * @param border_rect Граница.
 */
ALWAYS_INLINE static void rect_clip(rect_t* rect, const rect_t* border_rect)
{
    if(rect->left   < border_rect->left)   rect->left = border_rect->left;
    if(rect->top    < border_rect->top)    rect->top = border_rect->top;
    if(rect->right  > border_rect->right)  rect->right = border_rect->right;
    if(rect->bottom > border_rect->bottom) rect->bottom = border_rect->bottom;
}

#endif	/* RECT_H */

