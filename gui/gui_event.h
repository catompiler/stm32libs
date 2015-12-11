/**
 * @file gui_event.h Реализация события графического интерфейса.
 */

#ifndef GUI_EVENT_H
#define	GUI_EVENT_H

#include "gui.h"
#include <stddef.h>
#include <stdbool.h>
#include "graphics/rect.h"
#include "input/key_input.h"

typedef enum _Gui_Event_Type {
    GUI_EVENT_TYPE_NONE = 0, //!< Отсутствующее событие.
    GUI_EVENT_TYPE_RESIZE = 1, //!< Событие изменение размера.
    GUI_EVENT_TYPE_REPAINT = 2, //!< Событие перерисовки.
    GUI_EVENT_TYPE_KEY_PRESS = 3, //!< Событие нажатия клавиши.
    GUI_EVENT_TYPE_KEY_RELEASE = 4 //!< Событие отпускания клавиши.
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
    rect_t rect; //!< Область перерисовки.
} gui_repaint_event_t;

//! Структура события клавиатуры.
typedef struct _Gui_Key_Event {
    gui_event_t super; //!< Суперкласс.
    key_t key; //!< Код клавиши.
} gui_key_event_t;

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
 * @param width Новая ширина.
 * @param height Новая высота.
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
 * @param x Координата X области перерисовки.
 * @param y Координата Y области перерисовки.
 * @param width Ширина области перерисовки.
 * @param height Высота области перерисовки.
 */
ALWAYS_INLINE static void gui_repaint_event_init(gui_repaint_event_t* event, graphics_pos_t x, graphics_pos_t y, graphics_size_t width, graphics_size_t height)
{
    gui_event_init(GUI_EVENT(event), GUI_EVENT_TYPE_REPAINT);
    rect_set_x(&event->rect, x);
    rect_set_y(&event->rect, y);
    rect_set_width(&event->rect, width);
    rect_set_height(&event->rect, height);
}

/**
 * Инициализирует событие перерисовки.
 * @param event Событие перерисовки.
 * @param rect Прямоугольная область перерисовки.
 */
ALWAYS_INLINE static void gui_repaint_event_init_rect(gui_repaint_event_t* event, const rect_t* rect)
{
    gui_event_init(GUI_EVENT(event), GUI_EVENT_TYPE_REPAINT);
    rect_set_left(&event->rect, rect_left(rect));
    rect_set_top(&event->rect, rect_top(rect));
    rect_set_right(&event->rect, rect_right(rect));
    rect_set_bottom(&event->rect, rect_bottom(rect));
}

ALWAYS_INLINE static void gui_key_press_event_init(gui_key_event_t* event, key_t key)
{
    gui_event_init(GUI_EVENT(event), GUI_EVENT_TYPE_KEY_PRESS);
    event->key = key;
}

ALWAYS_INLINE static void gui_key_release_event_init(gui_key_event_t* event, key_t key)
{
    gui_event_init(GUI_EVENT(event), GUI_EVENT_TYPE_KEY_RELEASE);
    event->key = key;
}

#endif	/* GUI_EVENT_H */
