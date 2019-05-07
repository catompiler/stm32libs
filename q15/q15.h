/**
 * @file q15.h Библиотека работы с числами с фиксированной запятой.
 */

#ifndef Q15_H_
#define Q15_H_

//#include <arm_math.h>
#include <stdint.h>
#include "defs/defs.h"



/**
 * Число с фиксированной запятой.
 */
typedef int16_t q15_t;

/**
 * Число с фиксированной запятой
 * (с целой частью).
 */
typedef int32_t iq15_t;

/**
 * Расширенное число с фиксированной запятой
 * (с целой частью).
 */
typedef int64_t lq15_t;


//! Число дробных бит.
#define Q15_FRACT_BITS 15

//! Бит сатурации.
#define Q15_SAT_BIT ((Q15_FRACT_BITS) + 1)

//! Делитель дробной части (значение 1.0).
#define Q15_BASE (1 << (Q15_FRACT_BITS))

//! Маска дробной части.
#define Q15_FRACT_MASK ((1 << (Q15_FRACT_BITS)) - 1)

//! Маска знака.
#define Q15_SIGN_MASK (1 << (Q15_FRACT_BITS))


//! Вычисляет число с фиксированной запятой по float.
#define Q15(F) ((q15_t)(F * Q15_BASE))

//! Вычисляет число с фиксированной запятой с целой частью по float.
#define IQ15(F) ((iq15_t)(F * Q15_BASE))

//! Вычисляет число с фиксированной запятой с целой частью по int.
#define IQ15I(I) (((iq15_t)(I)) << Q15_FRACT_BITS)

//! Вычисляет число с фиксированной запятой с по дроби A/B.
#define IQ15F(A, B) ((((iq15_t)(A)) << Q15_FRACT_BITS) / (B))

//! Вычисляет расширенное число с фиксированной запятой с целой частью по float.
#define LQ15(F) ((lq15_t)(F * Q15_BASE))

//! Вычисляет расширенное число с фиксированной запятой с целой частью по int.
#define LQ15I(I) (((lq15_t)(I)) << Q15_FRACT_BITS)

//! Вычисляет расширенное число с фиксированной запятой с по дроби A/B.
#define LQ15F(A, B) ((((lq15_t)(A)) << Q15_FRACT_BITS) / (B))


//! Максимальное число Q15.
#define Q15_MAX ((q15_t)0x7fff)

//! Минимальное число Q15.
#define Q15_MIN ((q15_t)0x8000)


//! Преобразовывает одно число с фиксированной запятой
//! из формата Qn в формат Qm.
#define IQNtoM(Q, N, M) ((M > N) ? ((Q) << (M - N)) : ((Q) >> (N - M)))
#define QNtoM(Q, N, M) IQNtoM(Q, N, M)

//! Получает модуль числа с фиксированной запятой.
#define IQ15_ABS(Q) (((Q) >= 0) ? ((Q)) : (-(Q)))
#define Q15_ABS(Q) IQ15_ABS(Q)

//! Получает целую часть числа с фиксированной запятой.
#define IQ15_INT(Q) ((Q) >> Q15_FRACT_BITS)

//! Получает дробную часть числа с фиксированной запятой.
#define IQ15_FRACT(Q) (((Q) > 0) ? ((Q) & Q15_FRACT_MASK) : (-((-(Q)) & Q15_FRACT_MASK)))

//! Выполняет насыщение числа с фиксированной запятой
//! Q до разряда N.
#ifdef __ARM__
#define IQ15_SSAT(Q, N)\
    ({ register int32_t __RES, __Q = Q;\
    __asm__("ssat %0, %1, %2" : "=r"(__RES) : "I"(N), "r"(__Q));\
    __RES;})
#else
#define IQ15_SSAT(Q, N)\
    ({ register int32_t __RES, __Q = Q;\
       const int32_t __MAX = ( (1<<((N)-1))-1);\
       const int32_t __MIN = (-(1<<((N)-1))  );\
    __RES = (__Q > __MAX) ? __MAX : ((__Q < __MIN) ? __MIN : __Q);\
    __RES;})
#endif
#define Q15_SSAT(Q, N) IQ15_SSAT(Q, N)

//! Выполняет насыщение числа с фиксированной запятой
//! Q до разряда N со сдвигом вправо на S разрядов.
#ifdef __ARM__
#define IQ15_SSAT_ASR(Q, N, S)\
    ({ register int32_t __RES, __Q = Q;\
    __asm__("ssat %0, %1, %2 ASR %3" : "=r"(__RES) : "I"(N), "r"(__Q), "I"(S));\
    __RES;})
#else
#define IQ15_SSAT_ASR(Q, N, S)\
    ({ register int32_t __RES, __Q = (Q) >> (S);\
    __RES = IQ15_SSAT(__Q, N);\
    __RES;})
#endif
#define Q15_SSAT_ASR(Q, N, S) IQ15_SSAT_ASR(Q, N, S)

//! Выполняет умножение чисел с фиксированной запятой
//! Q1 и Q2 и сложение их с аккумулятором A.
#ifdef __ARM__
#define IQ15_MLA(Q1, Q2, A)\
    ({ register int32_t __RES, __Q1 = Q1, __Q2 = Q2, __A = A;\
    __asm__("mla %0, %1, %2, %3" : "=r"(__RES) : "r"(__Q1), "r"(__Q2), "r"(__A));\
    __RES;})
#else
#define IQ15_MLA(Q1, Q2, A)\
    ({ register int32_t __RES, __Q1 = Q1, __Q2 = Q2, __A = A;\
    __RES = __Q1 * __Q2 + __A;\
    __RES;})
#endif
#define Q15_MLA(Q1, Q2, A) IQ15_MLA(Q1, Q2, A)

//! Выполняет умножение чисел с фиксированной запятой
//! Q1 и Q2 и сложение их с 64 битным аккумулятором A.
#ifdef __ARM__
#define IQ15_MLAL(Q1, Q2, A)\
    ({ union __U_MLAL_64 { int64_t _64;\
           struct _S_MLAL_64 {int32_t lo; int32_t hi;} _32; };\
       register union __U_MLAL_64 __u_mlal_64 = {A};\
       register int32_t __Q1 = Q1, __Q2 = Q2;\
    __asm__("smlal %0, %1, %2, %3" : "=r"(__u_mlal_64._32.lo), "=r"(__u_mlal_64._32.hi) :\
            "r"(__Q1), "r"(__Q2), "0"(__u_mlal_64._32.lo), "1"(__u_mlal_64._32.hi));\
    (__u_mlal_64._64);})
#else
#define IQ15_MLAL(Q1, Q2, A)\
    ({ register int32_t __Q1 = Q1, __Q2 = Q2;\
       register int64_t __RES, __A = A;\
    __RES = (int64_t)__Q1 * __Q2 + __A;\
    __RES;})
#endif
#define Q15_MLAL(Q1, Q2, A) IQ15_MLAL(Q1, Q2, A)


/**
 * Получает модуль числа Q15.
 * @param q Число.
 * @return Модуль числа.
 */
ALWAYS_INLINE static q15_t q15_abs(q15_t q)
{
    return Q15_ABS(q);
}

/**
 * Получает модуль числа IQ15.
 * @param q Число.
 * @return Модуль числа.
 */
ALWAYS_INLINE static iq15_t iq15_abs(iq15_t q)
{
    return IQ15_ABS(q);
}

/**
 * Получает целую часть числа IQ15.
 * @param q Число.
 * @return Целая часть.
 */
ALWAYS_INLINE static int16_t iq15_int(iq15_t q)
{
    return (int16_t)IQ15_INT(q);
}

/**
 * Получает дробную часть числа IQ15.
 * @param q Число.
 * @return Дробная часть.
 */
ALWAYS_INLINE static int16_t iq15_fract(iq15_t q)
{
    return (int16_t)IQ15_FRACT(q);
}

/**
 * Насыщает число Q15 до Q15.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static q15_t q15_sat(int32_t q)
{
    register int32_t res;

    res = Q15_SSAT(q, Q15_SAT_BIT);

    return (q15_t)res;
}

/**
 * Насыщает число IQ15 до Q15.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static q15_t iq15_sat(iq15_t q)
{
    register int32_t res;

    res = IQ15_SSAT(q, Q15_SAT_BIT);

    return (q15_t)res;
}

/*
 * Сложение.
 */

/**
 * Складывает два числа Q15.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static iq15_t q15_add(q15_t a, q15_t b)
{
    register int32_t res;

    res = (int32_t)a + b;

    return res;
}

/**
 * Складывает два числа IQ15.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static iq15_t iq15_add(iq15_t a, iq15_t b)
{
    register int32_t res;

    res = a + b;

    return res;
}

/**
 * Складывает два числа Q15
 * с насыщением.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static q15_t q15_add_sat(q15_t a, q15_t b)
{
    register int32_t res;

    res = (int32_t)a + b;
    res = Q15_SSAT(res, Q15_SAT_BIT);

    return (q15_t)res;
}

/*
 * Вычитание.
 */

/**
 * Вычитает два числа Q15.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static iq15_t q15_sub(q15_t a, q15_t b)
{
    register int32_t res;

    res = (int32_t)a - b;

    return res;
}

/**
 * Вычитает два числа IQ15.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static iq15_t iq15_sub(iq15_t a, iq15_t b)
{
    register int32_t res;

    res = a - b;

    return res;
}

/**
 * Вычитает два числа Q15
 * с насыщением.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static q15_t q15_sub_sat(q15_t a, q15_t b)
{
    register int32_t res;

    res = (int32_t)a - b;
    res = Q15_SSAT(res, Q15_SAT_BIT);

    return (q15_t)res;
}

/*
 * Умножение.
 */

/**
 * Умножает два числа Q15.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static q15_t q15_mul(q15_t a, q15_t b)
{
    register int32_t res;

    res = (int32_t)a * b;
    res = res >> Q15_FRACT_BITS;

    return (q15_t)res;
}

/**
 * Умножает два числа IQ15.
 * Не контролирует переполнение.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static iq15_t iq15_mul(iq15_t a, iq15_t b)
{
    register int32_t res;

    res = a * b;
    res = res >> Q15_FRACT_BITS;

    return res;
}

/**
 * Умножает два числа IQ15.
 * Использует 64 бит результат.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static lq15_t iq15_mull(iq15_t a, iq15_t b)
{
    register int64_t res;

    res = (int64_t)a * b;
    res = res >> Q15_FRACT_BITS;

    return res;
}

/**
 * Умножает число Q15 на целое число.
 * @param a Число.
 * @param b Целое число.
 * @return Результат.
 */
ALWAYS_INLINE static iq15_t q15_imul(q15_t a, int32_t b)
{
    register int32_t res;

    res = (int32_t)a * b;

    return res;
}

/**
 * Умножает число IQ15 на целое число.
 * @param a Число.
 * @param b Целое число.
 * @return Результат.
 */
ALWAYS_INLINE static iq15_t iq15_imul(iq15_t a, int32_t b)
{
    register int32_t res;

    res = a * b;

    return res;
}

/**
 * Умножает число IQ15 на целое число.
 * Использует 64 бит результат.
 * @param a Число.
 * @param b Целое число.
 * @return Результат.
 */
ALWAYS_INLINE static lq15_t iq15_imull(iq15_t a, int32_t b)
{
    register int64_t res;

    res = (int64_t)a * b;

    return res;
}

/*
 * Деление.
 */

/**
 * Делит два числа Q15.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static iq15_t q15_div(q15_t a, q15_t b)
{
    register int32_t res;

    res = (int32_t)a << Q15_FRACT_BITS;
    res = res / b;

    return res;
}

/**
 * Делит два числа IQ15.
 * Не контролирует переполнение.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static iq15_t iq15_div(iq15_t a, iq15_t b)
{
    register int32_t res;

    res = a << Q15_FRACT_BITS;
    res = res / b;

    return res;
}

/**
 * Делит два числа IQ15.
 * Использует 64 бит результат.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static lq15_t iq15_divl(iq15_t a, iq15_t b)
{
    register int64_t res;

    res = (int64_t)a << Q15_FRACT_BITS;
    res = res / b;

    return res;
}

/**
 * Делит два числа Q15
 * с насыщением.
 * @param a Число.
 * @param b Число.
 * @return Результат.
 */
ALWAYS_INLINE static q15_t q15_div_sat(q15_t a, q15_t b)
{
    register int32_t res;

    res = (int32_t)a << Q15_FRACT_BITS;
    res = res / b;
    res = Q15_SSAT(res, Q15_SAT_BIT);

    return (q15_t)res;
}

/**
 * Делит число Q15 на целое число.
 * @param a Число.
 * @param b Целое число.
 * @return Результат.
 */
ALWAYS_INLINE static q15_t q15_idiv(q15_t a, int32_t b)
{
    register int32_t res;

    res = (int32_t)a / b;

    return (q15_t)res;
}

/**
 * Делит число IQ15 на целое число.
 * @param a Число.
 * @param b Целое число.
 * @return Результат.
 */
ALWAYS_INLINE static iq15_t iq15_idiv(iq15_t a, int32_t b)
{
    register int32_t res;

    res = a / b;

    return res;
}

/*
 * Умножение с накоплением.
 */

/**
 * Перемножает с накоплением два числа Q15.
 * @param a Число.
 * @param b Число.
 * @param acc Аккумулятор.
 * @return Результат.
 */
ALWAYS_INLINE static int32_t q15_mla(q15_t a, q15_t b, int32_t acc)
{
    register int32_t res;

    res = Q15_MLA(a, b, acc);

    return res;
}

/**
 * Перемножает с накоплением два числа Q15.
 * @param a Число.
 * @param b Число.
 * @param acc Аккумулятор.
 * @return Результат.
 */
ALWAYS_INLINE static int64_t iq15_mlal(iq15_t a, iq15_t b, int64_t acc)
{
    register int64_t res;

    res = IQ15_MLAL(a, b, acc);

    return res;
}


#endif /* Q15_H_ */
