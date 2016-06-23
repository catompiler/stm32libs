/**
 * @file gui_checkbox.h Реализация виджета флажка графического интерфейса.
 */

#ifndef GUI_CHECKBOX_H
#define	GUI_CHECKBOX_H

#include "gui.h"
#include "gui_widget.h"
#include "errors/errors.h"
#include "defs/defs.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct _Gui_Checkbox gui_checkbox_t;

//! Зеначение идентификатора типа флажка.
#define GUI_CHECKBOX_TYPE_ID 3

struct _Gui_Checkbox {
    gui_widget_t super; //!< Суперкласс.
    const char* text; //!< Текст флажка.
    bool checked; //!< Флаг.
    size_t check_size; //!< Размер флажка.
    void (*on_toggled)(gui_checkbox_t* checkbox, bool checked); //!< Каллбэк при изменении флага.
};

typedef void (*gui_checkbox_on_toggled_t)(gui_checkbox_t*, bool);

//! Приводит указатель checkbox к типу флажка.
#define GUI_CHECKBOX(checkbox) ((gui_checkbox_t*)(checkbox))

// Размер флажка по-умолчанию,
#define GUI_CHECKBOX_DEFAULT_CHECK_SIZE 12

/**
 * Инициализирует флажок.
 * @param checkbox Флажок.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
EXTERN err_t gui_checkbox_init(gui_checkbox_t* checkbox, gui_t* gui);

/**
 * Инициализирует флажок как потомок заданного родителя.
 * @param checkbox Флажок.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
EXTERN err_t gui_checkbox_init_parent(gui_checkbox_t* checkbox, gui_t* gui, gui_widget_t* parent);

/**
 * Получает текст флажка.
 * @param checkbox Флажок.
 * @return Текст флажка.
 */
ALWAYS_INLINE static const char* gui_checkbox_text(gui_checkbox_t* checkbox)
{
    return checkbox->text;
}

/**
 * Устанавливает текст флажка.
 * @param checkbox Флажок.
 * @param text Текст флажка.
 */
EXTERN void gui_checkbox_set_text(gui_checkbox_t* checkbox, const char* text);

/**
 * Получает установленность флажка.
 * @param checkbox Флажок.
 * @return Установленность флажка.
 */
ALWAYS_INLINE static bool gui_checkbox_checked(gui_checkbox_t* checkbox)
{
    return checkbox->checked;
}

/**
 * Устанавливает установленность флажка.
 * @param checkbox Флажок.
 * @param checked Установленность флажка.
 */
EXTERN void gui_checkbox_set_checked(gui_checkbox_t* checkbox, bool checked);

/**
 * Получает каллбэк изменения флага.
 * @param checkbox Флажок.
 * @return Каллбэк изменения флага.
 */
ALWAYS_INLINE static gui_checkbox_on_toggled_t gui_checkbox_on_toggled(gui_checkbox_t* checkbox)
{
    return checkbox->on_toggled;
}

/**
 * Устанавливает каллбэк изменения флага.
 * @param checkbox Флажок.
 * @param on_toggled Каллбэк изменения флага.
 */
ALWAYS_INLINE static void gui_checkbox_set_on_toggled(gui_checkbox_t* checkbox, gui_checkbox_on_toggled_t on_toggled)
{
    checkbox->on_toggled = on_toggled;
}

/**
 * Получает размер флажка.
 * @param checkbox Флажок.
 * @return Размер флажка.
 */
ALWAYS_INLINE static size_t gui_checkbox_check_size(gui_checkbox_t* checkbox)
{
    return checkbox->check_size;
}

/**
 * Устанавливает размер флажка.
 * @param checkbox Флажок.
 * @param size Размер флажка.
 */
EXTERN void gui_checkbox_set_check_size(gui_checkbox_t* checkbox, size_t size);

/**
 * Обработчик перерисовки.
 * @param checkbox Флажок.
 * @param rect Область перерисовки.
 */
EXTERN void gui_checkbox_on_repaint(gui_checkbox_t* checkbox, const rect_t* rect);

/**
 * Обработчик нажатия клавиши.
 * @param checkbox Флажок.
 * @param key Код клавиши.
 */
EXTERN void gui_checkbox_on_key_press(gui_checkbox_t* checkbox, keycode_t key);

#endif	/* GUI_CHECKBOX_H */

