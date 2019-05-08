/**
 * @file decim_avg_avg.h Библиотека дециматора с усреднением.
 */

#ifndef DECIM_AVG_H_
#define DECIM_AVG_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "q15.h"

//! Тип данных для децимации.
typedef q15_t decim_avg_data_t;

//! Тип суммы данных для децимации.
typedef iq15_t decim_avg_sum_t;

//! Структура дециматора.
typedef struct _Decim_Avg {
    size_t scale; //!< Коэффициент децимации.
    size_t index; //!< Счётчик семплов.
    decim_avg_sum_t sum; //!< Сумма данных.
    decim_avg_data_t data; //!< Текущие данные.
} decim_avg_t;


/**
 * Инициализирует дециматор.
 * @param decim Дециматор.
 * @param scale Коэффициент децимации.
 * @return Код ошибки.
 */
extern err_t decim_avg_init(decim_avg_t* decim, size_t scale);

/**
 * Сбрасывает дециматор.
 * @param decim Дециматор.
 */
extern void decim_avg_reset(decim_avg_t* decim);

/**
 * Помещает данные в дециматор.
 * @param decim Дециматор.
 * @param data Данные.
 */
extern void decim_avg_put(decim_avg_t* decim, decim_avg_data_t data);

/**
 * Получает флаг готовности данных.
 * @param decim Дециматор.
 * @return Флаг готовности данных.
 */
ALWAYS_INLINE static bool decim_avg_ready(decim_avg_t* decim)
{
    return decim->index == 0;
}

/**
 * Получает последние данные.
 * @param decim Дециматор.
 * @return Последние данные.
 */
ALWAYS_INLINE static decim_avg_data_t decim_avg_data(decim_avg_t* decim)
{
    return decim->data;
}
/**
 * Получает коэффициент децимации.
 * @param decim Дециматор.
 * @return Коэффициент децимации.
 */
ALWAYS_INLINE static size_t decim_avg_scale(decim_avg_t* decim)
{
    return decim->scale;
}

/**
 * Получает смещение с последней выборки исходного сигнала.
 * @param decim Дециматор.
 * @return Смещение.
 */
ALWAYS_INLINE static size_t decim_avg_skew(decim_avg_t* decim)
{
    return decim->index;
}

#endif /* DECIM_H_ */
