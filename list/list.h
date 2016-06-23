#ifndef LIST_H
#define	LIST_H

#include <stddef.h>
#include <stdbool.h>
#include "errors/errors.h"
#include "defs/defs.h"

//! Предварительная декларация структуры элемента списка.
struct _List_Item;

//! Тип списка.
typedef struct _List {
    struct _List_Item* head; //!< Первый элемент списка.
    struct _List_Item* tail; //!< Последний элемент списка.
} list_t;

//! Тип элемента списка.
typedef struct _List_Item {
    void* data; //!< Данные элемента списка.
    struct _List_Item* prev; //!< Предыдущий элемент.
    struct _List_Item* next; //!< Следующий элемент.
} list_item_t;


/**
 * Инициализация списка.
 * @param list Список.
 * @return Код ошибки.
 */
EXTERN err_t list_init(list_t* list);

/**
 * Получает первый элемент списка.
 * @param list Список.
 * @return Первый элемент списка.
 */
ALWAYS_INLINE static list_item_t* list_head(list_t* list)
{
    return list->head;
}

/**
 * Получает последний элемент списка.
 * @param list Список.
 * @return Последний элемент списка.
 */
ALWAYS_INLINE static list_item_t* list_tail(list_t* list)
{
    return list->tail;
}

/**
 * Добавляет элемент в конец списка.
 * @param list Список.
 * @param item Элемент списка.
 * @return Код ошибки.
 */
EXTERN err_t list_append(list_t* list, list_item_t* item);

/**
 * Добавляет элемент в начало списка.
 * @param list Список.
 * @param item Элемент списка.
 * @return Код ошибки.
 */
EXTERN err_t list_prepend(list_t* list, list_item_t* item);

/**
 * Добавляет элемент в список после заданного элемента.
 * @param list Список.
 * @param ref Место вставки.
 * @param item Элемент списка.
 * @return Код ошибки.
 */
EXTERN err_t list_insert_after(list_t* list, list_item_t* ref, list_item_t* item);

/**
 * Добавляет элемент в список перед заданным элементом.
 * @param list Список.
 * @param ref Место вставки.
 * @param item Элемент списка.
 * @return Код ошибки.
 */
EXTERN err_t list_insert_before(list_t* list, list_item_t* ref, list_item_t* item);

/**
 * Добавляет элемент в сортированный список.
 * @param list Список.
 * @param item Элемент списка.
 * @param compare Функция сравнения.
 * @return Код ошибки.
 */
EXTERN err_t list_insert_sorted(list_t* list, list_item_t* item, int (*compare)(const void*, const void*));

/**
 * Удаляет элемент из списка.
 * @param list Список.
 * @param item Элемент списка.
 * @return Код ошибки.
 */
EXTERN err_t list_remove(list_t* list, list_item_t* item);

/**
 * Создаёт новый элемент списка с заданными данными.
 * и добавляет его в конец списка.
 * @param list Список.
 * @param data Данные элемента списка.
 * @return Новый элемент списка.
 */
EXTERN list_item_t* list_append_new(list_t* list, void* data);

/**
 * Создаёт новый элемент списка с заданными данными.
 * и добавляет его в начало списка.
 * @param list Список.
 * @param data Данные элемента списка.
 * @return Новый элемент списка.
 */
EXTERN list_item_t* list_prepend_new(list_t* list, void* data);

/**
 * Создаёт новый элемент списока после заданного элемента.
 * @param list Список.
 * @param ref Место вставки.
 * @param data Данные элемента списка.
 * @return Новый элемент списка.
 */
EXTERN list_item_t* list_insert_after_new(list_t* list, list_item_t* ref, void* data);

/**
 * Создаёт новый элемент списока перед заданным элементом.
 * @param list Список.
 * @param ref Место вставки.
 * @param data Данные элемента списка.
 * @return Новый элемент списка.
 */
EXTERN list_item_t* list_insert_before_new(list_t* list, list_item_t* ref, void* data);

/**
 * Создаёт новый элемент в сортированном списке.
 * @param list Список.
 * @param data Данные элемента списка.
 * @param compare Функция сравнения.
 * @return Новый элемент списка.
 */
EXTERN list_item_t* list_insert_sorted_new(list_t* list, void* data, int (*compare)(const void*, const void*));

/**
 * Удаляет элемент из списка и уничтожает его.
 * @param list Список.
 * @param item Элемент списка.
 * @return Код ошибки.
 */
EXTERN err_t list_delete(list_t* list, list_item_t* item);

/**
 * Получает первый элемент списка.
 * @param list Список.
 * @return Первый элемент списка.
 */
ALWAYS_INLINE static list_item_t* list_first(list_t* list)
{
    return list->head;
}

/**
 * Получает следующий элемент списка.
 * @param item Элемент списка.
 * @return Следующий элемент списка.
 */
ALWAYS_INLINE static list_item_t* list_next(list_item_t* item)
{
    return item->next;
}

/**
 * Получает последний элемент списка.
 * @param list Список.
 * @return Последний элемент списка.
 */
ALWAYS_INLINE static list_item_t* list_last(list_t* list)
{
    return list->tail;
}

/**
 * Получает предыдущий элемент списка.
 * @param item Элемент списка.
 * @return Предыдущий элемент списка.
 */
ALWAYS_INLINE static list_item_t* list_prev(list_item_t* item)
{
    return item->prev;
}

/**
 * Ищет элемент в списке с заданными данными.
 * @param list Список.
 * @param data Данные списка.
 * @return Элемент списка.
 */
EXTERN list_item_t* list_find(list_t* list, void* data);

/**
 * Проверяет наличие элемента в списке.
 * @param list Список.
 * @param item Элемент списка.
 * @return true если элемент существует, иначе false.
 */
EXTERN bool list_contains(list_t* list, list_item_t* item);

/**
 * Обходит список.
 * @param list Список.
 * @param proc Функция обработки элемента.
 */
EXTERN void list_foreach(list_t* list, void (*proc)(void* item_data));

/**
 * Обходит список.
 * @param list Список.
 * @param proc Функция обработки элемента.
 * @param user_data Пользовательские данные.
 */
EXTERN void list_foreach2_first(list_t* list, void (*proc)(void* user_data, void* item_data), void* user_data);

/**
 * Обходит список.
 * @param list Список.
 * @param proc Функция обработки элемента.
 * @param user_data Пользовательские данные.
 */
EXTERN void list_foreach2_second(list_t* list, void (*proc)(void* item_data, void* user_data), void* user_data);

/**
 * Реверсно обходит список.
 * @param list Список.
 * @param proc Функция обработки элемента.
 */
EXTERN void list_rforeach(list_t* list, void (*proc)(void* item_data));

/**
 * Реверсно обходит список.
 * @param list Список.
 * @param proc Функция обработки элемента.
 * @param user_data Пользовательские данные.
 */
EXTERN void list_rforeach2_first(list_t* list, void (*proc)(void* user_data, void* item_data), void* user_data);

/**
 * Реверсно обходит список.
 * @param list Список.
 * @param proc Функция обработки элемента.
 * @param user_data Пользовательские данные.
 */
EXTERN void list_rforeach2_second(list_t* list, void (*proc)(void* item_data, void* user_data), void* user_data);

/**
 * Получает размер списка.
 * @param list Список.
 * @return Размер списка.
 */
EXTERN size_t list_size(list_t* list);

/**
 * Получает флаг пустоты списка.
 * @param list Список.
 * @return true если список пуст, иначе false.
 */
ALWAYS_INLINE static bool list_empty(list_t* list)
{
    return list->head == NULL;
}

/**
 * Инициализирует элемент списка.
 * @param item Элемент списка.
 * @return Код ошибки.
 */
EXTERN err_t list_item_init(list_item_t* item);

/**
 * Инициализирует элемент списка с данными.
 * @param item Элемент списка.
 * @param data Данные.
 * @return Код ошибки.
 */
EXTERN err_t list_item_init_data(list_item_t* item, void* data);

/**
 * Получает данные элемента списка.
 * @param item Элемент списка.
 * @return Данные элемента списка.
 */
ALWAYS_INLINE static void* list_item_data(list_item_t* item)
{
    return item->data;
}

/**
 * Устанавливает данные элемента списка.
 * @param item Элемент списка.
 * @param data Данные элемента списка.
 */
ALWAYS_INLINE static void list_item_set_data(list_item_t* item, void* data)
{
    item->data = data;
}

#endif	/* LIST_H */
