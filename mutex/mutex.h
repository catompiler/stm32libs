/**
 * @file mutex.h Реализация мютекса.
 */
#ifndef MUTEX_H
#define	MUTEX_H

#include <stdint.h>
#include <stdbool.h>

//! Состояния мютекса.
//! Мютекс не заблокирован.
#define MUTEX_UNLOCKED 0
//! Мютекс заблокирован.
#define MUTEX_LOCKED 1

//! Тип мютекса.
typedef uint8_t mutex_t;

/**
 * Инициализирует мютекс.
 * @param mutex Мютекс.
 */
extern void mutex_init(mutex_t* mutex);

/**
 * Пытается захватить мютекс.
 * @param mutex Мютекс.
 * @return Истина в случае успешного захвата мютекса, иначе false.
 */
extern bool mutex_trylock(mutex_t* mutex);

/**
 * Ждёт освобождения мютекса и блокирует его.
 * @param mutex Мютекс.
 */
extern void mutex_lock(mutex_t* mutex);

/**
 * Освобождает мютекс.
 * @param mutex Мютекс.
 */
extern void mutex_unlock(mutex_t* mutex);

#endif	/* MUTEX_H */

