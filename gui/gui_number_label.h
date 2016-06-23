/**
 * @file gui_number_label.h Реализация виджета числовой метки графического интерфейса.
 */

#ifndef GUI_NUMBER_LABEL_H
#define	GUI_NUMBER_LABEL_H

#include "gui.h"
#include "gui_widget.h"
#include "errors/errors.h"
#include "defs/defs.h"


typedef enum _Gui_Number_Label_Format {
    GUI_NUMBER_LABEL_DEC = 0, //!< Десятичный формат числа.
    GUI_NUMBER_LABEL_HEX, //!< Шестнадцатиричный формат числа.
    GUI_NUMBER_LABEL_FIX, //!< Формат числа с фиксированной запятой (fixed32_t).
} gui_number_label_format_t;

typedef struct _Gui_Number_Label gui_number_label_t;

//! Зеначение идентификатора типа числовой метки.
#define GUI_NUMBER_LABEL_TYPE_ID 2

struct _Gui_Number_Label {
    gui_widget_t super; //!< Суперкласс.
    int number; //!< Отображаемое число.
    gui_number_label_format_t format; //!< Формат числа.
    size_t decimals; //!< Число знаков дробной части для формата с фиксированной запятой.
};

//! Приводит указатель label к типу числовой метки.
#define GUI_NUMBER_LABEL(label) ((gui_number_label_t*)(label))

//! Максимальное число знаков после запятой.
#define GUI_NUMBER_LABEL_DECIMALS_MAX 6

/**
 * Инициализирует метку.
 * @param label Метка.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
EXTERN err_t gui_number_label_init(gui_number_label_t* label, gui_t* gui);

/**
 * Инициализирует метку как потомок заданного родителя.
 * @param label Метка.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
EXTERN err_t gui_number_label_init_parent(gui_number_label_t* label, gui_t* gui, gui_widget_t* parent);

/**
 * Получает число метки.
 * @param label Метка.
 * @return Число метки.
 */
ALWAYS_INLINE static int gui_number_label_number(gui_number_label_t* label)
{
    return label->number;
}

/**
 * Устанавливает число метки.
 * @param label Метка.
 * @param number Число метки.
 */
EXTERN void gui_number_label_set_number(gui_number_label_t* label, int number);

/**
 * Получает формат числа метки.
 * @param label Метка.
 * @return Формат числа метки.
 */
ALWAYS_INLINE static gui_number_label_format_t gui_number_label_format(gui_number_label_t* label)
{
    return label->format;
}

/**
 * Устанавливает формат числа метки.
 * @param label Метка.
 * @param format Формат числа метки.
 */
EXTERN void gui_number_label_set_format(gui_number_label_t* label, gui_number_label_format_t format);

/**
 * Получает число знаков дробной части метки.
 * @param label Метка.
 * @return Число знаков дробной части метки.
 */
ALWAYS_INLINE static int gui_number_label_decimals(gui_number_label_t* label)
{
    return label->decimals;
}

/**
 * Устанавливает число знаков дробной части метки.
 * @param label Метка.
 * @param decimal Число знаков дробной части метки.
 */
EXTERN void gui_number_label_set_decimals(gui_number_label_t* label, int decimals);

/**
 * Обработчик перерисовки.
 * @param label Метка.
 * @param rect Область перерисовки.
 */
EXTERN void gui_number_label_on_repaint(gui_number_label_t* label, const rect_t* rect);

#endif	/* GUI_NUMBER_LABEL_H */
