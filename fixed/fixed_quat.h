/**
 * @file fixed_quat.h Библиотека кватернион для чисел с фиксированной запятой.
 */

#ifndef FIXED_QUAT_H
#define FIXED_QUAT_H

#include "fixed_math.h"
#include "fixed_vec2.h"
#include "fixed_vec3.h"
#include "fixed_vec4.h"
#include "defs/defs.h"
#include <stddef.h>
#include <stdbool.h>


//! Число компонентов.
#define QUAT_COMPONENTS_COUNT 4

//! Структура двухмерного кватерниона.
#pragma pack(push, 1)
typedef struct _Quat {
    union {
        f32_t q[QUAT_COMPONENTS_COUNT];
        struct {
            f32_t x;
            f32_t y;
            f32_t z;
            f32_t w;
        };
    };
} quat_t;
#pragma pack(pop)



/**
 * Инициализирует кватернион.
 * @param q Кватернион.
 */
ALWAYS_INLINE static void quat_init(quat_t* q)
{
    q->x = 0;
    q->y = 0;
    q->z = 0;
    q->w = 0;
}

/**
 * Обнуляет кватернион.
 * @param q Кватернион.
 */
ALWAYS_INLINE static void quat_clear(quat_t* q)
{
    q->x = 0;
    q->y = 0;
    q->z = 0;
    q->w = 0;
}

/**
 * Инициализирует единичный кватернион.
 * @param q Кватернион.
 */
ALWAYS_INLINE static void quat_identity(quat_t* q)
{
    q->x = 0;
    q->y = 0;
    q->z = 0;
    q->w = F32I(1);
}

/**
 * Устанавливает компоненты кватерниона.
 * @param q Кватернион.
 * @param x Компонента X.
 * @param y Компонента Y.
 * @param z Компонента Z.
 * @param w Компонента W.
 */
ALWAYS_INLINE static void quat_set(quat_t* q, f32_t x, f32_t y, f32_t z, f32_t w)
{
    q->x = x;
    q->y = y;
    q->z = z;
    q->w = w;
}

/**
 * Устанавливает компоненты кватерниона.
 * @param q Кватернион.
 * @param p Компоненты.
 */
ALWAYS_INLINE static void quat_setv(quat_t* q, const f32_t* p)
{
    q->x = p[0];
    q->y = p[1];
    q->z = p[2];
    q->w = p[3];
}

/**
 * Копирует данные одного кватерниона в другой.
 * @param q1 Назначение.
 * @param q2 Источник.
 */
ALWAYS_INLINE static void quat_copy(quat_t* q1, const quat_t* q2)
{
    q1->x = q2->x;
    q1->y = q2->y;
    q1->z = q2->z;
    q1->w = q2->w;
}

/**
 * Получает компоненту X кватерниона.
 * @param q Кватернион.
 * @return Компонента X.
 */
ALWAYS_INLINE static f32_t quat_x(const quat_t* q)
{
    return q->x;
}

/**
 * Получает компоненту Y кватерниона.
 * @param q Кватернион.
 * @return Компонента Y.
 */
ALWAYS_INLINE static f32_t quat_y(const quat_t* q)
{
    return q->y;
}

/**
 * Получает компоненту Z кватерниона.
 * @param q Кватернион.
 * @return Компонента z.
 */
ALWAYS_INLINE static f32_t quat_z(const quat_t* q)
{
    return q->z;
}

/**
 * Получает компоненту W кватерниона.
 * @param q Кватернион.
 * @return Компонента w.
 */
ALWAYS_INLINE static f32_t quat_w(const quat_t* q)
{
    return q->w;
}

/**
 * Получает двухмерный вектор XY.
 * @param q Кватернион.
 * @param v_out Результат.
 */
ALWAYS_INLINE static void quat_xy(const quat_t* q, vec2_t* v_out)
{
    v_out->x = q->x;
    v_out->y = q->y;
}

/**
 * Получает трёхмерный вектор XYZ.
 * @param q Кватернион.
 * @param v_out Результат.
 */
ALWAYS_INLINE static void quat_xyz(const quat_t* q, vec3_t* v_out)
{
    v_out->x = q->x;
    v_out->y = q->y;
    v_out->z = q->z;
}

/**
 * Получает четырёхмерный вектор XYZW.
 * @param q Кватернион.
 * @param v_out Результат.
 */
ALWAYS_INLINE static void quat_xyzw(const quat_t* q, vec4_t* v_out)
{
    v_out->x = q->x;
    v_out->y = q->y;
    v_out->z = q->z;
    v_out->w = q->w;
}

/**
 * Получает компонент кватерниона по индексу.
 * @param q Кватернион.
 * @param i Индекс.
 * @return Компонент кватерниона.
 */
ALWAYS_INLINE static f32_t quat_at(const quat_t* q, size_t i)
{
    return q->q[i];
}

/**
 * Получает флаг равенства кватернионов.
 * @param q1 Первый кватернион.
 * @param q2 Второй кватернион.
 * @return Флаг равенства.
 */
EXTERN bool quat_equal(const quat_t* q1, const quat_t* q2);

/**
 * Получает угол вращения кватерниона.
 * @param q Кватернион.
 * @return Угол вращения.
 */
EXTERN f32_t quat_angle(const quat_t* q);

/**
 * Получает ось вращения кватерниона.
 * @param q Кватернион.
 * @param v Ось вращения.
 * @return Флаг получения оси вращения.
 * Возвращает false при нулевом вращении.
 */
EXTERN bool quat_axis(const quat_t* q, vec3_t* v);

/**
 * Получает кватернион вращения.
 * @param q Результат.
 * @param x Компонента X оси вращения.
 * @param y Компонента Y оси вращения.
 * @param z Компонента Z оси вращения.
 * @param angle Угол вращения.
 */
EXTERN void quat_rotation(quat_t* q, f32_t x, f32_t y, f32_t z, f32_t angle);

/**
 * Получает кватернион вращения.
 * @param q Результат.
 * @param v Ось вращения.
 * @param angle Угол вращения.
 */
ALWAYS_INLINE static void quat_rotationv(quat_t* q, const vec3_t* v, f32_t angle)
{
    quat_rotation(q, v->x, v->y, v->z, angle);
}

/**
 * Получает кватернион вращения одного вектора в другой.
 * @param q Результат.
 * @param v1 Первый вектор.
 * @param v2 Второй вектор.
 */
EXTERN void quat_rotation2(quat_t* q, const vec3_t* v1, const vec3_t* v2);

/**
 * Получает кватернион из углов вращения.
 * @param q Результат.
 * @param pitch Угол по оси X.
 * @param roll Угол по оси Y.
 * @param yaw Угол по оси Z.
 */
EXTERN void quat_from_eulers(quat_t* q, f32_t roll, f32_t pitch, f32_t yaw);

/**
 * Получает углы вращения из кватерниона.
 * @param q Кватернион.
 * @param pitch Угол по оси X.
 * @param roll Угол по оси Y.
 * @param yaw Угол по оси Z.
 */
EXTERN void quat_to_eulers(const quat_t* q, f32_t* roll, f32_t* pitch, f32_t* yaw);

/**
 * Складывает два кватерниона.
 * @param q Результат.
 * @param q1 Первый кватернион.
 * @param q2 Второй кватернион.
 */
EXTERN void quat_add(quat_t* q, const quat_t* q1, const quat_t* q2);

/**
 * Вычитает второй кватернион из первого.
 * @param q Результат.
 * @param q1 Первый кватернион.
 * @param q2 Второй кватернион.
 */
EXTERN void quat_sub(quat_t* q, const quat_t* q1, const quat_t* q2);

/**
 * Умножает один кватернион на другой.
 * Суммирует вращение.
 * @param q Результат.
 * @param q1 Первый кватернион.
 * @param q2 Второй кватернион.
 */
EXTERN void quat_mul(quat_t* q, const quat_t* q1, const quat_t* q2);

/**
 * Умножает кватернион на число.
 * @param q Результат.
 * @param q1 Кватернион.
 * @param n Число.
 */
EXTERN void quat_mul_n(quat_t* q, const quat_t* q1, f32_t n);

/**
 * Делит кватернион на число.
 * @param q Результат.
 * @param q1 Кватернион.
 * @param n Число.
 */
EXTERN void quat_div_n(quat_t* q, const quat_t* q1, f32_t n);

/**
 * Получает норму кватерниона.
 * @param q Кватернион.
 * @return Норма кватерниона.
 */
EXTERN f32_t quat_norm(const quat_t* q);

/**
 * Получает величину кватерниона.
 * @param q Кватернион.
 * @return Величина кватерниона.
 */
EXTERN f32_t quat_magnitude(const quat_t* q);

/**
 * Получает сопряжённый кватернион.
 * @param q Результат.
 * @param q1 Кватернион.
 */
EXTERN void quat_conjugate(quat_t* q, const quat_t* q1);

/**
 * Получает инвертированный кватернион.
 * @param q Результат.
 * @param q1 Кватернион.
 * @return Величина кватерниона (при 0 инверсия не произведена).
 */
EXTERN f32_t quat_invert(quat_t* q, const quat_t* q1);

/**
 * Получает скалярное произведение кватернионов.
 * @param q1 Первый кватернион.
 * @param q2 Второй кватернион.
 * @return Скалярное произведение.
 */
EXTERN f32_t quat_inner(const quat_t* q1, const quat_t* q2);

/**
 * Нормализует кватернион.
 * @param q Результат.
 * @param q1 Кватернион.
 * @return Величина кватерниона (при 0 нормализация не произведена).
 */
EXTERN f32_t quat_normalize(quat_t* q, const quat_t* q1);

/**
 * Поворачивает вектор кватернионом.
 * @param q Кватернион.
 * @param v_in Вектор.
 * @param v_out Результат.
 */
EXTERN void quat_rotate_vec3(const quat_t* q, const vec3_t* v_in, vec3_t* v_out);

/**
 * Поворачивает вектор кватернионом.
 * @param q Кватернион.
 * @param v_in Вектор.
 * @param v_out Результат.
 */
EXTERN void quat_rotate_vec4(const quat_t* q, const vec4_t* v_in, vec4_t* v_out);

/**
 * Выполняет сферическую интерполяцию.
 * @param q Результат.
 * @param q1 Первый кватернион.
 * @param q2 Второй кватернион.
 * @param t Параметр интерполяции.
 */
EXTERN void quat_slerp(quat_t* q, const quat_t* q1, const quat_t* q2, f32_t t);

/**
 * Ограничивает компоненты кватерниона.
 * @param q Результат.
 * @param q1 Кватернион.
 * @param q_min Минимальные компоненты кватерниона.
 * @param q_max Максимальные компоненты кватерниона.
 */
EXTERN void quat_clamp(quat_t* q, const quat_t* q1, const quat_t* q_min, const quat_t* q_max);

#endif /* FIXED_QUAT_H */
