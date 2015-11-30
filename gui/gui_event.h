/**
 * @file gui_event.h Реализация события графического интерфейса.
 */

#ifndef GUI_EVENT_H
#define	GUI_EVENT_H

#include "gui.h"
#include <stddef.h>
#include <stdbool.h>

typedef enum _Gui_Event_Type {
    GUI_EVENT_TYPE_NONE = 0, //!< Отсутствующее событие.
    GUI_EVENT_TYPE_RESIZE = 1, //!< Событие изменение размера.
    GUI_EVENT_TYPE_REPAINT = 2 //!< Событие перерисовки.
} gui_event_type_t;

//! Структура события графического интерфейса.
typedef struct _Gui_Event {
    gui_event_type_t type; //!< Тип события.
    bool accepted; //!< Принято ли событие.
} gui_event_t;

//! Структура события изменения размера.
typedef struct _Gui_Resize_Event {
    gui_event_t super; //!< Суперкласс.
    graphics_size_t width; //!< Новая ширина.
    graphics_size_t height; //!< Новая высота.
} gui_resize_event_t;

//! Структура события перерисовки.
typedef struct _Gui_Repaint_Event {
    gui_event_t super; //!< Суперкласс.
    graphics_pos_t x; //!< Координата X области перерисовки.
    graphics_pos_t y; //!< Координата Y области перерисовки.
    graphics_size_t width; //!< Ширина области перерисовки.
    graphics_size_t height; //!< Высота области перерисовки.
} gui_repaint_event_t;

//! Приводит указатель event к типу события.
#define GUI_EVENT(event) ((gui_event_t*)(event))

/**
 * Инициализирует событие.
 * @param event Событие.
 */
ALWAYS_INLINE static void gui_event_init(gui_event_t* event, gui_event_type_t type)
{
    event->type = type;
    event->accepted = false;
}

/**
 * Получает флаг принятия события.
 * @param event Событие.
 * @return Флаг принятия события.
 */
ALWAYS_INLINE static bool gui_event_accepted(gui_event_t* event)
{
    return event->accepted;
}

/**
 * Принимает событие.
 * @param event Событие.
 */
ALWAYS_INLINE static void gui_event_accept(gui_event_t* event)
{
    event->accepted = true;
}

/**
 * Игнорирует событие.
 * @param event Событие.
 */
ALWAYS_INLINE static void gui_event_ignore(gui_event_t* event)
{
    event->accepted = false;
}

//! Приводит указатель event к типу события изменения размера.
#define GUI_RESIZE_EVENT(event) ((gui_resize_event_t*)(event))

/**
 * Инициализирует событие изменения размера.
 * @param event Событие изменения размера.
 */
ALWAYS_INLINE static void gui_resize_event_init(gui_resize_event_t* event, graphics_size_t width, graphics_size_t height)
{
    gui_event_init(GUI_EVENT(event), GUI_EVENT_TYPE_RESIZE);
    event->width = width;
    event->height = height;
}

//! Приводит указатель event к типу события перерисовки..
#define GUI_REPAINT_EVENT(event) ((gui_repaint_event_t*)(event))

/**
 * Инициализирует событие перерисовки.
 * @param event Событие перерисовки.
 */
ALWAYS_INLINE static void gui_repaint_event_init(gui_repaint_event_t* event, graphics_pos_t x, graphics_pos_t y, graphics_size_t width, graphics_size_t height)
{
    gui_event_init(GUI_EVENT(event), GUI_EVENT_TYPE_REPAINT);
    event->x = x;
    event->y = y;
    event->width = width;
    event->height = height;
}

#endif	/* GUI_EVENT_H */

