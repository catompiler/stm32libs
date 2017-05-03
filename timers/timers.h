/**
 * @file timers.h Библиотека таймеров.
 */

#ifndef TIMERS_H
#define TIMERS_H



#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/time.h>
#include "future/future.h"
#include "errors/errors.h"
#include "list/list.h"
#include "defs/defs.h"


//! Ошибочный идентификатор таймера.
#define INVALID_TIMER_ID 0


//! Тип идентификатора таймера.
typedef uint32_t timer_id_t;

//! Тип функции таймера.
typedef void* (*timer_proc_t)(void*);

//! Тип дескриптора таймера.
typedef struct _Timer_Descr_t {
    list_item_t list_item; //!< Элемент списка таймеров.
    timer_id_t tid; //!< Идентификатор таймера.
    timer_proc_t proc; //!< Функция таймера.
    struct timeval t_interval; //!< Интервал таймера.
    struct timeval t_value; //!< Следующее время срабатывания таймера.
    void* arg; //!< Аргумент таймера.
    future_t* future; //!< Будущее таймера.
} timer_descr_t;

//! Декларация буфера таймеров.
#define TIMERS_BUFFER(name, size) timer_descr_t name[size]

/**
 * Функция обратного вызова однократного запуска таймера для отсчёта времени.
 * @param tv_time Время, через которое должен сработать таймер.
 */
typedef void (*timers_setup_timer_callback_t)(const struct timeval* tv_time);

//! Структура инициализации таймеров.
typedef struct _Timers_Init_Struct {
    timer_descr_t* buffer; //!< Массив дескрипторов таймеров.
    size_t count; //!< Число дескрипторов.
    timers_setup_timer_callback_t setup_timer_callback; //!< Функция обратного вызова запуска таймера.
} timers_init_t;

/**
 * Инициализирует таймеры.
 * Таймеры используют буфер дескрипторов
 * заданного размера для добавления таймеров.
 * Размер буфера определяет максимальное число
 * добавленных таймеров.
 * @param timers_is Структура инициализации таймеров.
 * @return Код ошибки.
 */
EXTERN err_t timers_init(timers_init_t* timers_is);

/**
 * Получает идентификатор текущего таймера.
 * @return Идентификатор текущего таймера.
 */
EXTERN timer_id_t timers_current_timer_id(void);

/**
 * Добавляет таймер.
 * @param proc Функция таймера.
 * @param t_start Время срабатывания таймера.
 * Не может быть NULL или 0.
 * @param t_int Интервал периодического срабатывания таймера.
 * NULL или 0 для однократного запуска.
 * @param arg Аргумент таймера.
 * @return Идентификатор таймера, при ошибке возвращает INVALID_TIMER_ID.
 */
EXTERN timer_id_t timers_add_timer(timer_proc_t proc, struct timeval* t_start, struct timeval* t_int, void* arg, future_t* future);

/**
 * Удаляет таймер.
 * @param tid Идентификатор таймера.
 * @return Флаг удаления таймера.
 */
EXTERN bool timers_remove_timer(timer_id_t tid);

/**
 * Обработчик срабатывания таймера отсчёта времени.
 */
EXTERN void timers_timer_handler(void);

#endif /* TIMERS_H */
