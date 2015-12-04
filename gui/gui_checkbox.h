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

typedef struct _Gui_Checkbox gui_checkbox_t;

struct _Gui_Checkbox {
    gui_widget_t super; //!< Суперкласс.
    const char* text; //!< Текст флажка.
    bool checked; //!< Флаг.
};

//! Приводит указатель checkbox к типу флажка.
#define GUI_CHECKBOX(checkbox) ((gui_checkbox_t*)(checkbox))

/**
 * Инициализирует флажок.
 * @param checkbox Флажок.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
extern err_t gui_checkbox_init(gui_checkbox_t* checkbox, gui_t* gui);

/**
 * Инициализирует флажок как потомок заданного родителя.
 * @param checkbox Флажок.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
extern err_t gui_checkbox_init_parent(gui_checkbox_t* checkbox, gui_t* gui, gui_widget_t* parent);

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
extern void gui_checkbox_set_text(gui_checkbox_t* checkbox, const char* text);

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
extern void gui_checkbox_set_checked(gui_checkbox_t* checkbox, bool checked);

/**
 * Обработчик перерисовки.
 * @param checkbox Флажок.
 * @param rect Область перерисовки.
 */
extern void gui_checkbox_on_repaint(gui_checkbox_t* checkbox, const rect_t* rect);

#endif	/* GUI_CHECKBOX_H */

