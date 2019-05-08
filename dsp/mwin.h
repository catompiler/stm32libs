/**
 * @file mwin.h Библиотека скользящего окна.
 */

#ifndef MWIN_H_
#define MWIN_H_

#include <stddef.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "q15/q15.h"


//! Тип данных скользящего окна.
typedef q15_t mwin_data_t;

//! Тип суммы данных скользящего окна.
typedef iq15_t mwin_sum_t;

//! Структура скользящего окна.
typedef struct _MWin {
    mwin_data_t* data; //!< Данные.
    size_t size; //!< Размер данных.
    size_t count; //!< Число данных.
    size_t index; //!< Индекс.
    mwin_sum_t sum; //!< Сумма.
} mwin_t;

/**
 * Инициализирует скользящее окно.
 * @param mwin Скользящее окно.
 * @param data Буфер данных.
 * @param size Размер буфера данных.
 * @return Код ошибки.
 */
extern err_t mwin_init(mwin_t* mwin, mwin_data_t* data, size_t size);

/**
 * Сбрасывает скользящее окно.
 * @param mwin Скользящее окно.
 */
extern void mwin_reset(mwin_t* mwin);

/**
 * Помещает данные в скользящее окно.
 * @param mwin Скользящее окно.
 * @param data Данные.
 */
extern void mwin_put(mwin_t* mwin, mwin_data_t data);

/**
 * Получает сумму данных в скользящем окне.
 * @param mwin Скользящее окно.
 * @return Сумма данных.
 */
ALWAYS_INLINE static mwin_sum_t mwin_sum(mwin_t* mwin)
{
    return mwin->sum;
}

/**
 * Получает размер скользящего окна.
 * @param mwin Скользящее окно.
 * @return Размер скользящего окна.
 */
ALWAYS_INLINE static size_t mwin_size(mwin_t* mwin)
{
    return mwin->size;
}

/**
 * Получает число данных в скользящем окне.
 * @param mwin Скользящее окно.
 * @return Число данных в скользящем окне.
 */
ALWAYS_INLINE static size_t mwin_count(mwin_t* mwin)
{
    return mwin->count;
}

#endif /* MWIN_H_ */
