/**
 * @file filter_ab.h Библиотека цифрового фильтра первого порядка (альфа-бета фильтр).
 */

#ifndef FILTER_AB_H
#define FILTER_AB_H

#include <stdint.h>
#include <stdbool.h>
#include "defs/defs.h"

//! Тип значения фильтра.
typedef int32_t filter_ab_value_t;
//! Тип веса фильтра.
typedef uint16_t filter_ab_weight_t;
//! Число значащих бит веса фильтра.
#define FILTER_AB_WEIGHT_BITS 16

//! Размер медианного фильтра.
#define FILTER_AB_SIZE 3
typedef struct _Filter_AB {
    filter_ab_value_t value; //!< Значение фильтра.
    /**
     * Вес фильтра
     * в формате Q16
     * (беззнаковое число с фиксированной запятой
     * от 0 до 0xffff - [0, 0.99999]).
     */
    filter_ab_weight_t weight;
} filter_ab_t;

/**
 * Инициализирует фильтр.
 * @param filter Фильтр.
 */
EXTERN void filter_ab_init(filter_ab_t* filter);

/**
 * Сбрасывает фильтр.
 * @param filter Фильтр.
 */
EXTERN void filter_ab_reset(filter_ab_t* filter);

/**
 * Устанавливает вес фильтра.
 * @param filter Фильтра.
 * @param weight Вес фильтра.
 */
EXTERN void filter_ab_set_weight(filter_ab_t* filter, filter_ab_weight_t weight);

/**
 * Помещает значение в фильтр.
 * @param filter Фильтр.
 * @param value Значение.
 */
EXTERN void filter_ab_put(filter_ab_t* filter, filter_ab_value_t value);

/**
 * Вычисляет значение фильтра.
 * @param filter Фильтр.
 * @return Значение фильтра.
 */
EXTERN filter_ab_value_t filter_ab_value(filter_ab_t* filter);

#endif /* FILTER_AB_H */
