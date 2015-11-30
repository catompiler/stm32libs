/**
 * @file gui_widget.h Реализация виджета графического интерфейса.
 */

#ifndef GUI_WIDGET_H
#define	GUI_WIDGET_H

#include "gui.h"
#include "gui_object.h"
#include "gui_event.h"
#include "errors/errors.h"
#include "defs/defs.h"
#include <stdbool.h>
#include <stddef.h>


#ifndef GUI_WIDGET_TYPE_DEFINED
#define GUI_WIDGET_TYPE_DEFINED
//! Тип структуры виджета.
typedef struct _Gui_Widget gui_widget_t;
#endif //GUI_WIDGET_TYPE_DEFINED

//! Тип идентификатора виджета.
typedef int widget_id_t;

//! Структура виджета графического интерфейса.
struct _Gui_Widget {
    gui_object_t super; //!< Суперкласс.
    widget_id_t id; //!< Идентификатор виджета.
    bool visible; //!< Флаг видимости.
    graphics_pos_t x; //!< Координата x.
    graphics_pos_t y; //!< Координата y.
    graphics_size_t width; //!< Ширина.
    graphics_size_t height; //!< Высота.
    /**
     * Обработчик изменения размера.
     * @param widget Виджет.
     * @param event Событие изменения размера.
     */
    void (*resize_event)(gui_widget_t* widget, gui_resize_event_t* event);
    /**
     * Обработчик перерисовки.
     * @param widget Виджет.
     * @param event Событие перерисовки.
     */
    void (*repaint_event)(gui_widget_t* widget, gui_repaint_event_t* event);
};

//! Приводит указатель widget к типу виджета.
#define GUI_WIDGET(widget) ((gui_widget_t*)(widget))

/**
 * Обработчик изменения размера по-умолчанию.
 * @param widget Виджет.
 * @param event Событие изменения размера.
 */
extern void gui_widget_resize_event(gui_widget_t* widget, gui_resize_event_t* event);

/**
 * Обработчик перерисовки по-умолчанию.
 * @param widget Виджет.
 * @param event Событие перерисовки.
 */
extern void gui_widget_repaint_event(gui_widget_t* widget, gui_repaint_event_t* event);

/**
 * Инициализирует виджет.
 * @param widget Виджет.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
extern err_t gui_widget_init(gui_widget_t* widget, gui_t* gui);

/**
 * Инициализируют виджет как потомок заданного родителя.
 * @param widget Виджет.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
extern err_t gui_widget_init_parent(gui_widget_t* widget, gui_t* gui, gui_widget_t* parent);

/**
 * Получает идентификатор виджета.
 * @param widget Виджет.
 * @return Идентификатор виджета.
 */
ALWAYS_INLINE static widget_id_t gui_widget_id(gui_widget_t* widget)
{
    return widget->id;
}

/**
 * Устанавливает идентификатор виджета.
 * @param widget Виджет.
 * @param id Идентификатор виджета.
 */
ALWAYS_INLINE static void gui_widget_set_id(gui_widget_t* widget, widget_id_t id)
{
    widget->id = id;
}

/**
 * Получает флаг видимости виджета.
 * @param widget Виджет.
 * @return Флаг видимости виджета.
 */
ALWAYS_INLINE static bool gui_widget_visible(gui_widget_t* widget)
{
    return widget->visible;
}

/**
 * Устанавливает флаг видимости виджета.
 * @param widget Виджет.
 * @param Флаг видимости виджета.
 */
extern void gui_widget_set_visible(gui_widget_t* widget, bool visible);

/**
 * Получает координату X виджета.
 * @param widget Виджет.
 * @return Координата X виджета.
 */
ALWAYS_INLINE static graphics_pos_t gui_widget_x(gui_widget_t* widget)
{
    return widget->x;
}

/**
 * Получает координату Y виджета.
 * @param widget Виджет.
 * @return Координата Y виджета.
 */
ALWAYS_INLINE static graphics_pos_t gui_widget_y(gui_widget_t* widget)
{
    return widget->y;
}

/**
 * Устанавливает координату X виджета.
 * @param widget Виджет.
 * @param x Координата X виджета.
 */
extern void gui_widget_set_x(gui_widget_t* widget, graphics_pos_t x);

/**
 * Устанавливает координату Y виджета.
 * @param widget Виджет.
 * @param y Координата Y виджета.
 */
extern void gui_widget_set_y(gui_widget_t* widget, graphics_pos_t y);

/**
 * Перемещает виджет.
 * @param widget Виджет.
 * @param x Координата X виджета.
 * @param y Координата Y виджета.
 */
extern void gui_widget_move(gui_widget_t* widget, graphics_pos_t x, graphics_pos_t y);

/**
 * Получает ширину виджета.
 * @param widget Виджет.
 * @return Ширина виджета.
 */
ALWAYS_INLINE static graphics_size_t gui_widget_width(gui_widget_t* widget)
{
    return widget->width;
}

/**
 * Получает высоту виджета.
 * @param widget Виджет.
 * @return Высота виджета.
 */
ALWAYS_INLINE static graphics_size_t gui_widget_height(gui_widget_t* widget)
{
    return widget->height;
}

/**
 * Устанавливает ширину виджета.
 * @param widget Виджет.
 * @param width Ширина виджета.
 */
extern void gui_widget_set_width(gui_widget_t* widget, graphics_size_t width);

/**
 * Устанавливает высоту виджета.
 * @param widget Виджет.
 * @param height Высота виджета.
 */
extern void gui_widget_set_height(gui_widget_t* widget, graphics_size_t height);

/**
 * Изменяет размер виджета.
 * @param widget Виджет.
 * @param width Ширина виджета.
 * @param height Высота виджета.
 */
extern void gui_widget_resize(gui_widget_t* widget, graphics_size_t width, graphics_size_t height);

#endif	/* GUI_WIDGET_H */

