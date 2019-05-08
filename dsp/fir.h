/**
 * @file fir.h Библиотека КИХ-фильтра.
 */

#ifndef FIR_H_
#define FIR_H_

#include "errors/errors.h"
#include <stdint.h>
#include <stddef.h>
#include "q15/q15.h"

//! Структура КИХ-фильтра.
typedef struct _Fir {
    size_t size; //!< Число элементов фильтра (Размер буфера данных).
    q15_t* coefs; //!< Коэффициенты.
    q15_t* data; //!< Данные.
    size_t index; //!< Индекс.
} fir_t;

/**
 * Инициализирует фильтр.
 * @param fir Фильтр.
 * @param coefs Коэффициенты фильтра.
 * @param data Буфер данных, проходящих фильтр.
 * @param size Число коэффициентов и размер буфера данных фильтра.
 * @return Код ошибк.
 */
extern err_t fir_init(fir_t* fir, q15_t* coefs, q15_t* data, size_t size);

/**
 * Сбрасывает фильтр.
 * @param fir Фильтр.
 */
extern void fir_reset(fir_t* fir);

/**
 * Вставляет данные в фильтр.
 * Использует индекс.
 * @param fir Фильтр.
 * @param data Данные.
 */
extern void fir_put(fir_t* fir, q15_t data);

/**
 * Вычисляет значение фильтра.
 * Использует 64 бит аккумулятор.
 * Использует индекс.
 * @param fir Фильтр.
 * @return Отфильтрованное значение.
 */
extern q15_t fir_calc(fir_t* fir);

/**
 * Добавляет данные в фильтр
 * и вычисляет значение фильтра.
 * Не использует индекс,
 * сдвигает данные в массиве
 * одновременно с вычислением.
 * @param fir Фильтр.
 * @param data Данные.
 * @return Отфильтрованное значение.
 */
extern q15_t fir_filter(fir_t* fir, q15_t data);

#endif /* FIR_H_ */
