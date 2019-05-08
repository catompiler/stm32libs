/**
 * @file avg.h Библиотека усреднениея.
 */

#ifndef AVG_H_
#define AVG_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "q15/q15.h"

//! Тип данных для усреднения.
typedef q15_t avg_data_t;

//! Тип суммы данных для усреднения.
typedef iq15_t avg_sum_t;

//! Структура усреднения.
typedef struct _Avg {
    size_t count; //!< Количество элементов для усреднения.
    avg_sum_t sum; //!< Сумма данных.
    avg_data_t data; //!< Текущие данные.
} avg_t;


/**
 * Инициализирует усреднение.
 * @param avg Усреднение.
 * @return Код ошибки.
 */
extern err_t avg_init(avg_t* avg);

/**
 * Сбрасывает усреднение.
 * @param avg Усреднение.
 */
extern void avg_reset(avg_t* avg);

/**
 * Помещает данные в усреднение.
 * @param avg Усреднение.
 * @param data Данные.
 */
extern void avg_put(avg_t* avg, avg_data_t data);

/**
 * Вычисляет среднее.
 * @param avg Усреднение.
 * @return Последние данные.
 */
extern avg_data_t avg_calc(avg_t* avg);

/**
 * Получает последние данные.
 * @param avg Усреднение.
 * @return Последние данные.
 */
ALWAYS_INLINE static avg_data_t avg_data(avg_t* avg)
{
    return avg->data;
}

#endif /* AVG_H_ */
