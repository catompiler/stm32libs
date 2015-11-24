#ifndef MENU_H
#define	MENU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "defs/defs.h"
#include "errors/errors.h"
#include "fixed/fixed32.h"



//! Список типов значений элементов меню.
typedef enum _MenuValueType {
    MENU_VALUE_TYPE_NONE = 0, //!< Нет значения.
    MENU_VALUE_TYPE_STRING = 1, //!< Строка.
    MENU_VALUE_TYPE_INT = 2, //!< Целочисленное.
    MENU_VALUE_TYPE_BOOL = 3, //!< Логическое.
    MENU_VALUE_TYPE_FIXED = 4, //!< Число с фиксированной запятой.
    MENU_VALUE_TYPE_ENUM = 5, //!< Списочное.
    MENU_VALUE_TYPE_CUSTOM = 6 //!< Пользовательское.
} menu_value_type_t;


// Предварительная декларация типа значения.
struct _MenuValue;

//! Текстовое значение.
typedef const char* menu_value_string_t;
//! Целочисленное значение.
typedef int32_t menu_value_int_t;
//! Логическое значение.
typedef bool menu_value_bool_t;
//! Число с фиксированной запятой.
typedef fixed32_t menu_value_fixed_t;
//! Списочное значение.
typedef struct _MenuValueEnum {
    struct _MenuValue* values; //!< Значения.
    size_t count; //!< Число значений.
    size_t current; //!< Текущее значение.
} menu_value_enum_t;
//! Пользовательское.
typedef void* menu_value_custom_t;

//! Общий тип значения элемента меню.
typedef struct _MenuValue {
    menu_value_type_t type; //!< Тип значения.
    union {
        menu_value_string_t value_string; //!< Текстовое значение.
        menu_value_int_t value_int; //!< Целочисленное значение.
        menu_value_bool_t value_bool; //!< Логическое значение.
        menu_value_fixed_t value_fixed; //!< Число с фиксированной запятой.
        menu_value_enum_t value_enum; //!< Списочное значение.
        menu_value_custom_t value_custom; //!< Пользовательское значение.
    };
} menu_value_t;


//! Тип идентификатора меню.
typedef uint32_t menu_id_t;
//! Тип флагов меню.
typedef uint32_t menu_flags_t;

//! Предварительная декларация типа элемента меню.
struct _MenuItem;

//! Тип меню.
typedef struct _Menu {
    struct _MenuItem* root; //!< Корневой элемент.
    struct _MenuItem* current; //!< Текущий элемент.
} menu_t;

//! Тип элемента меню.
typedef struct _MenuItem {
    menu_id_t id; //!< Идентификатор.
    
    struct _MenuItem* parent; //!< Родительский элемент.
    struct _MenuItem* child; //!< Дочерний элемент.
    struct _MenuItem* prev; //!< Предыдущий элемент.
    struct _MenuItem* next; //!< Следующий элемент.
    
    const char* text; //!< Текст.
    menu_value_t* value; //!< Значение.
    
    menu_flags_t flags; //!< Флаги.
} menu_item_t;


/**
 * Атрибут для переменной меню.
 */
#ifndef MENU_ATTRIBS
#define MENU_ATTRIBS
#endif

/**
 * Атрибут для переменной значения элемента меню.
 */
#ifndef MENU_VALUE_ATTRIBS
#define MENU_VALUE_ATTRIBS
#endif

/**
 * Атрибут для массива значений элемента меню.
 */
#ifndef MENU_VALUE_ARRAY_ATTRIBS
#define MENU_VALUE_ARRAY_ATTRIBS
#endif

/**
 * Атрибут для переменной элемента меню.
 */
#ifndef MENU_ITEM_ATTRIBS
#define MENU_ITEM_ATTRIBS
#endif

/**
 * Постфикс для генерируемых имён переменных значений элементов меню.
 */
#ifndef MENU_VALUE_NAME_POSTFIX
#define MENU_VALUE_NAME_POSTFIX _value
#endif

//! Декларирует меню.
#define DECLARE_MENU(name) MENU_ATTRIBS menu_t name
//! Декларирует несколько меню.
#define DECLARE_MENUS(...) MENU_ATTRIBS menu_t __VA_ARGS__
//! Инициализация структуры меню.
#define MAKE_MENU(arg_root) { .root = (menu_item_t*)arg_root, .current = (menu_item_t*)arg_root }
//! Обявляет и инициализирует переменную меню.
#define MENU(name, arg_root) DECLARE_MENU(name) = MAKE_MENU(arg_root)


//! Декларирует значение элемента меню.
#define DECLARE_MENU_VALUE(name) MENU_VALUE_ATTRIBS menu_value_t name
//! Декларирует несколько значений элемента меню.
#define DECLARE_MENU_VALUES(...) MENU_VALUE_ATTRIBS menu_value_t __VA_ARGS__
//! Декларирует массив значений элемента меню.
#define DECLARE_MENU_VALUES_ARRAY(name) MENU_VALUE_ARRAY_ATTRIBS menu_value_t name[]
//! Инициализирует массив значений меню.
#define MAKE_MENU_VALUES(...) { __VA_ARGS__ }
//! Объявляет переменную и инициалищирует массив значений элемента меню.
#define MENU_VALUES(name, ...) DECLARE_MENU_VALUES_ARRAY(name) = {__VA_ARGS__}

//! Инициализирует пустое значение элемента меню.
#define MAKE_MENU_VALUE_NONE()\
            { .type = MENU_VALUE_TYPE_NONE }
//! Инициализирует строковое значение элемента меню.
#define MAKE_MENU_VALUE_STRING(arg_value_string)\
            { .type = MENU_VALUE_TYPE_STRING, .value_string = arg_value_string }
//! Инициализирует целочисленное значение элемента меню.
#define MAKE_MENU_VALUE_INT(arg_value_int)\
            { .type = MENU_VALUE_TYPE_INT, .value_int = arg_value_int }
//! Инициализирует логическое значение элемента меню.
#define MAKE_MENU_VALUE_BOOL(arg_value_bool)\
            { .type = MENU_VALUE_TYPE_BOOL, .value_bool = arg_value_bool }
//! Инициализирует значение числа с фиксированной запятой элемента меню.
#define MAKE_MENU_VALUE_FIXED(arg_value_fixed)\
            { .type = MENU_VALUE_TYPE_FIXED, .value_fixed = arg_value_fixed }
//! Инициализирует перечислимое значение элемента меню.
#define MAKE_MENU_VALUE_ENUM(arg_current, arg_count, arg_values)\
            { .type = MENU_VALUE_TYPE_ENUM, .value_enum.values = (menu_value_t*)arg_values,\
              .value_enum.count = arg_count, .value_enum.current = arg_current }
//! Инициализирует пользовательское значение элемента меню.
#define MAKE_MENU_VALUE_CUSTOM(arg_value_custom)\
            { .type = MENU_VALUE_TYPE_CUSTOM, .value_custom = arg_value_custom }

//! Обявляет переменную пустого значения элемента меню.
#define MENU_VALUE_NONE(name)\
        DECLARE_MENU_VALUE(name) = MAKE_MENU_VALUE_NONE()
//! Обявляет переменную строкового значения элемента меню.
#define MENU_VALUE_STRING(name, arg_value_string)\
        DECLARE_MENU_VALUE(name) = MAKE_MENU_VALUE_STRING(arg_value_string)
//! Обявляет переменную целочисленного значения элемента меню.
#define MENU_VALUE_INT(name, arg_value_int)\
        DECLARE_MENU_VALUE(name) = MAKE_MENU_VALUE_INT(arg_value_int)
//! Обявляет переменную логического значения элемента меню.
#define MENU_VALUE_BOOL(name, arg_value_bool)\
        DECLARE_MENU_VALUE(name) = MAKE_MENU_VALUE_BOOL(arg_value_bool)
//! Обявляет переменную значения числа с фиксированной запятой элемента меню.
#define MENU_VALUE_FIXED(name, arg_value_fixed)\
        DECLARE_MENU_VALUE(name) = MAKE_MENU_VALUE_FIXED(arg_value_fixed)
//! Обявляет переменную перичислимого значения элемента меню.
#define MENU_VALUE_ENUM(name, arg_current, arg_count, arg_values)\
        DECLARE_MENU_VALUE(name) = MAKE_MENU_VALUE_ENUM(arg_current, arg_count, arg_values)
//! Обявляет переменную перичислимого значения элемента меню списком значений.
#define MENU_VALUE_ENUM_LIST(name, arg_current, arg_count, ...)\
            MENU_VALUES(CONCAT(name, MENU_VALUE_NAME_POSTFIX), __VA_ARGS__ );\
            DECLARE_MENU_VALUE(name) = MAKE_MENU_VALUE_ENUM(arg_current, arg_count, CONCAT(name, MENU_VALUE_NAME_POSTFIX))
//! Обявляет переменную пользовательского значения элемента меню.
#define MENU_VALUE_CUSTOM(name, arg_value_custom)\
        DECLARE_MENU_VALUE(name) = MAKE_MENU_VALUE_CUSTOM(arg_value_custom)


//! Декларирует элемент меню.
#define DECLARE_MENU_ITEM(name) MENU_ITEM_ATTRIBS menu_item_t name
//! Декларирует несколько элементов меню.
#define DECLARE_MENU_ITEMS(...) MENU_ITEM_ATTRIBS menu_item_t __VA_ARGS__

//! Инициализирует элемент меню.
#define MAKE_MENU_ITEM(arg_id, arg_parent, arg_child, arg_prev, arg_next,\
                       arg_text, arg_value, arg_flags)\
                      { .id = arg_id, .parent = (menu_item_t*)arg_parent, .child = (menu_item_t*)arg_child,\
                        .prev = (menu_item_t*)arg_prev, .next = (menu_item_t*)arg_next, .text = arg_text,\
                        .value = (menu_value_t*)arg_value, .flags = arg_flags }
//! Объявляет переменную и инициализирует элемент меню.
#define MENU_ITEM(name, arg_id, arg_parent, arg_child, arg_prev, arg_next,\
                  arg_text, arg_value, arg_flags)\
        DECLARE_MENU_ITEM(name) = { .id = arg_id, .parent = (menu_item_t*)arg_parent, .child = (menu_item_t*)arg_child,\
                                    .prev = (menu_item_t*)arg_prev, .next = (menu_item_t*)arg_next, .text = arg_text,\
                                    .value = (menu_value_t*)arg_value, .flags = arg_flags }
//! Объявляет переменную и инициализирует элемент подменю.
#define SUBMENU(name, arg_id, arg_parent, arg_child, arg_prev, arg_next, arg_text, arg_flags)\
        DECLARE_MENU_ITEM(name) = { .id = arg_id, .parent = (menu_item_t*)arg_parent, .child = (menu_item_t*)arg_child,\
                                    .prev = (menu_item_t*)arg_prev, .next = (menu_item_t*)arg_next, .text = arg_text,\
                                    .value = NULL, .flags = arg_flags }
//! Объявляет переменную и инициализирует элемент подменю со значением.
#define SUBITEM(name, arg_id, arg_parent, arg_child, arg_prev, arg_next,\
                  arg_text, arg_flags, arg_value)\
        DECLARE_MENU_VALUE(CONCAT(name, MENU_VALUE_NAME_POSTFIX)) = arg_value;\
        DECLARE_MENU_ITEM(name) = { .id = arg_id, .parent = (menu_item_t*)arg_parent, .child = (menu_item_t*)arg_child,\
                                    .prev = (menu_item_t*)arg_prev, .next = (menu_item_t*)arg_next, .text = arg_text,\
                                    .value = (menu_value_t*)&CONCAT(name, MENU_VALUE_NAME_POSTFIX), .flags = arg_flags }
//! Объявляет переменную и инициализирует элемент меню со значением.
#define MENUITEM(name, arg_id, arg_parent, arg_prev, arg_next,\
                  arg_text, arg_flags, arg_value)\
        DECLARE_MENU_VALUE(CONCAT(name, MENU_VALUE_NAME_POSTFIX)) = arg_value;\
        DECLARE_MENU_ITEM(name) = { .id = arg_id, .parent = (menu_item_t*)arg_parent, .child = NULL,\
                                    .prev = (menu_item_t*)arg_prev, .next = (menu_item_t*)arg_next, .text = arg_text,\
                                    .value = (menu_value_t*)&CONCAT(name, MENU_VALUE_NAME_POSTFIX), .flags = arg_flags }
//! Объявляет переменную и инициализирует элемент меню без значения.
#define MENUACTION(name, arg_id, arg_parent, arg_prev, arg_next,\
                  arg_text, arg_flags)\
        DECLARE_MENU_ITEM(name) = { .id = arg_id, .parent = (menu_item_t*)arg_parent, .child = NULL,\
                                    .prev = (menu_item_t*)arg_prev, .next = (menu_item_t*)arg_next, .text = arg_text,\
                                    .value = NULL, .flags = arg_flags }


/**
 * Инициализирует меню.
 * @param menu Меню.
 * @param root Корневой элемент меню.
 * @return Код ошибки.
 */
extern err_t menu_init(menu_t* menu, menu_item_t* root);

/**
 * Получает корневой элемент меню.
 * @param menu Меню.
 * @return Корневой элемент меню.
 */
ALWAYS_INLINE static menu_item_t* menu_root(menu_t* menu)
{
    return menu->root;
}

/**
 * Устанавливает корневой элемент меню.
 * @param menu Меню.
 * @param root Корневой элемент меню.
 * @return Код ошибки.
 */
extern err_t menu_set_root(menu_t* menu, menu_item_t* root);

/**
 * Получает текущий элемент меню.
 * @param menu Меню.
 * @return Текущий элемент меню.
 */
ALWAYS_INLINE static menu_item_t* menu_current(menu_t* menu)
{
    return menu->current;
}

/**
 * Устанавливает текущий элемент меню.
 * @param menu Меню.
 * @param current Текущий элемент меню.
 * @return Код ошибки.
 */
extern err_t menu_set_current(menu_t* menu, menu_item_t* current);

/**
 * Сбрасывает текущий элемент меню.
 * @param menu Меню.
 */
extern void menu_reset_current(menu_t* menu);

/**
 * Поднимает текущий элемент меню на уровень выше.
 * @param menu Меню.
 * @return true в случае успеха, иначе false.
 */
extern bool menu_up(menu_t* menu);

/**
 * Поднимает текущий элемент меню на уровень ниже.
 * @param menu Меню.
 * @return true в случае успеха, иначе false.
 */
extern bool menu_down(menu_t* menu);

/**
 * Переходит на следующий элемент меню.
 * @param menu Меню.
 * @return true в случае успеха, иначе false.
 */
extern bool menu_next(menu_t* menu);

/**
 * Переходит на предыдущий элемент меню.
 * @param menu Меню.
 * @return true в случае успеха, иначе false.
 */
extern bool menu_prev(menu_t* menu);

/**
 * Инициализирует элемент меню.
 * @param item Элемент меню.
 * @param text Текст.
 * @return Код ошибки.
 */
extern err_t menu_item_init(menu_item_t* item, const char* text);

/**
 * Получает идентификатор элемента меню.
 * @param item Элемент меню.
 * @return Идентификатор элемента меню.
 */
ALWAYS_INLINE static menu_id_t menu_item_id(menu_item_t* item)
{
    return item->id;
}

/**
 * Устанавливает идентификатор элемента меню.
 * @param item Элемент меню.
 * @param id Идентификатор элемента меню.
 */
ALWAYS_INLINE static void menu_item_set_id(menu_item_t* item, menu_id_t id)
{
    item->id = id;
}

/**
 * Получает текст элемента меню.
 * @param item Элемент меню.
 * @return Текст элемента меню.
 */
ALWAYS_INLINE static const char* menu_item_text(menu_item_t* item)
{
    return item->text;
}

/**
 * Устанавливает текст элемента меню.
 * @param item Элемент меню.
 * @param text Текст элемента меню.
 */
ALWAYS_INLINE static void menu_item_set_text(menu_item_t* item, const char* text)
{
    item->text = text;
}

/**
 * Получает флаги элемента меню.
 * @param item Элемент меню.
 * @return Флаги элемента меню.
 */
ALWAYS_INLINE static menu_flags_t menu_item_flags(menu_item_t* item)
{
    return item->flags;
}

/**
 * Устанавливает флаги элемента меню.
 * @param item Элемент меню.
 * @param flags Флаги элемента меню.
 */
ALWAYS_INLINE static void menu_item_set_flags(menu_item_t* item, menu_flags_t flags)
{
    item->flags = flags;
}

/**
 * Получает предка элемента меню.
 * @param item Элемент меню.
 * @return предок элемента меню.
 */
ALWAYS_INLINE static menu_item_t* menu_item_parent(menu_item_t* item)
{
    return item->parent;
}

/**
 * Устанавливает предка элемента меню.
 * @param item Элемент меню.
 * @param parent предок элемента меню.
 */
ALWAYS_INLINE static void menu_item_set_parent(menu_item_t* item, menu_item_t* parent)
{
    item->parent = parent;
}

/**
 * Получает потомка элемента меню.
 * @param item Элемент меню.
 * @return Потомок элемента меню.
 */
ALWAYS_INLINE static menu_item_t* menu_item_child(menu_item_t* item)
{
    return item->child;
}

/**
 * Устанавливает потомка элемента меню.
 * @param item Элемент меню.
 * @param child Потомок элемента меню.
 */
ALWAYS_INLINE static void menu_item_set_child(menu_item_t* item, menu_item_t* child)
{
    item->child = child;
}

/**
 * Получает предыдущий элемент меню.
 * @param item Элемент меню.
 * @return Предыдущие элемент меню.
 */
ALWAYS_INLINE static menu_item_t* menu_item_prev(menu_item_t* item)
{
    return item->prev;
}

/**
 * Устанавливает предыдущий элемент меню.
 * @param item Элемент меню.
 * @param prev Предыдущий элемент меню.
 */
ALWAYS_INLINE static void menu_item_set_prev(menu_item_t* item, menu_item_t* prev)
{
    item->prev = prev;
}

/**
 * Получает следующий элемент меню.
 * @param item Элемент меню.
 * @return Следующий элемент меню.
 */
ALWAYS_INLINE static menu_item_t* menu_item_next(menu_item_t* item)
{
    return item->next;
}

/**
 * Устанавливает следующий элемент меню.
 * @param item Элемент меню.
 * @param next Следующий элемент меню.
 */
ALWAYS_INLINE static void menu_item_set_next(menu_item_t* item, menu_item_t* next)
{
    item->next = next;
}

/**
 * Получает первый элемент в пределах родителя.
 * @param item Элемент меню.
 * @return Первый элемент меню.
 */
extern menu_item_t* menu_item_first(menu_item_t* item);

/**
 * Получает последний элемент в пределах родителя.
 * @param item Элемент меню.
 * @return Последний элемент меню.
 */
extern menu_item_t* menu_item_last(menu_item_t* item);

/**
 * Связывает два элемента меню.
 * @param prev Предыдущий элемент меню.
 * @param next Следующий элемент меню.
 */
extern void menu_item_link(menu_item_t* prev, menu_item_t* next);

/**
 * Получает значение элемента меню.
 * @param item Элемент меню.
 * @return Значение элемента меню.
 */
ALWAYS_INLINE static menu_value_t* menu_item_value(menu_item_t* item)
{
    return item->value;
}

/**
 * Устанавливает значение элемента меню.
 * @param item Элемент меню.
 * @param value Значение элемента меню.
 */
ALWAYS_INLINE static void menu_item_set_value(menu_item_t* item, menu_value_t* value)
{
    item->value = value;
}

/**
 * Получает флаг наличия значения элемента меню.
 * @param item Элемент меню.
 * @return Флаг наличия значения элемента меню.
 */
ALWAYS_INLINE static bool menu_item_has_value(menu_item_t* item)
{
    return item->value != NULL;
}

/**
 * Инициализирует значение элемента меню.
 * @param value Значение элемента меню.
 * @return Код ошибки.
 */
extern err_t menu_value_init(menu_value_t* value);

/**
 * Получает тип значения элемента меню.
 * @param value Значение элемента меню.
 * @return Тип значения элемента меню.
 */
ALWAYS_INLINE static menu_value_type_t menu_value_type(menu_value_t* value)
{
    return value->type;
}

/**
 * Получает строковое значениче элемента меню.
 * @param value Значение элемента меню.
 * @return Строка - значение элемента меню.
 */
ALWAYS_INLINE static menu_value_string_t menu_value_string(menu_value_t* value)
{
    return value->value_string;
}

/**
 * Устанавливает строковое значение элемента меню.
 * @param value Значение элемента меню.
 * @param val Строка.
 */
ALWAYS_INLINE static void menu_value_set_string(menu_value_t* value, menu_value_string_t val)
{
    value->type = MENU_VALUE_TYPE_STRING;
    value->value_string = val;
}

/**
 * Получает целочисленное значениче элемента меню.
 * @param value Значение элемента меню.
 * @return Число - значение элемента меню.
 */
ALWAYS_INLINE static menu_value_int_t menu_value_int(menu_value_t* value)
{
    return value->value_int;
}

/**
 * Устанавливает целочисленное значение элемента меню.
 * @param value Значение элемента меню.
 * @param val Число.
 */
ALWAYS_INLINE static void menu_value_set_int(menu_value_t* value, menu_value_int_t val)
{
    value->type = MENU_VALUE_TYPE_INT;
    value->value_int = val;
}

/**
 * Получает логическое значениче элемента меню.
 * @param value Значение элемента меню.
 * @return Логическое значение элемента меню.
 */
ALWAYS_INLINE static menu_value_bool_t menu_value_bool(menu_value_t* value)
{
    return value->value_bool;
}

/**
 * Устанавливает логическое значение элемента меню.
 * @param value Значение элемента меню.
 * @param val Логическое значение.
 */
ALWAYS_INLINE static void menu_value_set_bool(menu_value_t* value, menu_value_bool_t val)
{
    value->type = MENU_VALUE_TYPE_BOOL;
    value->value_bool = val;
}

/**
 * Получает значениче с фиксированной запятой элемента меню.
 * @param value Значение элемента меню.
 * @return значение с фиксированной запятой элемента меню.
 */
ALWAYS_INLINE static menu_value_fixed_t menu_value_fixed(menu_value_t* value)
{
    return value->value_fixed;
}

/**
 * Устанавливает значение с фиксированной запятой элемента меню.
 * @param value Значение элемента меню.
 * @param val Значение с фиксированной запятой.
 */
ALWAYS_INLINE static void menu_value_set_fixed(menu_value_t* value, menu_value_fixed_t val)
{
    value->type = MENU_VALUE_TYPE_FIXED;
    value->value_fixed = val;
}

/**
 * Получает списочное значениче элемента меню.
 * @param value Значение элемента меню.
 * @return Списочное значение элемента меню.
 */
ALWAYS_INLINE static menu_value_enum_t* menu_value_enum(menu_value_t* value)
{
    return &value->value_enum;
}

/**
 * Устанавливает списочное значение элемента меню.
 * @param value Значение элемента меню.
 * @param val Списочное значение элемента меню.
 * @return Код ошибки.
 */
extern err_t menu_value_set_enum(menu_value_t* value, menu_value_enum_t* val);

/**
 * Получает текущий индекс списка значений элемента меню.
 * @param value Значение элемента меню.
 * @return Текущей индекс списка значений элемента меню.
 */
ALWAYS_INLINE static size_t menu_value_enum_current(menu_value_t* value)
{
    return value->value_enum.current;
}

/**
 * Устанавливает текущий индекс списка значений элемента меню.
 * @param value Значение элемента меню.
 * @param current Текущей индекс списка значений элемента меню.
 */
ALWAYS_INLINE static void menu_value_enum_set_current(menu_value_t* value, size_t current)
{
    if(current < value->value_enum.count) value->value_enum.current = current;
}

/**
 * Получает число значений списка значений элемента меню.
 * @param value Значение элемента меню.
 * @return Число значений списка значений элемента меню.
 */
ALWAYS_INLINE static size_t menu_value_enum_count(menu_value_t* value)
{
    return value->value_enum.count;
}

/**
 * Устанавливает число значений списка значений элемента меню.
 * @param value Значение элемента меню.
 * @param count Число значений списка значений элемента меню.
 */
extern err_t menu_value_enum_set_count(menu_value_t* value, size_t count);

/**
 * Получает значения списка значений элемента меню.
 * @param value Значение элемента меню.
 * @return Значения списка значений элемента меню.
 */
ALWAYS_INLINE static menu_value_t* menu_value_enum_values(menu_value_t* value)
{
    return value->value_enum.values;
}

/**
 * Устанавливает значения списка значений элемента меню.
 * @param value Значение элемента меню.
 * @param values Значения списка значений элемента меню.
 */
extern err_t menu_value_enum_set_values(menu_value_t* value, menu_value_t* values);

/**
 * Получает текущие значение списка значений элемента меню.
 * @param value Значение элемента меню.
 * @return Текущее значение списка значений элемента меню.
 */
extern menu_value_t* menu_value_enum_current_value(menu_value_t* value);


/*
 * Пример.
 *
 * Создание такого меню:
 * 
 * |-item1
 * |-item2
 * |-item3
 *     |-item4
 *     |-item5
 * 

// Объявление элементов меню.
DECLARE_MENU_ITEMS(item1, item2, item3, item4, item5);

// Создание меню.
MENU(menu0, &item1);

// Создание списка значений для значения элемента 5.
MENU_VALUES(menu_values_en_dis, MAKE_MENU_VALUE_STRING("Enabled"), MAKE_MENU_VALUE_STRING("Disabled"));

// Создание самого меню.

//        Имя  ID  Предок  Предыдущий Следующий Текст      Флаги Значения
MENUITEM(item1, 0, NULL,   NULL,      &item2,   "Menu0_0", 0,    MAKE_MENU_VALUE_STRING("Text"));
MENUITEM(item2, 0, NULL,   &item1,    &item3,   "Menu0_1", 0,    MAKE_MENU_VALUE_INT(123));

//         Имя  ID  Предок  Потомок Предыдущий Следующий    Текст      Флаги
SUBMENU (item3, 0,  NULL,   &item4, &item2,    NULL,        "Menu0_2", 0);

//         Имя  ID Предок  Предыдущий Следующий Текст      Флаги Значения
MENUITEM(item4, 0, &item3, NULL,      &item5,   "Menu1_0", 0,    MAKE_MENU_VALUE_BOOL(true));
MENUITEM(item5, 0, &item3, &item4,    NULL,     "Menu1_1", 0,    MAKE_MENU_VALUE_ENUM(0, 2, menu_values_en_dis));

*/

#endif	/* MENU_H */
