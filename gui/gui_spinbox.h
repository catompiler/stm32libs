/**
 * @file gui_spinbox.h Реализация виджета спинбокса графического интерфейса.
 */

#ifndef GUI_SPINBOX_H
#define	GUI_SPINBOX_H

#include "gui.h"
#include "gui_widget.h"
#include "gui_number_label.h"
#include "errors/errors.h"
#include "defs/defs.h"
#include <stddef.h>


typedef struct _Gui_Spinbox gui_spinbox_t;

//! Зеначение идентификатора типа числовой спинбокса.
#define GUI_SPINBOX_TYPE_ID 6

struct _Gui_Spinbox {
    gui_widget_t super; //!< Суперкласс.
    int value; //!< Значение.
    int increment; //!< Значение инкремента.
    int maximum; //!< Максимальное значение.
    int minimum; //!< Минимальное значение.
    gui_number_label_format_t format; //!< Формат числа.
    size_t spin_size; //!< Размер кнопок инкремента / декремента.
    bool incrementing; //!< Флаг увеличения значения.
    bool decrementing; //!< Флаг уменьшения значения.
    void (*on_value_changed)(gui_spinbox_t* spinbox, int value); //!< Каллбэк при изменении значения.
};

typedef void (*gui_spinbox_on_value_changed_t)(gui_spinbox_t*, int);

//! Приводит указатель spinbox к типу числовой спинбокса.
#define GUI_SPINBOX(spinbox) ((gui_spinbox_t*)(spinbox))

//! Размер кнопок инкремента / декремента по-умолчанию.
#define GUI_SPINBOX_DEFAULT_SPIN_SIZE 11

/**
 * Инициализирует спинбокс.
 * @param spinbox Спинбокс.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
EXTERN err_t gui_spinbox_init(gui_spinbox_t* spinbox, gui_t* gui);

/**
 * Инициализирует спинбокс как потомок заданного родителя.
 * @param spinbox Спинбокс.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
EXTERN err_t gui_spinbox_init_parent(gui_spinbox_t* spinbox, gui_t* gui, gui_widget_t* parent);

/**
 * Получает значение спинбокса.
 * @param spinbox Спинбокс.
 * @return Значение спинбокса.
 */
ALWAYS_INLINE static int gui_spinbox_value(gui_spinbox_t* spinbox)
{
    return spinbox->value;
}

/**
 * Устанавливает значение спинбокса.
 * @param spinbox Спинбокс.
 * @param value Значение спинбокса.
 */
EXTERN void gui_spinbox_set_value(gui_spinbox_t* spinbox, int value);

/**
 * Увеличивает значение на значение инкремента.
 * @param spinbox Спинбокс.
 */
EXTERN void gui_spinbox_inc_value(gui_spinbox_t* spinbox);

/**
 * Уменьшает значение на значение инкремента.
 * @param spinbox Спинбокс.
 */
EXTERN void gui_spinbox_dec_value(gui_spinbox_t* spinbox);

/**
 * Получает значение инкремента спинбокса.
 * @param spinbox Спинбокс.
 * @return Значение инкремента спинбокса.
 */
ALWAYS_INLINE static int gui_spinbox_increment(gui_spinbox_t* spinbox)
{
    return spinbox->increment;
}

/**
 * Устанавливает значение инкремента спинбокса.
 * @param spinbox Спинбокс.
 * @param increment Значение инкремента спинбокса.
 */
ALWAYS_INLINE static void gui_spinbox_set_increment(gui_spinbox_t* spinbox, int increment)
{
    spinbox->increment = increment;
}

/**
 * Получает максимальное значение спинбокса.
 * @param spinbox Спинбокс.
 * @return Максимальное значение спинбокса.
 */
ALWAYS_INLINE static int gui_spinbox_maximum(gui_spinbox_t* spinbox)
{
    return spinbox->maximum;
}

/**
 * Устанавливает максимальное значение спинбокса.
 * @param spinbox Спинбокс.
 * @param maximum Максимальное значение спинбокса.
 */
EXTERN void gui_spinbox_set_maximum(gui_spinbox_t* spinbox, int maximum);

/**
 * Получает минимальное значение спинбокса.
 * @param spinbox Спинбокс.
 * @return Минимальное значение спинбокса.
 */
ALWAYS_INLINE static int gui_spinbox_minimum(gui_spinbox_t* spinbox)
{
    return spinbox->minimum;
}

/**
 * Устанавливает минимальное значение спинбокса.
 * @param spinbox Спинбокс.
 * @param minimum Минимальное значение спинбокса.
 */
EXTERN void gui_spinbox_set_minimum(gui_spinbox_t* spinbox, int minimum);

/**
 * Устанавливает минимальное и минимальное значение спинбокса.
 * @param spinbox Спинбокс.
 * @param minimum Минимальное значение.
 * @param maximum Максимальное значение.
 */
EXTERN void gui_spinbox_set_range(gui_spinbox_t* spinbox, int minimum, int maximum);

/**
 * Получает формат числа спинбокса.
 * @param spinbox Спинбокс.
 * @return Формат числа спинбокса.
 */
ALWAYS_INLINE static gui_number_label_format_t gui_spinbox_format(gui_spinbox_t* spinbox)
{
    return spinbox->format;
}

/**
 * Устанавливает формат числа спинбокса.
 * @param spinbox Спинбокс.
 * @param format Формат числа спинбокса.
 */
EXTERN void gui_spinbox_set_format(gui_spinbox_t* spinbox, gui_number_label_format_t format);

/**
 * Получает каллбэк изменения значения.
 * @param spinbox Спинбокс.
 * @return Каллбэк изменения значения.
 */
ALWAYS_INLINE static gui_spinbox_on_value_changed_t gui_spinbox_on_value_changed(gui_spinbox_t* spinbox)
{
    return spinbox->on_value_changed;
}

/**
 * Устанавливает каллбэк изменения значения.
 * @param spinbox Спинбокс.
 * @param on_value_changed Каллбэк изменения значения.
 */
ALWAYS_INLINE static void gui_spinbox_set_on_value_changed(gui_spinbox_t* spinbox, gui_spinbox_on_value_changed_t on_value_changed)
{
    spinbox->on_value_changed = on_value_changed;
}

/**
 * Получает размер кнопок инкремента / декремента.
 * @param spinbox Спинбокс.
 * @return Размер кнопок инкремента / декремента.
 */
ALWAYS_INLINE static size_t gui_spinbox_spin_size(gui_spinbox_t* spinbox)
{
    return spinbox->spin_size;
}

/**
 * Устанавливает размер кнопок инкремента / декремента.
 * @param spinbox Спинбокс.
 * @param size Размер кнопок инкремента / декремента.
 */
EXTERN void gui_spinbox_set_spin_size(gui_spinbox_t* spinbox, size_t size);

/**
 * Обработчик перерисовки.
 * @param spinbox Спинбокс.
 * @param rect Область перерисовки.
 */
EXTERN void gui_spinbox_on_repaint(gui_spinbox_t* spinbox, const rect_t* rect);

/**
 * Обработчик нажатия клавиши.
 * @param spinbox Спинбокс.
 * @param key Код клавиши.
 */
EXTERN void gui_spinbox_on_key_press(gui_spinbox_t* spinbox, keycode_t key);

/**
 * Обработчик отпускания клавиши.
 * @param spinbox Спинбокс.
 * @param key Код клавиши.
 */
EXTERN void gui_spinbox_on_key_release(gui_spinbox_t* spinbox, keycode_t key);

#endif	/* GUI_SPINBOX_H */
