/**
 * @file decim.h Библиотека дециматора.
 */

#ifndef DECIM_H_
#define DECIM_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "q15/q15.h"

//! Тип данных для децимации.
typedef q15_t decim_data_t;

//! Структура дециматора.
typedef struct _Decim {
    size_t scale; //!< Коэффициент децимации.
    size_t index; //!< Счётчик семплов.
    decim_data_t data; //!< Текущие данные.
} decim_t;


/**
 * Инициализирует дециматор.
 * @param decim Дециматор.
 * @param scale Коэффициент децимации.
 * @return Код ошибки.
 */
extern err_t decim_init(decim_t* decim, size_t scale);

/**
 * Сбрасывает дециматор.
 * @param decim Дециматор.
 */
extern void decim_reset(decim_t* decim);

/**
 * Помещает данные в дециматор.
 * @param decim Дециматор.
 * @param data Данные.
 */
extern void decim_put(decim_t* decim, decim_data_t data);

/**
 * Получает флаг готовности данных.
 * @param decim Дециматор.
 * @return Флаг готовности данных.
 */
ALWAYS_INLINE static bool decim_ready(decim_t* decim)
{
    return decim->index == 0;
}

/**
 * Получает последние данные.
 * @param decim Дециматор.
 * @return Последние данные.
 */
ALWAYS_INLINE static decim_data_t decim_data(decim_t* decim)
{
    return decim->data;
}
/**
 * Получает коэффициент децимации.
 * @param decim Дециматор.
 * @return Коэффициент децимации.
 */
ALWAYS_INLINE static size_t decim_scale(decim_t* decim)
{
    return decim->scale;
}

/**
 * Получает смещение с последней выборки исходного сигнала.
 * @param decim Дециматор.
 * @return Смещение.
 */
ALWAYS_INLINE static size_t decim_skew(decim_t* decim)
{
    return decim->index;
}

#endif /* DECIM_H_ */
