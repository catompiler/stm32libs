/**
 * @file gui_radiobutton.h Реализация виджета переключателя графического интерфейса.
 */

#ifndef GUI_RADIOBUTTON_H
#define	GUI_RADIOBUTTON_H

#include "gui.h"
#include "gui_widget.h"
#include "errors/errors.h"
#include "defs/defs.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct _Gui_Radiobutton gui_radiobutton_t;

//! Зеначение идентификатора типа переключателя.
#define GUI_RADIOBUTTON_TYPE_ID 5

struct _Gui_Radiobutton {
    gui_widget_t super; //!< Суперкласс.
    const char* text; //!< Текст переключателя.
    bool checked; //!< Флаг.
    size_t radio_size; //!< Размер переключателя.
    void (*on_toggled)(gui_radiobutton_t* radiobutton, bool checked); //!< Каллбэк при изменении флага.
};

typedef void (*gui_radiobutton_on_toggled_t)(gui_radiobutton_t*, bool);

//! Приводит указатель radiobutton к типу переключателя.
#define GUI_RADIOBUTTON(radiobutton) ((gui_radiobutton_t*)(radiobutton))

//! Размер переключателя по-умолчанию.
#define GUI_RADIOBUTTON_DEFAULT_RADIO_SIZE 12

/**
 * Инициализирует переключатель.
 * @param radiobutton Переключатель.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
extern err_t gui_radiobutton_init(gui_radiobutton_t* radiobutton, gui_t* gui);

/**
 * Инициализирует переключатель как потомок заданного родителя.
 * @param radiobutton Переключатель.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
extern err_t gui_radiobutton_init_parent(gui_radiobutton_t* radiobutton, gui_t* gui, gui_widget_t* parent);

/**
 * Получает текст переключателя.
 * @param radiobutton Переключатель.
 * @return Текст переключателя.
 */
ALWAYS_INLINE static const char* gui_radiobutton_text(gui_radiobutton_t* radiobutton)
{
    return radiobutton->text;
}

/**
 * Устанавливает текст переключателя.
 * @param radiobutton Переключатель.
 * @param text Текст переключателя.
 */
extern void gui_radiobutton_set_text(gui_radiobutton_t* radiobutton, const char* text);

/**
 * Получает установленность переключателя.
 * @param radiobutton Переключатель.
 * @return Установленность переключателя.
 */
ALWAYS_INLINE static bool gui_radiobutton_checked(gui_radiobutton_t* radiobutton)
{
    return radiobutton->checked;
}

/**
 * Устанавливает установленность переключателя.
 * @param radiobutton Переключатель.
 * @param checked Установленность переключателя.
 */
extern void gui_radiobutton_set_checked(gui_radiobutton_t* radiobutton, bool checked);

/**
 * Получает каллбэк изменения флага.
 * @param radiobutton Переключатель.
 * @return Каллбэк изменения флага.
 */
ALWAYS_INLINE static gui_radiobutton_on_toggled_t gui_radiobutton_on_toggled(gui_radiobutton_t* radiobutton)
{
    return radiobutton->on_toggled;
}

/**
 * Устанавливает каллбэк изменения флага.
 * @param radiobutton Переключатель.
 * @param on_toggled Каллбэк изменения флага.
 */
ALWAYS_INLINE static void gui_radiobutton_set_on_toggled(gui_radiobutton_t* radiobutton, gui_radiobutton_on_toggled_t on_toggled)
{
    radiobutton->on_toggled = on_toggled;
}

/**
 * Получает размер переключателя.
 * @param radiobutton Переключатель.
 * @return Размер переключателя.
 */
ALWAYS_INLINE static size_t gui_radiobutton_radio_size(gui_radiobutton_t* radiobutton)
{
    return radiobutton->radio_size;
}

/**
 * Устанавливает размер переключателя.
 * @param radiobutton Переключатель.
 * @param size Размер переключателя.
 */
extern void gui_radiobutton_set_radio_size(gui_radiobutton_t* radiobutton, size_t size);

/**
 * Обработчик перерисовки.
 * @param radiobutton Переключатель.
 * @param rect Область перерисовки.
 */
extern void gui_radiobutton_on_repaint(gui_radiobutton_t* radiobutton, const rect_t* rect);

/**
 * Обработчик нажатия клавиши.
 * @param radiobutton Переключатель.
 * @param key Код клавиши.
 */
extern void gui_radiobutton_on_key_press(gui_radiobutton_t* radiobutton, keycode_t key);

#endif	/* GUI_RADIOBUTTON_H */

