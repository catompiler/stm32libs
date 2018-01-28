/**
 * @file fixed_math.h Библиотека математики для чисел с фиксированной запятой.
 */

#ifndef FIXED_MATH_H
#define FIXED_MATH_H

#include <stdint.h>
#include "defs/defs.h"


/**
 * Тип числа с фиксированной запятой.
 * 32 бит.
 * ----------------------------------------------------------
 * |  1 bit sign  |  15 bit int part  |  16 bit fract part  |
 * ----------------------------------------------------------
 */
typedef int32_t f32_t;
typedef int32_t i32_t;
typedef uint32_t u32_t;
// Совместимость с fixed32.
#ifdef F32_FIXED32_COMPATIBLE
typedef f32_t fixed32_t;
#endif// F32_FIXED32_COMPATIBLE
/**
 * Тип числа с фиксированной запятой.
 * 64 бит.
 * ----------------------------------------------------------
 * |  1 bit sign  |  47 bit int part  |  16 bit fract part  |
 * ----------------------------------------------------------
 */
typedef int64_t f64_t;
typedef int64_t i64_t;
typedef uint64_t u64_t;
// Совместимость с fixed64.
#ifdef F64_FIXED64_COMPATIBLE
typedef f64_t fixed64_t;
#endif// F64_FIXED64_COMPATIBLE


/*
 * Макросы для инициализации.
 */
// Инициализация.
#define F32(f) ((int32_t)(f * 0x10000))
// Инициализация целым числом.
#define F32I(i) ((i) << 16)
// Инициализация дробью a / b.
#define F32F(a, b) (((int64_t)(a) << 16) / (b))
// Инициализация.
#define F64(f) ((int64_t)(f * 0x10000))
// Инициализация целым числом.
#define F64I(i) ((i) << 16)
// Инициализация дробью a / b.
#define F64F(a, b) (((int64_t)(a) << 16) / (b))


/*
 * Значения углов.
 */
// 0 градусов.
#define F32_ANGLE_0   0
// 90 градусов.
#define F32_ANGLE_90  F32I(90)
// 180 градусов.
#define F32_ANGLE_180 F32I(180)
// 270 градусов.
#define F32_ANGLE_270 F32I(270)
// 360 градусов.
#define F32_ANGLE_360 F32I(360)


/*
 * Базовые операции.
 */

/*
 * Инициализация.
 */

ALWAYS_INLINE static f32_t f32_from_int(i32_t i)
    { return F32I(i); }

ALWAYS_INLINE static f32_t f32_from_fract(i32_t a, i32_t b)
    { return F32F(a, b); }

/*
 * Доступ к частям.
 */

ALWAYS_INLINE static i32_t f32_int_part(f32_t f)
    { return (f >= 0) ? (f >> 16) : (-((-f) >> 16)); }

ALWAYS_INLINE static i32_t f32_fract_part(f32_t f)
    { return (f >= 0) ? (f & 0xffff) : (-((-f) & 0xffff)); }

ALWAYS_INLINE static i32_t f32_fract_num(f32_t f, i32_t d)
    { return ((i64_t)f32_fract_part(f) * d) >> 16; }

/*
 * Округление.
 */
ALWAYS_INLINE static f32_t f32_floor(f32_t f)
    { return f & 0xffff0000; }

ALWAYS_INLINE static f32_t f32_ceil(f32_t f)
    { return (f + 0xffff) & 0xffff0000; }

ALWAYS_INLINE static f32_t f32_round(f32_t f)
    { return (f + 0x8000) & 0xffff0000; }

/*
 * Сложение.
 */
ALWAYS_INLINE static f32_t add32_f32_f32(f32_t a, f32_t b)
    { return a + b; }

ALWAYS_INLINE static f64_t add64_f32_f32(f32_t a, f32_t b)
    { return (f64_t)a + b; }

ALWAYS_INLINE static f64_t add64_f64_f32(f64_t a, f32_t b)
    { return a + b; }

ALWAYS_INLINE static f64_t add64_f64_f64(f64_t a, f64_t b)
    { return a + b; }

ALWAYS_INLINE static f32_t add32_f32_i32(f32_t a, i32_t b)
    { return a + F32I(b); }

ALWAYS_INLINE static f64_t add64_f32_i32(f32_t a, i32_t b)
    { return (f64_t)a + F32I(b); }

ALWAYS_INLINE static f64_t add64_f64_i32(f64_t a, i32_t b)
    { return a + F32I(b); }

ALWAYS_INLINE static f64_t add64_f64_i64(f64_t a, i64_t b)
    { return a + F64I(b); }

/*
 * Вычитание.
 */
ALWAYS_INLINE static f32_t sub32_f32_f32(f32_t a, f32_t b)
    { return a - b; }

ALWAYS_INLINE static f64_t sub64_f32_f32(f32_t a, f32_t b)
    { return (f64_t)a - b; }

ALWAYS_INLINE static f64_t sub64_f64_f32(f64_t a, f32_t b)
    { return a - b; }

ALWAYS_INLINE static f64_t sub64_f64_f64(f64_t a, f64_t b)
    { return a - b; }

ALWAYS_INLINE static f32_t sub32_f32_i32(f32_t a, i32_t b)
    { return a - F32I(b); }

ALWAYS_INLINE static f64_t sub64_f32_i32(f32_t a, i32_t b)
    { return (f64_t)a + F32I(b); }

ALWAYS_INLINE static f64_t sub64_f64_i32(f64_t a, i32_t b)
    { return a + F32I(b); }

ALWAYS_INLINE static f64_t sub64_f64_i64(f64_t a, i64_t b)
    { return a + F64I(b); }

/*
 * Умножение.
 */
ALWAYS_INLINE static f32_t mul32_f32_f32(f32_t a, f32_t b)
    { return ((int64_t)a * b) >> 16; }

ALWAYS_INLINE static f64_t mul64_f32_f32(f32_t a, f32_t b)
    { return ((int64_t)a * b) >> 16; }

ALWAYS_INLINE static f64_t mul64_f64_f32(f64_t a, f32_t b)
    { return (a * b) >> 16; }

ALWAYS_INLINE static f64_t mul64_f64_f64(f64_t a, f64_t b)
    { return (a * b) >> 16; }

ALWAYS_INLINE static f32_t mul32_f32_i32(f32_t a, i32_t b)
    { return a * b; }

ALWAYS_INLINE static f64_t mul64_f32_i32(f32_t a, i32_t b)
    { return (int64_t)a * b; }

ALWAYS_INLINE static f64_t mul64_f64_i32(f64_t a, i32_t b)
    { return a * b; }

ALWAYS_INLINE static f64_t mul64_f64_i64(f64_t a, i64_t b)
    { return a * b; }

/*
 * Деление.
 */
ALWAYS_INLINE static f32_t div32_f32_f32(f32_t a, f32_t b)
    { return ((int64_t)a << 16) / b; }

ALWAYS_INLINE static f64_t div64_f32_f32(f32_t a, f32_t b)
    { return ((int64_t)a << 16) / b; }

ALWAYS_INLINE static f64_t div64_f64_f32(f64_t a, f32_t b)
    { return (a << 16) / b; }

ALWAYS_INLINE static f64_t div64_f64_f64(f64_t a, f64_t b)
    { return (a << 16) / b; }

ALWAYS_INLINE static f32_t div32_f32_i32(f32_t a, i32_t b)
    { return a / b; }

ALWAYS_INLINE static f64_t div64_f32_i32(f32_t a, i32_t b)
    { return (int64_t)a / b; }

ALWAYS_INLINE static f64_t div64_f64_i32(f64_t a, i32_t b)
    { return a / b; }

ALWAYS_INLINE static f64_t div64_f64_i64(f64_t a, i64_t b)
    { return a / b; }

/*
 * Модуль.
 */
ALWAYS_INLINE static f32_t abs32_f32(f32_t f)
    { return (f >= 0) ? f : (-f); }

ALWAYS_INLINE static f64_t abs64_f64(f64_t f)
    { return (f >= 0) ? f : (-f); }

/*
 * Насыщение.
 */
ALWAYS_INLINE static f32_t sat32_f64(f64_t f)
    { return (f > INT32_MAX) ? (INT32_MAX) : ((f < INT32_MIN) ? (INT32_MIN) : f); }


/*
 * Операции с числом.
 */

/*
 * Арифметика.
 */


/**
 * Складывает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static f32_t add_f32(f32_t a, f32_t b)
{
    return sat32_f64(add64_f32_f32(a, b));
}

/**
 * Складывает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static f32_t addi_f32(f32_t a, int32_t b)
{
    return sat32_f64(add64_f32_i32(a, b));
}

/**
 * Вычитает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static f32_t sub_f32(f32_t a, f32_t b)
{
    return sat32_f64(sub64_f32_f32(a, b));
}

/**
 * Вычитает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static f32_t subi_f32(f32_t a, int32_t b)
{
    return sat32_f64(sub64_f32_i32(a, b));
}

/**
 * Умножает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static f32_t mul_f32(f32_t a, f32_t b)
{
    return sat32_f64(mul64_f32_f32(a, b));
}

/**
 * Умножает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static f32_t muli_f32(f32_t a, int32_t b)
{
    return sat32_f64(mul64_f32_i32(a, b));
}

/**
 * Делит два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static f32_t div_f32(f32_t a, f32_t b)
{
    return sat32_f64(div64_f32_f32(a, b));
}

/**
 * Делит два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static f32_t divi_f32(f32_t a, int32_t b)
{
    return div32_f32_i32(a, b);
}

/**
 * Ограничивает 64 битное число 32 битным.
 * @param n Число.
 * @return Ограниченное число.
 */
ALWAYS_INLINE static f32_t sat_f32(f64_t f)
{
    return sat32_f64(f);
}

// Совместимость с fixed32.
#ifdef F32_FIXED32_COMPATIBLE
/**
 * Создаёт число с фиксированной запятой из целого числа.
 * @param n Целое число.
 * @return Результат.
 */
ALWAYS_INLINE static fixed32_t fixed32_from_int(int32_t i)
{
    return f32_from_int(i);
}

/**
 * Создаёт число с фиксированной запятой из дроби.
 * @param a Делитель.
 * @param b Делимое.
 * @return Результат.
 */
ALWAYS_INLINE static fixed32_t fixed32_from_fract(int32_t a, int32_t b)
{
    return f32_from_fract(a, b);
}

/**
 * Получает целую часть.
 * @param f Число.
 * @return Целая часть.
 */
ALWAYS_INLINE static int32_t fixed32_int(fixed32_t f)
{
    return f32_int_part(f);
}

/**
 * Получает дробную часть.
 * @param f Число.
 * @return Дробная часть.
 */
ALWAYS_INLINE static int32_t fixed32_fract(fixed32_t f)
{
    return f32_fract_part(f);
}

/**
 * Получает число в дробной части для заданного делителя.
 * @param f Число.
 * @param d Делитель
 * @return Число дробной части.
 */
ALWAYS_INLINE static int32_t fixed32_fract_num(fixed32_t f, int32_t d)
{
    return f32_fract_num(f, d);
}

/*
 * Арифметика.
 */


/**
 * Складывает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static fixed32_t fixed32_add(fixed32_t a, fixed32_t b)
{
    return sat32_f64(add64_f32_f32(a, b));
}

/**
 * Складывает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static fixed32_t fixed32_add_int(fixed32_t a, int32_t b)
{
    return sat32_f64(add64_f32_i32(a, b));
}

/**
 * Вычитает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static fixed32_t fixed32_sub(fixed32_t a, fixed32_t b)
{
    return sat32_f64(sub64_f32_f32(a, b));
}

/**
 * Вычитает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static fixed32_t fixed32_sub_int(fixed32_t a, int32_t b)
{
    return sat32_f64(sub64_f32_i32(a, b));
}

/**
 * Умножает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static fixed64_t fixed32_mul(fixed32_t a, fixed32_t b)
{
    return sat32_f64(mul64_f32_f32(a, b));
}

/**
 * Умножает два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static fixed64_t fixed32_mul_int(fixed32_t a, int32_t b)
{
    return sat32_f64(mul64_f32_i32(a, b));
}

/**
 * Делит два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static fixed64_t fixed32_div(fixed32_t a, fixed32_t b)
{
    return sat32_f64(div64_f32_f32(a, b));
}

/**
 * Делит два числа.
 * @param a Число А.
 * @param b Число Б.
 * @return Результат.
 */
ALWAYS_INLINE static fixed32_t fixed32_div_int(fixed32_t a, int32_t b)
{
    return div32_f32_i32(a, b);
}

/**
 * Получает модуль числа.
 * @param f Число.
 * @return Модуль числа.
 */
ALWAYS_INLINE static fixed32_t fixed32_abs(fixed32_t f)
{
    return abs32_f32(f);
}

/**
 * Ограничивает 64 битное число 32 битным.
 * @param n Число.
 * @return Ограниченное число.
 */
ALWAYS_INLINE static fixed32_t fixed32_saturate(fixed64_t f)
{
    return sat32_f64(f);
}
#endif

/*
 * Математика.
 */

/**
 * Получает модуль числа.
 * @param f Число.
 * @return Модуль числа.
 */
ALWAYS_INLINE static f32_t abs_f32(f32_t f)
{
    return abs32_f32(f);
}

/**
 * Получает ближайшее меньшее целое число.
 * @param f Число.
 * @return Округлённое число.
 */
ALWAYS_INLINE static f32_t floor_f32(f32_t f)
{
    return f32_floor(f);
}

/**
 * Получает ближайшее большее целое число.
 * @param f Число.
 * @return Округлённое число.
 */
ALWAYS_INLINE static f32_t ceil_f32(f32_t f)
{
    return f32_ceil(f);
}

/**
 * Получает ближайшее целое число.
 * @param f Число.
 * @return Округлённое число.
 */
ALWAYS_INLINE static f32_t round_f32(f32_t f)
{
    return f32_round(f);
}

/**
 * Получает целую часть числа.
 * @param f Число.
 * @return Целая часть.
 */
ALWAYS_INLINE static i32_t int_f32(f32_t f)
{
    return f32_int_part(f);
}

/**
 * Получает дробную часть числа.
 * @param f Число.
 * @return Дробная часть.
 */
ALWAYS_INLINE static f32_t fract_f32(f32_t f)
{
    return f32_fract_part(f);
}

/**
 * Получает число в дробной части для заданного делителя.
 * @param f Число.
 * @param d Делитель
 * @return Число дробной части.
 */
ALWAYS_INLINE static i32_t fract_num_f32(f32_t f, i32_t d)
{
    return f32_fract_num(f, d);
}

/**
 * Выполняет линейную интерполяцию чисел.
 * @param a Число А.
 * @param b Число Б.
 * @param t Вес числа Б.
 * @return Результат.
 */
EXTERN f32_t lerp_f32(f32_t a, f32_t b, f32_t t);

/**
 * Вычисляет квадратный корень числа.
 * Возвращает -1 при недопустимом аргументе.
 * @param n Число.
 * @return Квадратный корень.
 */
EXTERN i32_t sqrt_i32(i32_t n);

/**
 * Вычисляет квадратный корень числа.
 * Возвращает -1 при недопустимом аргументе.
 * @param n Число.
 * @return Квадратный корень.
 */
EXTERN f32_t sqrt_f32(f32_t n);

/**
 * Вычисляет синус заданного угла.
 * @param a Угол в градусах.
 * @return Синус.
 */
EXTERN f32_t sin_i32(i32_t angle);

/**
 * Вычисляет косинус заданного угла.
 * @param a Угол в градусах.
 * @return Косинус.
 */
EXTERN f32_t cos_i32(i32_t angle);

/**
 * Вычисляет синус заданного угла.
 * @param a Угол в градусах.
 * @return Синус.
 */
EXTERN f32_t sin_f32(f32_t angle);

/**
 * Вычисляет косинус заданного угла.
 * @param a Угол в градусах.
 * @return Косинус.
 */
EXTERN f32_t cos_f32(f32_t angle);

/**
 * Вычисляет арксинус.
 * @param value Значение синуса угла.
 * @return Угол.
 */
EXTERN f32_t asin_f32(f32_t value);

/**
 * Вычисляет арккосинус.
 * @param value Значение косинуса угла.
 * @return Угол.
 */
EXTERN f32_t acos_f32(f32_t value);

/**
 * Вычисляет арктангенс по вектору.
 * @param x Координата X.
 * @param y Координата Y.
 * @return Арктангенс.
 */
EXTERN f32_t atan2_f32(f32_t x, f32_t y);

#endif /* FIXED_MATH_H */
