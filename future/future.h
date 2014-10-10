/**
 * @file future.h
 * Библиотека для работы с ещё не наступившем событием.
 */

#ifndef FUTURE_H
#define	FUTURE_H

#include <stdbool.h>

/**
 * Структура будущего.
 */
typedef struct _Future {
    //! Результат.
    void * volatile result;
    //! Флаг окончания.
    volatile bool done;
    //! Флаг выполнения.
    volatile bool running;
}future_t;

/**
 * Инициализирует будущее.
 * Незавершённое, невыполняющееся, с нулевым результатом.
 * @param future Будущее.
 */
extern void future_init(future_t* future);

/**
 * Получает результат будущего.
 * @param future Будущее.
 * @return Результат.
 */
extern void* future_result(future_t* future);

/**
 * Устанавливает результат.
 * @param future Будущее.
 * @param result Результат.
 */
extern void future_set_result(future_t* future, void* result);

/**
 * Получает флаг завершения.
 * @param future Будущее.
 * @return Флаг завершения.
 */
extern bool future_done(future_t* future);

/**
 * Устанавливает флаг завершения.
 * @param future Будущее.
 * @param done Флаг завершения.
 */
extern void future_set_done(future_t* future, bool done);

/**
 * Получает флаг выполнения.
 * @param future Будущее.
 * @return Флаг выполнения.
 */
extern bool future_running(future_t* future);

/**
 * Устанавливает флаг выполнения.
 * @param future Будущее.
 * @param running Флаг выполнения.
 */
extern void future_set_running(future_t* future, bool running);

/**
 * Запускает выполнение будущего (done = false; running = true;).
 * @param future Будущее.
 */
extern void future_start(future_t* future);

/**
 * Завершает выполнение будущего (done = true; running = false;).
 * @param future Будущее.
 * @param result Результат.
 */
extern void future_finish(future_t* future, void* result);

/**
 * Ждёт окончания выполнения будущего.
 * @param future Будущее.
 */
extern void future_wait(future_t* future);

#endif	/* FUTURE_H */

