#include "gui.h"
#include "gui_widget.h"

err_t gui_init(gui_t* gui, graphics_t* graphics, gui_theme_t* theme)
{
    if(graphics == NULL) return E_NULL_POINTER;
    if(theme == NULL) return E_NULL_POINTER;
    
    gui->graphics = graphics;
    gui->theme = theme;
    gui->root_widget = NULL;
    
    return E_NO_ERROR;
}

err_t gui_set_graphics(gui_t* gui, graphics_t* graphics)
{
    if(graphics == NULL) return E_NULL_POINTER;
    
    gui->graphics = graphics;
    
    return E_NO_ERROR;
}

err_t gui_set_theme(gui_t* gui, gui_theme_t* theme)
{
    if(theme == NULL) return E_NULL_POINTER;
    
    gui->theme = theme;
    
    return E_NO_ERROR;
}

err_t gui_set_root_widget(gui_t* gui, gui_widget_t* widget)
{
    if(widget == NULL) return E_NULL_POINTER;
    
    gui->root_widget = widget;
    
    return E_NO_ERROR;
}

bool gui_set_focus_widget(gui_t* gui, gui_widget_t* widget)
{
    if(gui->focus_widget == widget) return false;
    if(widget){
        if(!gui_widget_focusable(widget)) return false;
        if(!gui_widget_visible_parents(widget)) return false;
    }
    
    gui_widget_t* old_widget = gui->focus_widget;
    
    gui->focus_widget = widget;
    
    if(old_widget){
        gui_widget_repaint(old_widget, NULL);
    }
    if(widget){
        gui_widget_repaint(widget, NULL);
    }
    return true;
}

static gui_widget_t* gui_first_widget(gui_widget_t* widget)
{
    if(widget == NULL) return NULL;
    
    while(gui_widget_first_child(widget)){
        widget = gui_widget_first_child(widget);
    }
    
    return widget;
}

static gui_widget_t* gui_next_widget(gui_widget_t* widget)
{
    if(widget == NULL) return NULL;
    
    if(gui_widget_next_child(widget)){
        return gui_first_widget(gui_widget_next_child(widget));
    }
    return gui_widget_parent(widget);
}

bool gui_focus_next_widget(gui_t* gui)
{
    gui_widget_t* widget = gui->focus_widget;
    if(widget == NULL){
        widget = gui->root_widget;
    }
    if(widget == NULL) return false;
    
    gui_widget_t* start_widget = widget;
    
    for(;;){
        if(widget == gui->root_widget){
            widget = gui_first_widget(widget);
        }else{
            widget = gui_next_widget(widget);
        }
        
        if(widget == NULL) widget = gui->root_widget;
        if(gui_set_focus_widget(gui, widget)) return true;
        if(widget == start_widget) break;
    }
    return false;
}

static gui_widget_t* gui_last_widget(gui_widget_t* widget)
{
    if(widget == NULL) return NULL;
    
    while(gui_widget_last_child(widget)){
        widget = gui_widget_last_child(widget);
    }
    
    return widget;
}

static gui_widget_t* gui_prev_widget(gui_widget_t* widget)
{
    if(widget == NULL) return NULL;
    
    if(gui_widget_prev_child(widget)){
        return gui_last_widget(gui_widget_prev_child(widget));
    }
    return gui_widget_parent(widget);
}

bool gui_focus_prev_widget(gui_t* gui)
{
    gui_widget_t* widget = gui->focus_widget;
    if(widget == NULL){
        widget = gui->root_widget;
    }
    if(widget == NULL) return false;
    
    gui_widget_t* start_widget = widget;
    
    for(;;){
        if(widget == gui->root_widget){
            widget = gui_last_widget(widget);
        }else{
            widget = gui_prev_widget(widget);
        }
        
        if(widget == NULL) widget = gui->root_widget;
        if(gui_set_focus_widget(gui, widget)) return true;
        if(widget == start_widget) break;
    }
    return false;
}