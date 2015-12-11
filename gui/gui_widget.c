#include "gui_widget.h"
#include "utils/utils.h"
#include "graphics/painter.h"


static widget_id_t next_widget_id = 1;



static void gui_wdiget_foreach_childs_repaint(void* widget, void* event)
{
    gui_widget_repaint_event(GUI_WIDGET(widget), GUI_REPAINT_EVENT(event));
}

void gui_widget_resize_event(gui_widget_t* widget, gui_resize_event_t* event)
{
    if(event == NULL) return;
    if(widget->on_resize) widget->on_resize(widget, event->width, event->height);
}

void gui_widget_repaint_event(gui_widget_t* widget, gui_repaint_event_t* event)
{
    if(!gui_widget_visible_parents(widget)) return;
    
    if(gui_widget_width(widget) == 0 || gui_widget_height(widget) == 0) return;
    
    rect_t* rect = NULL;
    if(event) rect = &event->rect;
    
    if(widget->on_repaint) widget->on_repaint(widget, rect);
    
    list_foreach2_second(&GUI_OBJECT(widget)->childs, gui_wdiget_foreach_childs_repaint, event);
}

void gui_widget_key_event(gui_widget_t* widget, gui_key_event_t* event)
{
    if(event == NULL) return;
    
    if(GUI_EVENT(event)->type == GUI_EVENT_TYPE_KEY_PRESS){
        if(widget->on_key_press) widget->on_key_press(widget, event->key);
    }else{
        if(widget->on_key_release) widget->on_key_release(widget, event->key);
    }
}

err_t gui_widget_init(gui_widget_t* widget, gui_t* gui)
{
    return gui_widget_init_parent(widget, gui, NULL);
}

err_t gui_widget_init_parent(gui_widget_t* widget, gui_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_object_init_parent(GUI_OBJECT(widget), gui, GUI_OBJECT(parent)));
    
    widget->id = next_widget_id ++;
    widget->type_id = GUI_WIDGET_TYPE_ID;
    widget->visible = false;
    widget->focusable = false;
    rect_init(&widget->rect);
    widget->border = GUI_BORDER_NONE;
    widget->back_color = gui_theme(gui_object_gui(GUI_OBJECT(widget)))->panel_color;
    widget->on_resize = gui_widget_on_resize;
    widget->on_repaint = gui_widget_on_repaint;
    widget->on_key_press = gui_widget_on_key_press;
    widget->on_key_release = gui_widget_on_key_release;
    
    return E_NO_ERROR;
}

void gui_widget_set_border(gui_widget_t* widget, gui_border_t border)
{
    widget->border = border;
    gui_widget_repaint(widget, NULL);
}

void gui_widget_set_back_color(gui_widget_t* widget, graphics_color_t back_color)
{
    widget->back_color = back_color;
    gui_widget_repaint(widget, NULL);
}

bool gui_widget_visible_parents(gui_widget_t* widget)
{
    if(!gui_widget_visible(widget)) return false;
    gui_object_t* parent = gui_object_parent(GUI_OBJECT(widget));
    
    while(parent){
        if(!gui_widget_visible(GUI_WIDGET(parent))) return false;
        parent = gui_object_parent(parent);
    }
    
    return true;
}

void gui_widget_set_visible(gui_widget_t* widget, bool visible)
{
    if(widget->visible == visible) return;
    
    widget->visible = visible;
    
    if(visible){
        gui_widget_repaint(widget, NULL);
    }else{
        gui_object_t* parent = gui_object_parent(GUI_OBJECT(widget));
        if(parent){
            rect_t rect;
            gui_widget_screen_rect(widget, &rect);
            gui_widget_repaint(GUI_WIDGET(parent), &rect);
        }
        if(gui_widget_has_focus(widget)){
            gui_clear_focus_widget(gui_object_gui(GUI_OBJECT(widget)));
        }
    }
}

void gui_widget_set_focus(gui_widget_t* widget)
{
    gui_set_focus_widget(gui_object_gui(GUI_OBJECT(widget)), widget);
}

void gui_widget_set_x(gui_widget_t* widget, graphics_pos_t x)
{
    gui_widget_move(widget, x, rect_y(&widget->rect));
}

void gui_widget_set_y(gui_widget_t* widget, graphics_pos_t y)
{
    gui_widget_move(widget, rect_x(&widget->rect), y);
}

graphics_pos_t gui_widget_screen_x(gui_widget_t* widget)
{
    graphics_pos_t x = gui_widget_x(widget);
    
    gui_object_t* parent = gui_object_parent(GUI_OBJECT(widget));
    
    while(parent){
        x += gui_widget_x(GUI_WIDGET(parent));
        parent = gui_object_parent(parent);
    }
    
    return x;
}

graphics_pos_t gui_widget_screen_y(gui_widget_t* widget)
{
    graphics_pos_t y = gui_widget_y(widget);
    
    gui_object_t* parent = gui_object_parent(GUI_OBJECT(widget));
    
    while(parent){
        y += gui_widget_y(GUI_WIDGET(parent));
        parent = gui_object_parent(parent);
    }
    
    return y;
}

void gui_widget_screen_position(gui_widget_t* widget, point_t* point)
{
    if(point == NULL) return;
    
    point->x = gui_widget_x(widget);
    point->y = gui_widget_y(widget);
    
    gui_object_t* parent = gui_object_parent(GUI_OBJECT(widget));
    
    while(parent){
        point->x += gui_widget_x(GUI_WIDGET(parent));
        point->y += gui_widget_y(GUI_WIDGET(parent));
        parent = gui_object_parent(parent);
    }
}

void gui_widget_screen_rect(gui_widget_t* widget, rect_t* rect)
{
    if(rect == NULL) return;
    
    rect_t widget_rect;
    gui_widget_rect(widget, &widget_rect);
    
    rect_t parent_rect;
    
    gui_object_t* parent = gui_object_parent(GUI_OBJECT(widget));
    
    while(parent){
        gui_widget_rect(GUI_WIDGET(parent), &parent_rect);
        
        widget_rect.left += parent_rect.left;
        widget_rect.top += parent_rect.top;
        widget_rect.right += parent_rect.left;
        widget_rect.bottom += parent_rect.top;
        
        //widget_rect.left = MAX(widget_rect.left, parent_rect.left);
        //widget_rect.top = MAX(widget_rect.top, parent_rect.top);
        //widget_rect.right = MIN(widget_rect.right, parent_rect.right);
        //widget_rect.bottom = MIN(widget_rect.bottom, parent_rect.bottom);
        
        parent = gui_object_parent(parent);
    }
    
    rect_copy(rect, &widget_rect);
}

void gui_widget_screen_visible_position(gui_widget_t* widget, point_t* point, rect_t* rect)
{
    if(point == NULL && rect == NULL) return;
    
    point_t widget_point;
    gui_widget_position(widget, &widget_point);
    
    rect_t widget_rect;
    gui_widget_rect(widget, &widget_rect);
    
    rect_t parent_rect;
    
    gui_object_t* parent = gui_object_parent(GUI_OBJECT(widget));
    
    while(parent){
        gui_widget_rect(GUI_WIDGET(parent), &parent_rect);
        
        widget_point.x += rect_x(&parent_rect);
        widget_point.y += rect_y(&parent_rect);
        
        widget_rect.left += parent_rect.left;
        widget_rect.top += parent_rect.top;
        widget_rect.right += parent_rect.left;
        widget_rect.bottom += parent_rect.top;
        
        widget_rect.left = MAX(widget_rect.left, parent_rect.left);
        widget_rect.top = MAX(widget_rect.top, parent_rect.top);
        widget_rect.right = MIN(widget_rect.right, parent_rect.right);
        widget_rect.bottom = MIN(widget_rect.bottom, parent_rect.bottom);
        
        parent = gui_object_parent(parent);
    }
    
    if(point) point_copy(point, &widget_point);
    if(rect)  rect_copy(rect, &widget_rect);
}

void gui_widget_move(gui_widget_t* widget, graphics_pos_t x, graphics_pos_t y)
{
    if(x == gui_widget_x(widget) && y == gui_widget_y(widget)) return;
    
    gui_object_t* parent = gui_object_parent(GUI_OBJECT(widget));
    
    if(gui_widget_visible(widget)){
        if(parent){
            rect_t rect;
            gui_repaint_event_t event;

            gui_widget_screen_rect(widget, &rect);

            rect_set_x(&widget->rect, x);
            rect_set_y(&widget->rect, y);

            gui_repaint_event_init_rect(&event, &rect);
            gui_widget_repaint_event(GUI_WIDGET(parent), &event);
        } else {
            rect_set_x(&widget->rect, x);
            rect_set_y(&widget->rect, y);
        }
        gui_widget_repaint(widget, NULL);
    } else {
        rect_set_x(&widget->rect, x);
        rect_set_y(&widget->rect, y);
    }
}

void gui_widget_set_width(gui_widget_t* widget, graphics_size_t width)
{
    gui_widget_resize(widget, width, rect_height(&widget->rect));
}

void gui_widget_set_height(gui_widget_t* widget, graphics_size_t height)
{
    gui_widget_resize(widget, rect_width(&widget->rect), height);
}

void gui_widget_resize(gui_widget_t* widget, graphics_size_t width, graphics_size_t height)
{
    graphics_size_t old_width = gui_widget_width(widget);
    graphics_size_t old_height = gui_widget_height(widget);
    
    if(old_width == width && old_height == height) return;
    
    graphics_size_t paint_width = MAX(width, old_width);
    graphics_size_t paint_height = MAX(height, old_height);
    
    rect_set_width(&widget->rect, width);
    rect_set_height(&widget->rect, height);
    
    gui_resize_event_t event;
    gui_resize_event_init(&event, width, height);
    gui_widget_resize_event(widget, &event);
    
    gui_object_t* parent = gui_object_parent(GUI_OBJECT(widget));

    if(parent){
        point_t point;
        gui_repaint_event_t event;

        gui_widget_screen_position(widget, &point);

        gui_repaint_event_init(&event, point.x, point.y, paint_width, paint_height);
        gui_widget_repaint_event(GUI_WIDGET(parent), &event);
    }
    gui_widget_repaint(widget, NULL);
}

void gui_widget_repaint(gui_widget_t* widget, const rect_t* rect)
{
    if(!gui_widget_visible(widget)) return;
    gui_repaint_event_t event;
    if(rect){
        gui_repaint_event_init_rect(&event, rect);
    }else{
        rect_t widget_rect;
        gui_widget_screen_rect(widget, &widget_rect);
        gui_repaint_event_init_rect(&event, &widget_rect);
    }
    gui_widget_repaint_event(widget, &event);
}

void gui_widget_on_resize(gui_widget_t* widget, graphics_size_t width, graphics_size_t height)
{
}

void gui_widget_on_repaint(gui_widget_t* widget, const rect_t* rect)
{
    painter_t painter;
    
    if(gui_widget_begin_paint(widget, &painter, rect) != E_NO_ERROR) return;
    
    gui_theme_t* theme = gui_theme(gui_widget_gui(widget));
    
    painter_set_pen(&painter, PAINTER_PEN_SOLID);
    painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
    painter_set_brush_color(&painter, widget->back_color);
    
    if(gui_widget_has_focus(widget)){
        painter_set_pen_color(&painter, theme->focus_color);
    }else{
        painter_set_pen_color(&painter, theme->border_color);
    }
    
    if(widget->border == GUI_BORDER_NONE && !gui_widget_has_focus(widget)){
        painter_draw_fillrect(&painter, 0, 0, rect_width(&widget->rect) - 1, rect_height(&widget->rect) - 1);
    }else{
        painter_draw_rect(&painter, 0, 0, rect_width(&widget->rect) - 1, rect_height(&widget->rect) - 1);
    }
    
    gui_widget_end_paint(widget, &painter);
}

void gui_widget_on_key_press(gui_widget_t* widget, keycode_t key)
{
}

void gui_widget_on_key_release(gui_widget_t* widget, keycode_t key)
{
}

err_t gui_widget_begin_paint(gui_widget_t* widget, painter_t* painter, const rect_t* rect)
{
    if(painter == NULL) return E_NULL_POINTER;
    
    RETURN_ERR_IF_FAIL(painter_init(painter, gui_graphics(gui_object_gui(GUI_OBJECT(widget)))));
    
    point_t widget_point;
    rect_t widget_rect;
    gui_widget_screen_visible_position(widget, &widget_point, &widget_rect);
    
    rect_t paint_rect;
    
    if(rect != NULL){
        rect_copy(&paint_rect, rect);
        rect_clip(&paint_rect, &widget_rect);
    }else{
        rect_copy(&paint_rect, &widget_rect);
    }
    
    painter_set_scissor_rect(painter, &paint_rect);
    painter_set_scissor_enabled(painter, true);
    
    painter_set_offset(painter, point_x(&widget_point), point_y(&widget_point));
    painter_set_offset_enabled(painter, true);
    
    return E_NO_ERROR;
}

err_t gui_widget_end_paint(gui_widget_t* widget, painter_t* painter)
{
    if(painter == NULL) return E_NULL_POINTER;
    
    painter_flush(painter);
    
    return E_NO_ERROR;
}
