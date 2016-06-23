/**
 * @file gui_label.h Реализация виджета метки графического интерфейса.
 */

#ifndef GUI_LABEL_H
#define	GUI_LABEL_H

#include "gui.h"
#include "gui_widget.h"
#include "errors/errors.h"
#include "defs/defs.h"

typedef struct _Gui_Label gui_label_t;

//! Зеначение идентификатора типа метки.
#define GUI_LABEL_TYPE_ID 1

struct _Gui_Label {
    gui_widget_t super; //!< Суперкласс.
    const char* text; //!< Отображаемый текст.
};

//! Приводит указатель label к типу метки.
#define GUI_LABEL(label) ((gui_label_t*)(label))

/**
 * Инициализирует метку.
 * @param label Метка.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
EXTERN err_t gui_label_init(gui_label_t* label, gui_t* gui);

/**
 * Инициализирует метку как потомок заданного родителя.
 * @param label Метка.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
EXTERN err_t gui_label_init_parent(gui_label_t* label, gui_t* gui, gui_widget_t* parent);

/**
 * Получает текст метки.
 * @param label Метка.
 * @return Текст метки.
 */
ALWAYS_INLINE static const char* gui_label_text(gui_label_t* label)
{
    return label->text;
}

/**
 * Устанавливает текст метки.
 * @param label Метка.
 * @param text Текст метки.
 */
EXTERN void gui_label_set_text(gui_label_t* label, const char* text);

/**
 * Обработчик перерисовки.
 * @param label Метка.
 * @param rect Область перерисовки.
 */
EXTERN void gui_label_on_repaint(gui_label_t* label, const rect_t* rect);

#endif	/* GUI_LABEL_H */
