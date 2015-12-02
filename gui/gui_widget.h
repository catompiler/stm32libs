/**
 * @file gui_widget.h Реализация виджета графического интерфейса.
 */

#ifndef GUI_WIDGET_H
#define	GUI_WIDGET_H

#include "gui.h"
#include "gui_object.h"
#include "gui_event.h"
#include <stdbool.h>
#include <stddef.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "graphics/rect.h"
#include "graphics/point.h"
#include "graphics/painter.h"



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
    rect_t rect; //!< Прямоугольная область окна.
    gui_border_t border; //!< Тип границы виджета.
    graphics_color_t back_color; //!< Цвет фона виджета.
    /**
     * Обработчик изменения размера.
     * @param widget Виджет.
     * @param width Ширина виджета.
     * @param height Высота виджета.
     */
    void (*on_resize)(gui_widget_t* widget, graphics_size_t width, graphics_size_t height);
    /**
     * Обработчик перерисовки.
     * @param widget Виджет.
     * @param rect Область перерисовки, может быть NULL.
     */
    void (*on_repaint)(gui_widget_t* widget, const rect_t* rect);
};

//! Приводит указатель widget к типу виджета.
#define GUI_WIDGET(widget) ((gui_widget_t*)(widget))

//! Приводит указатель на фонкцию proc к типу обработчика перерисовки виджета.
#define GUI_WIDGET_ON_REPAINT_PROC(proc) ((void (*)(gui_widget_t*, const rect_t*))proc)

//! Приводит указатель на фонкцию proc к типу обработчика изменения размера виджета.
#define GUI_WIDGET_ON_RERIZE_PROC(proc) ((void (*)(gui_widget_t*, graphics_size_t, graphics_size_t))proc)

/**
 * Обработчик события изменения размера.
 * @param widget Виджет.
 * @param event Событие изменения размера.
 */
extern void gui_widget_resize_event(gui_widget_t* widget, gui_resize_event_t* event);

/**
 * Обработчик события перерисовки.
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
 * Получает тип границы виджета.
 * @param widget Виджет.
 * @return Тип границы виджета.
 */
ALWAYS_INLINE static gui_border_t gui_widget_border(gui_widget_t* widget)
{
    return widget->border;
}

/**
 * Устанавливает тип границы виджета.
 * @param widget Виджет.
 * @param border Тип границы виджета.
 */
extern void gui_widget_set_border(gui_widget_t* widget, gui_border_t border);

/**
 * Получает цвет фона виджета.
 * @param widget Виджет.
 * @return Цвет фона виджета.
 */
ALWAYS_INLINE static graphics_color_t gui_widget_back_color(gui_widget_t* widget)
{
    return widget->back_color;
}

/**
 * Устанавливает цвет фона виджета.
 * @param widget Виджет.
 * @param back_color Цвет фона виджета.
 */
extern void gui_widget_set_back_color(gui_widget_t* widget, graphics_color_t back_color);

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
 * Получает флаг видимости виджета на экране с учётом видимости родителей.
 * @param widget Виджет.
 * @return Флаг видимости виджета.
 */
extern bool gui_widget_visible_parents(gui_widget_t* widget);

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
    return rect_x(&widget->rect);
}

/**
 * Получает координату Y виджета.
 * @param widget Виджет.
 * @return Координата Y виджета.
 */
ALWAYS_INLINE static graphics_pos_t gui_widget_y(gui_widget_t* widget)
{
    return rect_y(&widget->rect);
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
 * Получает ширину виджета.
 * @param widget Виджет.
 * @return Ширина виджета.
 */
ALWAYS_INLINE static graphics_size_t gui_widget_width(gui_widget_t* widget)
{
    return rect_width(&widget->rect);
}

/**
 * Получает высоту виджета.
 * @param widget Виджет.
 * @return Высота виджета.
 */
ALWAYS_INLINE static graphics_size_t gui_widget_height(gui_widget_t* widget)
{
    return rect_height(&widget->rect);
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
 * Получает координаты виджета.
 * @param widget Виджет.
 * @param point Координаты виджета.
 */
ALWAYS_INLINE static void gui_widget_position(gui_widget_t* widget, point_t* point)
{
    point_set_x(point, rect_x(&widget->rect));
    point_set_y(point, rect_y(&widget->rect));
}

/**
 * Получает прямоугольную область виджета.
 * @param widget Виджет.
 * @param rect Прямоугольная область.
 */
ALWAYS_INLINE static void gui_widget_rect(gui_widget_t* widget, rect_t* rect)
{
    rect_copy(rect, &widget->rect);
}

/**
 * Получает координату X виджета на экране с учётом позиции родителей.
 * @param widget Виджет.
 * @return Координату X виджета на экране.
 */
extern graphics_pos_t gui_widget_screen_x(gui_widget_t* widget);

/**
 * Получает координату Y виджета на экране с учётом позиции родителей.
 * @param widget Виджет.
 * @return Координату Y виджета на экране.
 */
extern graphics_pos_t gui_widget_screen_y(gui_widget_t* widget);

/**
 * Получает координаты виджета на экране с учётом позиции родителей.
 * @param widget Виджет.
 * @param point Координаты виджета на экране.
 */
extern void gui_widget_screen_position(gui_widget_t* widget, point_t* point);

/**
 * Получает прямоугольную область виджета на экране с учётом позиции родителей.
 * @param widget Виджет.
 * @param rect Прямоугольная область.
 */
extern void gui_widget_screen_rect(gui_widget_t* widget, rect_t* rect);

/**
 * Получает видимое положение виджета на экране с учётом позиции родителей.
 * @param widget Виджет.
 * @param point Положение виджета на экране.
 * @param rect Видимая на экране прямоугольная область виджета.
 */
extern void gui_widget_screen_visible_position(gui_widget_t* widget, point_t* point, rect_t* rect);

/**
 * Перемещает виджет.
 * @param widget Виджет.
 * @param x Координата X виджета.
 * @param y Координата Y виджета.
 */
extern void gui_widget_move(gui_widget_t* widget, graphics_pos_t x, graphics_pos_t y);

/**
 * Изменяет размер виджета.
 * @param widget Виджет.
 * @param width Ширина виджета.
 * @param height Высота виджета.
 */
extern void gui_widget_resize(gui_widget_t* widget, graphics_size_t width, graphics_size_t height);

/**
 * Перерисовывает виджет.
 * @param widget Виджет.
 * @param rect Область перерисовки.
 */
extern void gui_widget_repaint(gui_widget_t* widget, const rect_t* rect);

/**
 * Начинает перерисовку виджета и инициализирует заданный рисовальщик.
 * @param widget Виджет.
 * @param painter Рисовальщик.
 * @param rect Область перерисовки, может быть NULL.
 * @return Код ошибки.
 */
extern err_t gui_widget_begin_paint(gui_widget_t* widget, painter_t* painter, const rect_t* rect);

/**
 * Завершает перерисовку виджета.
 * @param widget Виджет.
 * @param painter Рисовальщик.
 * @return Код ошибки.
 */
extern err_t gui_widget_end_paint(gui_widget_t* widget, painter_t* painter);

/**
 * Обработчик изменения размера.
 * @param widget Виджет.
 * @param width Ширина виджета.
 * @param height Высота виджета.
 */
extern void gui_widget_on_resize(gui_widget_t* widget, graphics_size_t width, graphics_size_t height);

/**
 * Обработчик перерисовки.
 * @param widget Виджет.
 * @param rect Область перерисовки.
 */
extern void gui_widget_on_repaint(gui_widget_t* widget, const rect_t* rect);

/**
 * Получает графический интерфейс виджета.
 * @param widget Виджет.
 * @return  Графический интерфейс.
 */
ALWAYS_INLINE static gui_t* gui_widget_gui(gui_widget_t* widget)
{
    return gui_object_gui(GUI_OBJECT(widget));
}

/**
 * Получает родителя виджета.
 * @param widget Объект графического интрефейса.
 * @return Родитель виджета.
 */
ALWAYS_INLINE static gui_widget_t* gui_widget_parent(gui_widget_t* widget)
{
    return GUI_WIDGET(gui_object_parent(GUI_OBJECT(widget)));
}

/**
 * Устанавливает родителя виджета.
 * @param widget Объект графического интрефейса.
 * @param parent Родитель виджета.
 */
ALWAYS_INLINE static void gui_widget_set_parent(gui_widget_t* widget, gui_widget_t* parent)
{
    gui_object_set_parent(GUI_OBJECT(widget), GUI_OBJECT(parent));
}

/**
 * Добавляет дочерний виджет.
 * @param widget Виджет.
 * @param child Дочерний виджет.
 */
ALWAYS_INLINE static void gui_widget_add_child(gui_widget_t* widget, gui_widget_t* child)
{
    gui_object_add_child(GUI_OBJECT(widget), GUI_OBJECT(child));
}

/**
 * Удаляет дочерний виджет.
 * @param widget Виджет.
 * @param child Дочерний виджет.
 */
ALWAYS_INLINE static void gui_widget_remove_child(gui_widget_t* widget, gui_widget_t* child)
{
    gui_object_remove_child(GUI_OBJECT(widget), GUI_OBJECT(child));
}

/**
 * Получает первого потомка виджета.
 * @param widget Виджет.
 * @return Первый потомок виджета.
 */
ALWAYS_INLINE static gui_widget_t* gui_widget_first_child(gui_widget_t* widget)
{
    return GUI_WIDGET(gui_object_first_child(GUI_OBJECT(widget)));
}

/**
 * Получает следующего потомка виджета.
 * @param cur_child Текущий потомок виджета.
 * @return Следующий потомок виджета.
 */
ALWAYS_INLINE static gui_widget_t* gui_widget_next_child(gui_widget_t* cur_child)
{
    return GUI_WIDGET(gui_object_next_child(GUI_OBJECT(cur_child)));
}

/**
 * Получает последнего потомка виджета.
 * @param widget Виджет.
 * @return Последний потомок виджета.
 */
ALWAYS_INLINE static gui_widget_t* gui_widget_last_child(gui_widget_t* widget)
{
    return GUI_WIDGET(gui_object_last_child(GUI_OBJECT(widget)));
}

/**
 * Получает предыдущего потомка виджета.
 * @param cur_child Текущий потомок виджета.
 * @return Предыдущий потомок виджета.
 */
ALWAYS_INLINE static gui_widget_t* gui_widget_prev_child(gui_widget_t* cur_child)
{
    return GUI_WIDGET(gui_object_prev_child(GUI_OBJECT(cur_child)));
}

#endif	/* GUI_WIDGET_H */

