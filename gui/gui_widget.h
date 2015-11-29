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


#ifndef GUI_WIDGET_TYPE_DEFINED
#define GUI_WIDGET_TYPE_DEFINED
//! Тип структуры виджета.
typedef struct _Gui_Widget gui_widget_t;
#endif //GUI_WIDGET_TYPE_DEFINED

//! Структура виджета графического интерфейса.
struct _Gui_Widget {
    gui_object_t super; //!< Суперкласс.
    int id; //!< Идентификатор виджета.
    bool visible; //!< Флаг видимости.
    int x; //!< Координата x.
    int y; //!< Координата y.
    size_t width; //!< Ширина.
    size_t height; //!< Высота.
    /**
     * Обработчик изменения размера.
     * @param widget Виджет.
     * @param event Событие изменения размера.
     */
    void (*resize)(gui_widget_t* widget, gui_resize_event_t* event);
    /**
     * Обработчик перерисовки.
     * @param widget Виджет.
     * @param event Событие перерисовки.
     */
    void (*repaint)(gui_widget_t* widget, gui_repaint_event_t* event);
};



#endif	/* GUI_WIDGET_H */

