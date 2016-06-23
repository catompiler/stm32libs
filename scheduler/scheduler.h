/**
 * @file scheduler.h Библиотека планировщика задач.
 * Используются приоритеты и кооперативная многозадачность.
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "future/future.h"
#include "errors/errors.h"
#include "list/list.h"
#include "defs/defs.h"


//! Ошибочный идентификатор задачи.
#define INVALID_TASK_ID 0


//! Тип идентификатора задачи.
typedef uint32_t task_id_t;

//! Тип приоритета задачи.
typedef uint32_t task_priority_t;

//! Тип функции задачи.
typedef void* (*task_proc_t)(void*);

typedef enum _Task_Flag {
    TASK_RUN_ONCE = 1
} task_flag_t;

//! Тип флагов задачи.
typedef uint32_t task_flags_t;

//! Тип дескриптора задачи.
typedef struct _Task_Descr_t {
    list_item_t list_item; //!< Элемент списка задач.
    task_id_t tid; //!< Идентификатор задачи.
    task_priority_t priority; //!< Приоритет задачи.
    task_proc_t proc; //!< Функция задачи.
    void* arg; //!< Аргумент задачи.
    future_t* future; //!< Будущее задачи.
    task_flags_t flags; //!< Флаги задачи.
} task_descr_t;

//! Декларация буфера задач.
#define TASKS_BUFFER(name, size) task_descr_t name[size]

/**
 * Инициализирует планеровщик.
 * Планеровщик использует буфер дескрипторов
 * заданного размера для добавления задач.
 * Размер буфера определяет максимальное число
 * добавленных задач.
 * @param buffer Массив дескрипторов задач.
 * @param count Число дескрипторов.
 * @return Код ошибки.
 */
EXTERN err_t scheduler_init(task_descr_t* buffer, size_t count);

/**
 * Получает идентификатор текущей задачи.
 * @return Идентификатор текущей задачи.
 */
EXTERN task_id_t scheduler_current_task_id(void);

/**
 * Добавляет задачу.
 * @param proc Функция задачи.
 * @param priority Приоритет задачи.
 * @param arg Аргумент задачи.
 * @return Идентификатор задачи, при ошибке возвращает INVALID_TASK_ID.
 */
EXTERN task_id_t scheduler_add_task(task_proc_t proc, task_priority_t priority, void* arg, task_flags_t flags, future_t* future);

/**
 * Удаляет задачу.
 * @param tid Идентификатор задачи.
 * @return Флаг удаления задачи.
 */
EXTERN bool scheduler_remove_task(task_id_t tid);

/**
 * Главный цикл планеровщика.
 * Запускает очередную задачу.
 * @return Флаг запуска задачи, false если задач нет.
 */
EXTERN bool scheduler_process(void);

#endif /* SCHEDULER_H */
