/**
 * @file fixed_vec2.h Библиотека двухмерного вектора для чисел с фиксированной запятой.
 */

#ifndef FIXED_VEC2_H
#define FIXED_VEC2_H

#include "fixed_math.h"
#include "defs/defs.h"
#include <stddef.h>
#include <stdbool.h>


//! Число компонентов.
#define VEC2_COMPONENTS_COUNT 2

//! Структура двухмерного вектора.
#pragma pack(push, 1)
typedef struct _Vec2 {
    union {
        f32_t v[VEC2_COMPONENTS_COUNT];
        struct {
            f32_t x;
            f32_t y;
        };
    };
} vec2_t;
#pragma pack(pop)



/**
 * Инициализирует вектор.
 * @param v Вектор.
 */
ALWAYS_INLINE static void vec2_init(vec2_t* v)
{
    v->x = 0;
    v->y = 0;
}

/**
 * Обнуляет вектор.
 * @param v Вектор.
 */
ALWAYS_INLINE static void vec2_clear(vec2_t* v)
{
    v->x = 0;
    v->y = 0;
}

/**
 * Устанавливает координаты вектора.
 * @param v Вектор.
 * @param x Координата X.
 * @param y Координата Y.
 */
ALWAYS_INLINE static void vec2_set(vec2_t* v, f32_t x, f32_t y)
{
    v->x = x;
    v->y = y;
}

/**
 * Устанавливает координаты вектора.
 * @param v Вектор.
 * @param p Координаты.
 */
ALWAYS_INLINE static void vec2_setv(vec2_t* v, const f32_t* p)
{
    v->x = p[0];
    v->y = p[1];
}

/**
 * Копирует данные одного вектора в другой.
 * @param v1 Назначение.
 * @param v2 Источник.
 */
ALWAYS_INLINE static void vec2_copy(vec2_t* v1, const vec2_t* v2)
{
    v1->x = v2->x;
    v1->y = v2->y;
}

/**
 * Получает координату X вектора.
 * @param v Вектор.
 * @return Координата X.
 */
ALWAYS_INLINE static f32_t vec2_x(const vec2_t* v)
{
    return v->x;
}

/**
 * Получает координату Y вектора.
 * @param v Вектор.
 * @return Координата Y.
 */
ALWAYS_INLINE static f32_t vec2_y(const vec2_t* v)
{
    return v->y;
}

/**
 * Получает компонент вектора по индексу.
 * @param v Вектор.
 * @param i Индекс.
 * @return Компонент вектора.
 */
ALWAYS_INLINE static f32_t vec2_at(const vec2_t* v, size_t i)
{
    return v->v[i];
}

/**
 * Получает флаг равенства векторов.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 * @return Флаг равенства.
 */
EXTERN bool vec2_equal(const vec2_t* v1, const vec2_t* v2);

/**
 * Складывает два вектора.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec2_add(vec2_t* v, const vec2_t* v1, const vec2_t* v2);

/**
 * Вычитает второй вектор из первого.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec2_sub(vec2_t* v, const vec2_t* v1, const vec2_t* v2);

/**
 * Покомпонентно умножает два вектора.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec2_mul(vec2_t* v, const vec2_t* v1, const vec2_t* v2);

/**
 * Умножает вектор на число.
 * @param v Результат.
 * @param v1 Вектор.
 * @param n Число.
 */
EXTERN void vec2_mul_n(vec2_t* v, const vec2_t* v1, f32_t n);

/**
 * Покомпонентно делит два вектора.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec2_div(vec2_t* v, const vec2_t* v1, const vec2_t* v2);

/**
 * Делит вектор на число.
 * @param v Результат.
 * @param v1 Вектор.
 * @param n Число.
 */
EXTERN void vec2_div_n(vec2_t* v, const vec2_t* v1, f32_t n);

/**
 * Получает длину вектора.
 * @param v Вектор.
 * @return Длина вектора.
 */
EXTERN f32_t vec2_length(const vec2_t* v);

/**
 * Получает расстояние между векторами.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 * @return Расстояние между векторами.
 */
EXTERN f32_t vec2_distance(const vec2_t* v1, const vec2_t* v2);

/**
 * Нормализует вектор.
 * @param v Результат.
 * @param v1 Вектор.
 * @return Длина вектора (при 0 нормализация не произведена).
 */
EXTERN f32_t vec2_normalize(vec2_t* v, const vec2_t* v1);

/**
 * Вычисляет скалярное произведение.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 * @return Скалярное произведение.
 */
EXTERN f32_t vec2_dot(const vec2_t* v1, const vec2_t* v2);

/**
 * Ограничивает координаты вектора.
 * @param v Результат.
 * @param v1 Вектор.
 * @param v_min Минимальные координаты вектора.
 * @param v_max Максимальные координаты вектора.
 */
EXTERN void vec2_clamp(vec2_t* v, const vec2_t* v1, const vec2_t* v_min, const vec2_t* v_max);

#endif /* FIXED_VEC2_H */
