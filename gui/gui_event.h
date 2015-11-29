/**
 * @file gui_event.h Реализация события графического интерфейса.
 */

#ifndef GUI_EVENT_H
#define	GUI_EVENT_H

#include "gui.h"
#include <stddef.h>

typedef enum _Gui_Event_Type {
    GUI_EVENT_TYPE_NONE = 0, //!< Отсутствующее событие.
    GUI_EVENT_TYPE_RESIZE = 1, //!< Событие изменение размера.
    GUI_EVENT_TYPE_REPAINT = 2 //!< Событие перерисовки.
} gui_event_type_t;

//! Структура события графического интерфейса.
typedef struct _Gui_Event {
    gui_event_type_t type; //!< Тип события.
} gui_event_t;

//! Структура события изменения размера.
typedef struct _Gui_Resize_Event {
    gui_event_t super; //!< Суперкласс.
    size_t width; //!< Новая ширина.
    size_t height; //!< Новая высота.
} gui_resize_event_t;

//! Структура события перерисовки.
typedef struct _Gui_Repaint_Event {
    gui_event_t super; //!< Суперкласс.
    int x; //!< Координата X области перерисовки.
    int y; //!< Координата Y области перерисовки.
    size_t width; //!< Ширина области перерисовки.
    size_t height; //!< Высота области перерисовки.
} gui_repaint_event_t;


#define GUI_EVENT(event) ((gui_event_t*)(event))
/**
 * Инициализирует событие.
 * @param event Событие.
 */
ALWAYS_INLINE static void gui_event_init(gui_event_t* event, gui_event_type_t type)
{
    event->type = type;
}

#define GUI_RESIZE_EVENT(event) ((gui_resize_event_t*)(event))
/**
 * Инициализирует событие изменения размера.
 * @param event Событие изменения размера.
 */
ALWAYS_INLINE static void gui_resize_event_init(gui_resize_event_t* event, size_t width, size_t height)
{
    gui_event_init(GUI_EVENT(event), GUI_EVENT_TYPE_RESIZE);
    event->width = width;
    event->height = height;
}

#define GUI_REPAINT_EVENT(event) ((gui_repaint_event_t*)(event))
/**
 * Инициализирует событие перерисовки.
 * @param event Событие перерисовки.
 */
ALWAYS_INLINE static void gui_repaint_event_init(gui_repaint_event_t* event, int x, int y, size_t width, size_t height)
{
    gui_event_init(GUI_EVENT(event), GUI_EVENT_TYPE_REPAINT);
    event->x = x;
    event->y = y;
    event->width = width;
    event->height = height;
}

#endif	/* GUI_EVENT_H */

