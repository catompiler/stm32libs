#include "gui.h"

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
