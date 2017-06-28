/**
 * @file mid_filter3i.h Библиотека медианного фильтра по трём знаковым целочисленным элементам.
 */

#ifndef MID_FILTER3I_H
#define MID_FILTER3I_H

#include <stdint.h>
#include <stdbool.h>
#include "defs/defs.h"

//! Тип значения фильтра.
typedef int32_t mid_filter3i_value_t;

//! Размер медианного фильтра.
#define MID_FILTER3I_SIZE 3
typedef struct _Mid_Filter3i {
    mid_filter3i_value_t buffer[MID_FILTER3I_SIZE]; //!< Буфер на три элемента.
    int8_t index; //!< Индекс текущего элемента.
    int8_t count; //!< Количество элементов.
} mid_filter3i_t;

/**
 * Инициализирует фильтр.
 * @param filter Фильтр.
 */
EXTERN void mid_filter3i_init(mid_filter3i_t* filter);

/**
 * Сбрасывает фильтр.
 * @param filter Фильтр.
 */
EXTERN void mid_filter3i_reset(mid_filter3i_t* filter);

/**
 * Получает флаг заполненности фильтра.
 * @param filter Фильтр.
 * @return Флаг заполненности фильтра.
 */
EXTERN bool mid_filter3i_full(mid_filter3i_t* filter);

/**
 * Получает флаг пустоты фильтра.
 * @param filter Фильтр.
 * @return Флаг пустоты фильтра.
 */
EXTERN bool mid_filter3i_empty(mid_filter3i_t* filter);

/**
 * Помещает значение в фильтр.
 * @param filter Фильтр.
 * @param value Значение.
 */
EXTERN void mid_filter3i_put(mid_filter3i_t* filter, mid_filter3i_value_t value);

/**
 * Вычисляет значение фильтра.
 * @param filter Фильтр.
 * @return Значение фильтра.
 */
EXTERN mid_filter3i_value_t mid_filter3i_value(mid_filter3i_t* filter);

#endif /* MID_FILTER3I_H */
