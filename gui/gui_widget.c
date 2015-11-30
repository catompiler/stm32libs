#include "gui_widget.h"
#include "utils/utils.h"


static widget_id_t next_widget_id = 1;


void gui_widget_resize_event(gui_widget_t* widget, gui_resize_event_t* event)
{
    gui_widget_resize(widget, event->width, event->height);
}

void gui_widget_repaint_event(gui_widget_t* widget, gui_repaint_event_t* event)
{
}

err_t gui_widget_init(gui_widget_t* widget, gui_t* gui)
{
    RETURN_ERR_IF_FAIL(gui_object_init(GUI_OBJECT(widget), gui));
    
    widget->id = next_widget_id ++;
    widget->visible = true;
    widget->x = 0;
    widget->y = 0;
    widget->width = 0;
    widget->height = 0;
    widget->resize_event = gui_widget_resize_event;
    widget->repaint_event = gui_widget_repaint_event;
    
    return E_NO_ERROR;
}

err_t gui_widget_init_parent(gui_widget_t* widget, gui_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_object_init_parent(GUI_OBJECT(widget), gui, GUI_OBJECT(parent)));
    
    widget->id = next_widget_id ++;
    widget->visible = true;
    widget->x = 0;
    widget->y = 0;
    widget->width = 0;
    widget->height = 0;
    widget->resize_event = gui_widget_resize_event;
    widget->repaint_event = gui_widget_repaint_event;
    
    return E_NO_ERROR;
}

void gui_widget_set_visible(gui_widget_t* widget, bool visible)
{
    widget->visible = visible;
}

void gui_widget_set_x(gui_widget_t* widget, graphics_pos_t x)
{
    widget->x = x;
}

void gui_widget_set_y(gui_widget_t* widget, graphics_pos_t y)
{
    widget->y = y;
}

void gui_widget_move(gui_widget_t* widget, graphics_pos_t x, graphics_pos_t y)
{
    widget->x = x;
    widget->y = y;
}

void gui_widget_set_width(gui_widget_t* widget, graphics_size_t width)
{
    widget->width = width;
}

void gui_widget_set_height(gui_widget_t* widget, graphics_size_t height)
{
    widget->height = height;
}

void gui_widget_resize(gui_widget_t* widget, graphics_size_t width, graphics_size_t height)
{
    widget->width = width;
    widget->height = height;
}