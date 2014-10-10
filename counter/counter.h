/**
 * @file counter.h
 * Библиотека для работы со счётчиками и системный счётчик.
 */

#ifndef COUNTER_H
#define	COUNTER_H

#include <stdint.h>
#include "defs/defs.h"

#define COUNTER_BITS 32

#define MAKE_COUNTER_TYPE(type, bits) type ## bits ## _t

typedef volatile MAKE_COUNTER_TYPE(uint, 32) counter_uint_t;
typedef volatile MAKE_COUNTER_TYPE(int, 32) counter_int_t;

typedef counter_uint_t counter_t;
typedef counter_uint_t counter_diff_t;

/**
 * Инициализирует счётчик.
 * @param counter Счётчик.
 */
ALWAYS_INLINE static void counter_init(counter_t* counter)
{
    *counter = 0;
}

/**
 * Инкрементирует счётчик.
 * @param counter Счётчик.
 */
ALWAYS_INLINE static void counter_tick(counter_t* counter)
{
    (*counter) ++;
}

/**
 * Вычисляет разницу между предыдущим счётчиком counter_prev и текущим counter_next.
 * @param counter_next Текущий счётчик.
 * @param counter_prev Предыдущий счётчик.
 * @return Разница между счётчиками.
 */
extern counter_diff_t counter_diff(counter_t* counter_next, counter_t* counter_prev);


/*
 * Системный счётчик.
 */

/**
 * Инициализация системного счётчика.
 * @param ticks_per_sec Число тиков за секунду.
 */
extern void system_counter_init(counter_t ticks_per_sec);

/**
 * Инкремент системного счётчика.
 */
extern void system_counter_tick();

/**
 * Получения разницы между сохранённым и системным счётчиком.
 * @param counter Сохранённый счётчик.
 * @return Разница между счётчиками.
 */
extern counter_diff_t system_counter_diff(counter_t* counter);

/**
 * Получение числа тиков системного счётчика.
 * @return Число тиков системного счётчика.
 */
extern counter_t system_counter_ticks();

/**
 * Число тиков системного счётчика в секунду.
 * @return Число тиков системного счётчика в секунду.
 */
extern counter_t system_counter_ticks_per_sec();

/**
 * Устанавливает значение числа тиков за секунду.
 * @param ticks_per_sec Число тиков за секунду.
 */
extern void system_counter_set_ticks_per_sec(counter_t ticks_per_sec);

#endif	/* COUNTER_H */

