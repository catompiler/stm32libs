/**
 * @file fixed_vec4.h Библиотека четырёхмерного вектора для чисел с фиксированной запятой.
 */

#ifndef FIXED_VEC4_H
#define FIXED_VEC4_H

#include "fixed_math.h"
#include "fixed_vec2.h"
#include "fixed_vec3.h"
#include "defs/defs.h"
#include <stddef.h>
#include <stdbool.h>


//! Число компонентов.
#define VEC4_COMPONENTS_COUNT 4

//! Структура двухмерного вектора.
#pragma pack(push, 1)
typedef struct _Vec4 {
    union {
        f32_t v[VEC4_COMPONENTS_COUNT];
        struct {
            f32_t x;
            f32_t y;
            f32_t z;
            f32_t w;
        };
    };
} vec4_t;
#pragma pack(pop)



/**
 * Инициализирует вектор.
 * @param v Вектор.
 */
ALWAYS_INLINE static void vec4_init(vec4_t* v)
{
    v->x = 0;
    v->y = 0;
    v->z = 0;
    v->w = 0;
}

/**
 * Обнуляет вектор.
 * @param v Вектор.
 */
ALWAYS_INLINE static void vec4_clear(vec4_t* v)
{
    v->x = 0;
    v->y = 0;
    v->z = 0;
    v->w = 0;
}

/**
 * Устанавливает координаты вектора.
 * @param v Вектор.
 * @param x Координата X.
 * @param y Координата Y.
 * @param z Координата Z.
 * @param w Координата W.
 */
ALWAYS_INLINE static void vec4_set(vec4_t* v, f32_t x, f32_t y, f32_t z, f32_t w)
{
    v->x = x;
    v->y = y;
    v->z = z;
    v->w = w;
}

/**
 * Устанавливает координаты вектора.
 * @param v Вектор.
 * @param p Координаты.
 */
ALWAYS_INLINE static void vec4_setv(vec4_t* v, const f32_t* p)
{
    v->x = p[0];
    v->y = p[1];
    v->z = p[2];
    v->w = p[3];
}

/**
 * Копирует данные одного вектора в другой.
 * @param v1 Назначение.
 * @param v2 Источник.
 */
ALWAYS_INLINE static void vec4_copy(vec4_t* v1, const vec4_t* v2)
{
    v1->x = v2->x;
    v1->y = v2->y;
    v1->z = v2->z;
    v1->w = v2->w;
}

/**
 * Получает координату X вектора.
 * @param v Вектор.
 * @return Координата X.
 */
ALWAYS_INLINE static f32_t vec4_x(const vec4_t* v)
{
    return v->x;
}

/**
 * Получает координату Y вектора.
 * @param v Вектор.
 * @return Координата Y.
 */
ALWAYS_INLINE static f32_t vec4_y(const vec4_t* v)
{
    return v->y;
}

/**
 * Получает координату Z вектора.
 * @param v Вектор.
 * @return Координата z.
 */
ALWAYS_INLINE static f32_t vec4_z(const vec4_t* v)
{
    return v->z;
}

/**
 * Получает координату W вектора.
 * @param v Вектор.
 * @return Координата w.
 */
ALWAYS_INLINE static f32_t vec4_w(const vec4_t* v)
{
    return v->w;
}

/**
 * Получает двухмерный вектор XY.
 * @param v Вектор.
 * @param v_out Результат.
 */
ALWAYS_INLINE static void vec4_xy(const vec4_t* v, vec2_t* v_out)
{
    v_out->x = v->x;
    v_out->y = v->y;
}

/**
 * Получает трёхмерный вектор XYZ.
 * @param v Вектор.
 * @param v_out Результат.
 */
ALWAYS_INLINE static void vec4_xyz(const vec4_t* v, vec3_t* v_out)
{
    v_out->x = v->x;
    v_out->y = v->y;
    v_out->z = v->z;
}

/**
 * Получает компонент вектора по индексу.
 * @param v Вектор.
 * @param i Индекс.
 * @return Компонент вектора.
 */
ALWAYS_INLINE static f32_t vec4_at(const vec4_t* v, size_t i)
{
    return v->v[i];
}

/**
 * Получает флаг равенства векторов.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 * @return Флаг равенства.
 */
EXTERN bool vec4_equal(const vec4_t* v1, const vec4_t* v2);

/**
 * Складывает два вектора.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec4_add(vec4_t* v, const vec4_t* v1, const vec4_t* v2);

/**
 * Вычитает второй вектор из первого.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec4_sub(vec4_t* v, const vec4_t* v1, const vec4_t* v2);

/**
 * Покомпонентно умножает два вектора.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec4_mul(vec4_t* v, const vec4_t* v1, const vec4_t* v2);

/**
 * Умножает вектор на число.
 * @param v Результат.
 * @param v1 Вектор.
 * @param n Число.
 */
EXTERN void vec4_mul_n(vec4_t* v, const vec4_t* v1, f32_t n);

/**
 * Покомпонентно делит два вектора.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec4_div(vec4_t* v, const vec4_t* v1, const vec4_t* v2);

/**
 * Делит вектор на число.
 * @param v Результат.
 * @param v1 Вектор.
 * @param n Число.
 */
EXTERN void vec4_div_n(vec4_t* v, const vec4_t* v1, f32_t n);

/**
 * Получает длину вектора.
 * @param v Вектор.
 * @return Длина вектора.
 */
EXTERN f32_t vec4_length(const vec4_t* v);

/**
 * Получает расстояние между векторами.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 * @return Расстояние между векторами.
 */
EXTERN f32_t vec4_distance(const vec4_t* v1, const vec4_t* v2);

/**
 * Нормализует вектор.
 * @param v Результат.
 * @param v1 Вектор.
 * @return Длина вектора (при 0 нормализация не произведена).
 */
EXTERN f32_t vec4_normalize(vec4_t* v, const vec4_t* v1);

/**
 * Вычисляет скалярное произведение.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 * @return Скалярное произведение.
 */
EXTERN f32_t vec4_dot(const vec4_t* v1, const vec4_t* v2);

/**
 * Ограничивает координаты вектора.
 * @param v Результат.
 * @param v1 Вектор.
 * @param v_min Минимальные координаты вектора.
 * @param v_max Максимальные координаты вектора.
 */
EXTERN void vec4_clamp(vec4_t* v, const vec4_t* v1, const vec4_t* v_min, const vec4_t* v_max);

#endif /* FIXED_VEC4_H */
