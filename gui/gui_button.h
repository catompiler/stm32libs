/**
 * @file gui_button.h Реализация виджета кнопки графического интерфейса.
 */

#ifndef GUI_BUTTON_H
#define	GUI_BUTTON_H

#include "gui.h"
#include "gui_widget.h"
#include "errors/errors.h"
#include "defs/defs.h"
#include <stdbool.h>

typedef struct _Gui_Button gui_button_t;

//! Зеначение идентификатора типа кнопки.
#define GUI_BUTTON_TYPE_ID 4

struct _Gui_Button {
    gui_widget_t super; //!< Суперкласс.
    const char* text; //!< Текст кнопки.
    bool pressing; //!< Флаг нажимания на кнопку.
    void (*on_clicked)(gui_button_t* button); //!< Каллбэк при изменении флага.
};

typedef void (*gui_button_on_clicked_t)(gui_button_t*);

//! Приводит указатель button к типу кнопки.
#define GUI_BUTTON(button) ((gui_button_t*)(button))

/**
 * Инициализирует кнопку.
 * @param button Кнопка.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
extern err_t gui_button_init(gui_button_t* button, gui_t* gui);

/**
 * Инициализирует кнопку как потомок заданного родителя.
 * @param button Кнопка.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
extern err_t gui_button_init_parent(gui_button_t* button, gui_t* gui, gui_widget_t* parent);

/**
 * Получает текст кнопки.
 * @param button Кнопка.
 * @return Текст кнопки.
 */
ALWAYS_INLINE static const char* gui_button_text(gui_button_t* button)
{
    return button->text;
}

/**
 * Устанавливает текст кнопки.
 * @param button Кнопка.
 * @param text Текст кнопки.
 */
extern void gui_button_set_text(gui_button_t* button, const char* text);

/**
 * Получает каллбэк нажатия кнопки.
 * @param button Кнопка.
 * @return Каллбэк нажатия кнопки.
 */
ALWAYS_INLINE static gui_button_on_clicked_t gui_button_on_clicked(gui_button_t* button)
{
    return button->on_clicked;
}

/**
 * Устанавливает каллбэк нажатия кнопки.
 * @param button Кнопка.
 * @param on_clicked Каллбэк нажатия кнопки.
 */
ALWAYS_INLINE static void gui_button_set_on_clicked(gui_button_t* button, gui_button_on_clicked_t on_clicked)
{
    button->on_clicked = on_clicked;
}

/**
 * Обработчик перерисовки.
 * @param button Кнопка.
 * @param rect Область перерисовки.
 */
extern void gui_button_on_repaint(gui_button_t* button, const rect_t* rect);

/**
 * Обработчик нажатия клавиши.
 * @param button Кнопка.
 * @param key Код клавиши.
 */
extern void gui_button_on_key_press(gui_button_t* button, key_t key);

/**
 * Обработчик отпускания клавиши.
 * @param button Кнопка.
 * @param key Код клавиши.
 */
extern void gui_button_on_key_release(gui_button_t* button, key_t key);

#endif	/* GUI_BUTTON_H */

