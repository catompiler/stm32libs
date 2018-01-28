/**
 * @file fixed_vec3.h Библиотека трёхмерного вектора для чисел с фиксированной запятой.
 */

#ifndef FIXED_VEC3_H
#define FIXED_VEC3_H

#include "fixed_math.h"
#include "fixed_vec2.h"
#include "defs/defs.h"
#include <stddef.h>
#include <stdbool.h>


//! Число компонентов.
#define VEC3_COMPONENTS_COUNT 3

//! Структура двухмерного вектора.
#pragma pack(push, 1)
typedef struct _Vec3 {
    union {
        f32_t v[VEC3_COMPONENTS_COUNT];
        struct {
            f32_t x;
            f32_t y;
            f32_t z;
        };
    };
} vec3_t;
#pragma pack(pop)



/**
 * Инициализирует вектор.
 * @param v Вектор.
 */
ALWAYS_INLINE static void vec3_init(vec3_t* v)
{
    v->x = 0;
    v->y = 0;
    v->z = 0;
}

/**
 * Обнуляет вектор.
 * @param v Вектор.
 */
ALWAYS_INLINE static void vec3_clear(vec3_t* v)
{
    v->x = 0;
    v->y = 0;
    v->z = 0;
}

/**
 * Устанавливает координаты вектора.
 * @param v Вектор.
 * @param x Координата X.
 * @param y Координата Y.
 * @param z Координата Z.
 */
ALWAYS_INLINE static void vec3_set(vec3_t* v, f32_t x, f32_t y, f32_t z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

/**
 * Устанавливает координаты вектора.
 * @param v Вектор.
 * @param p Координаты.
 */
ALWAYS_INLINE static void vec3_setv(vec3_t* v, const f32_t* p)
{
    v->x = p[0];
    v->y = p[1];
    v->z = p[2];
}

/**
 * Копирует данные одного вектора в другой.
 * @param v1 Назначение.
 * @param v2 Источник.
 */
ALWAYS_INLINE static void vec3_copy(vec3_t* v1, const vec3_t* v2)
{
    v1->x = v2->x;
    v1->y = v2->y;
    v1->z = v2->z;
}

/**
 * Получает координату X вектора.
 * @param v Вектор.
 * @return Координата X.
 */
ALWAYS_INLINE static f32_t vec3_x(const vec3_t* v)
{
    return v->x;
}

/**
 * Получает координату Y вектора.
 * @param v Вектор.
 * @return Координата Y.
 */
ALWAYS_INLINE static f32_t vec3_y(const vec3_t* v)
{
    return v->y;
}

/**
 * Получает координату Z вектора.
 * @param v Вектор.
 * @return Координата z.
 */
ALWAYS_INLINE static f32_t vec3_z(const vec3_t* v)
{
    return v->z;
}

/**
 * Получает двухмерный вектор XY.
 * @param v Вектор.
 * @param v_out Результат.
 */
ALWAYS_INLINE static void vec3_xy(const vec3_t* v, vec2_t* v_out)
{
    v_out->x = v->x;
    v_out->y = v->y;
}

/**
 * Получает компонент вектора по индексу.
 * @param v Вектор.
 * @param i Индекс.
 * @return Компонент вектора.
 */
ALWAYS_INLINE static f32_t vec3_at(const vec3_t* v, size_t i)
{
    return v->v[i];
}

/**
 * Получает флаг равенства векторов.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 * @return Флаг равенства.
 */
EXTERN bool vec3_equal(const vec3_t* v1, const vec3_t* v2);

/**
 * Складывает два вектора.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec3_add(vec3_t* v, const vec3_t* v1, const vec3_t* v2);

/**
 * Вычитает второй вектор из первого.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec3_sub(vec3_t* v, const vec3_t* v1, const vec3_t* v2);

/**
 * Покомпонентно умножает два вектора.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec3_mul(vec3_t* v, const vec3_t* v1, const vec3_t* v2);

/**
 * Умножает вектор на число.
 * @param v Результат.
 * @param v1 Вектор.
 * @param n Число.
 */
EXTERN void vec3_mul_n(vec3_t* v, const vec3_t* v1, f32_t n);

/**
 * Покомпонентно делит два вектора.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec3_div(vec3_t* v, const vec3_t* v1, const vec3_t* v2);

/**
 * Делит вектор на число.
 * @param v Результат.
 * @param v1 Вектор.
 * @param n Число.
 */
EXTERN void vec3_div_n(vec3_t* v, const vec3_t* v1, f32_t n);

/**
 * Получает длину вектора.
 * @param v Вектор.
 * @return Длина вектора.
 */
EXTERN f32_t vec3_length(const vec3_t* v);

/**
 * Получает расстояние между векторами.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 * @return Расстояние между векторами.
 */
EXTERN f32_t vec3_distance(const vec3_t* v1, const vec3_t* v2);

/**
 * Нормализует вектор.
 * @param v Результат.
 * @param v1 Вектор.
 * @return Длина вектора (при 0 нормализация не произведена).
 */
EXTERN f32_t vec3_normalize(vec3_t* v, const vec3_t* v1);

/**
 * Вычисляет скалярное произведение.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 * @return Скалярное произведение.
 */
EXTERN f32_t vec3_dot(const vec3_t* v1, const vec3_t* v2);

/**
 * Вычисляет векторное произведение.
 * @param v Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void vec3_cross(vec3_t* v, const vec3_t* v1, const vec3_t* v2);

/**
 * Ограничивает координаты вектора.
 * @param v Результат.
 * @param v1 Вектор.
 * @param v_min Минимальные координаты вектора.
 * @param v_max Максимальные координаты вектора.
 */
EXTERN void vec3_clamp(vec3_t* v, const vec3_t* v1, const vec3_t* v_min, const vec3_t* v_max);

#endif /* FIXED_VEC3_H */
