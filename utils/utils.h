/**
 * @file utils.h
 * Различные функции.
 */

#ifndef UTILS_H
#define	UTILS_H

#include "defs/defs.h"

/**
 * Каст void*.
 */
#define int_to_pvoid(I) ((void*)(ptrdiff_t)I)
#define pvoid_to_int(I_TYPE, P) ((I_TYPE)(ptrdiff_t)P)
#define pvoid_cast(T, PV) ((T)PV)

/**
 * Максимальное значение из двух.
 */
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/**
 * Минимальное значнеие из двух.
 */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/**
 * Абсолютное значение числа.
 */
#ifndef ABS
#define ABS(a) ((a) < 0 ? -(a) : (a))
#endif

/**
 * Значение в границах.
 */
#ifndef CLAMP
#define CLAMP(v, min_v, max_v) (MIN(MAX(v, min_v), max_v))
#endif

/**
 * Меняет значения в переменных A и B, используя переменную TMP
 */
#define SWAP(A, B, TMP) do{ (TMP) = (A); (A) = (B); (B) = (TMP); }while(0)

/**
 * Цикличный инкремент/декремент на заданное значение.
 */
#define CYCLIC_ADD(VAL, ADD, MIN, MAX) do{VAL += (ADD); if(VAL >= (MAX)) VAL = (MIN);}while(0)
#define CYCLIC_SUB(VAL, SUB, MIN, MAX) do{VAL -= (SUB); if(VAL <= (MIN)) VAL = (MAX);}while(0)

/**
 * Цикличный инкремент/декремент на 1.
 */
#define CYCLIC_INC(VAL, MIN, MAX) CYCLIC_ADD(VAL, 1, MIN, MAX)
#define CYCLIC_DEC(VAL, MIN, MAX) CYCLIC_SUB(VAL, 1, MIN, MAX)

/**
 * Ожидание пока условие C истинно.
 */
#define WAIT_WHILE_TRUE(C) do{}while((C))

/**
 * Ожидание пока условие C ложно.
 */
#define WAIT_WHILE_FALSE(C) do{}while(!(C))

#endif	/* UTILS_H */
