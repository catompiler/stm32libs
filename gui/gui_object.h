/**
 * @file gui_object.h Реализация объекта графического интерфейса.
 */

#ifndef GUI_OBJECT_H
#define	GUI_OBJECT_H

#include "gui.h"
#include "list/list.h"

//! Тип структуры объекта графического интерфейса.
typedef struct _Gui_Object gui_object_t;

//! Структура объекта графического интерфейса.
struct _Gui_Object {
    list_item_t child; //!< Элемент списка дочерних объектов родителя.
    gui_object_t* parent; //!< Родительский объект.
    list_t childs; //!< Дочерние объекты.
    gui_t* gui; //!< Графический интерфейс.
};

//! Приводит указатель object к типу объекта.
#define GUI_OBJECT(object) ((gui_object_t*)(object))

/**
 * Инициализирует объект графического интерфейса.
 * @param object Объект графического интерфейса.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
extern err_t gui_object_init(gui_object_t* object, gui_t* gui);

/**
 * Инициализирует объект графического интерфейса
 * как потомок заданного родителя.
 * @param object Объект графического интерфейса.
 * @param gui Графический интерфейс.
 * @param parent Родитель объекта.
 * @return Код ошибки.
 */
extern err_t gui_object_init_parent(gui_object_t* object, gui_t* gui, gui_object_t* parent);

/**
 * Получает графический интерфейс объекта графического интерфейса.
 * @param object Объект графического интерфейса.
 * @return  Графический интерфейс.
 */
ALWAYS_INLINE static gui_t* gui_object_gui(gui_object_t* object)
{
    return object->gui;
}

/**
 * Получает родителя объекта графического интерфейса.
 * @param object Объект графического интрефейса.
 * @return Родитель объекта графического интерфейса.
 */
ALWAYS_INLINE static gui_object_t* gui_object_parent(gui_object_t* object)
{
    return object->parent;
}

/**
 * Устанавливает родителя объекта графического интерфейса.
 * @param object Объект графического интрефейса.
 * @param parent Родитель объекта графического интерфейса.
 */
extern void gui_object_set_parent(gui_object_t* object, gui_object_t* parent);

/**
 * Добавляет дочерний объект графического интерфейса.
 * @param object Объект графического интерфейса.
 * @param child Дочерний объект графического интерфейса.
 */
extern void gui_object_add_child(gui_object_t* object, gui_object_t* child);

/**
 * Удаляет дочерний объект графического интерфейса.
 * @param object Объект графического интерфейса.
 * @param child Дочерний объект графического интерфейса.
 */
extern void gui_object_remove_child(gui_object_t* object, gui_object_t* child);

/**
 * Получает первого потомка объекта графического интерфейса.
 * @param object Объект графического интерфейса.
 * @return Первый потомок объекта графического интерфейса.
 */
extern gui_object_t* gui_object_first_child(gui_object_t* object);

/**
 * Получает следующего потомка объекта графического интерфейса.
 * @param cur_child Текущий потомок объекта графического интерфейса.
 * @return Следующий потомок объекта графического интерфейса.
 */
extern gui_object_t* gui_object_next_child(gui_object_t* cur_child);

/**
 * Получает последнего потомка объекта графического интерфейса.
 * @param object Объект графического интерфейса.
 * @return Последний потомок объекта графического интерфейса.
 */
extern gui_object_t* gui_object_last_child(gui_object_t* object);

/**
 * Получает предыдущего потомка объекта графического интерфейса.
 * @param cur_child Текущий потомок объекта графического интерфейса.
 * @return Предыдущий потомок объекта графического интерфейса.
 */
extern gui_object_t* gui_object_prev_child(gui_object_t* cur_child);



#endif	/* GUI_OBJECT_H */

