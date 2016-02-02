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
 * Ограничительный инкремент/декремент на заданное значение.
 */
#define CLAMP_ADD(VAL, ADD, MAX) do{VAL += (ADD); if(VAL > (MAX)) VAL = (MAX);}while(0)
#define CLAMP_SUB(VAL, SUB, MIN) do{VAL -= (SUB); if(VAL < (MIN)) VAL = (MIN);}while(0)

/**
 * Ограничительный инкремент/декремент на 1.
 */
#define CLAMP_INC(VAL, MAX) CLAMP_ADD(VAL, 1, MAX)
#define CLAMP_DEC(VAL, MIN) CLAMP_SUB(VAL, 1, MIN)

/**
 * Ожидание пока условие C истинно.
 */
#define WAIT_WHILE_TRUE(C) do{}while((C))

/**
 * Ожидание пока условие C ложно.
 */
#define WAIT_WHILE_FALSE(C) do{}while(!(C))

/**
 * Если функция F возвратила ошибку, отличную от E_NO_ERROR - возвращает эту ошибку.
 */
#define RETURN_ERR_IF_FAIL(F) do{\
        err_t CONCAT(err, __LINE__) = F;\
        if(CONCAT(err, __LINE__) != E_NO_ERROR)\
        { return CONCAT(err, __LINE__); }\
        }while(0)

#endif	/* UTILS_H */
