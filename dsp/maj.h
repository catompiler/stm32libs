/**
 * @file maj.h Библиотека мажоритара.
 */

#ifndef MAJ_H_
#define MAJ_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "errors/errors.h"
#include "defs/defs.h"


//! Структура мажоритара.
typedef struct _Maj {
    size_t ones; //!< Число единиц.
    size_t zeros; //!< Число нулей.
    bool value; //!< Значение наибольшего.
} maj_t;


/**
 * Инициализирует мажоритар.
 * @param maj Мажоритар.
 * @return Код ошибки.
 */
extern err_t maj_init(maj_t* maj);

/**
 * Сбрасывает мажоритар.
 * @param maj Мажоритар.
 */
extern void maj_reset(maj_t* maj);

/**
 * Помещает значение в мажоритар.
 * @param maj Мажоритар.
 * @param value Значение.
 */
extern void maj_put(maj_t* maj, bool value);

/**
 * Вычисляет мажоритар.
 * @param maj Мажоритар.
 * @return Последние данные.
 */
extern bool maj_calc(maj_t* maj);

/**
 * Получает значение.
 * @param maj Мажоритар.
 * @return Значение.
 */
ALWAYS_INLINE static bool maj_value(maj_t* maj)
{
    return maj->value;
}

#endif /* MAJ_H_ */
