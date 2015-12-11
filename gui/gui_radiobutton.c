#include "gui_radiobutton.h"
#include "utils/utils.h"
#include "graphics/painter.h"
#include "keys.h"
#include "painter.h"

err_t gui_radiobutton_init(gui_radiobutton_t* radiobutton, gui_t* gui)
{
    return gui_radiobutton_init_parent(radiobutton, gui, NULL);
}

err_t gui_radiobutton_init_parent(gui_radiobutton_t* radiobutton, gui_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(radiobutton), gui, parent));
    
    GUI_WIDGET(radiobutton)->type_id = GUI_RADIOBUTTON_TYPE_ID;
    GUI_WIDGET(radiobutton)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_radiobutton_on_repaint);
    GUI_WIDGET(radiobutton)->on_key_press = GUI_WIDGET_ON_KEY_PRESS_PROC(gui_radiobutton_on_key_press);
    GUI_WIDGET(radiobutton)->focusable = true;
    radiobutton->text = NULL;
    radiobutton->checked = false;
    radiobutton->radio_size = GUI_RADIOBUTTON_DEFAULT_RADIO_SIZE;
    radiobutton->on_toggled = NULL;
    
    return E_NO_ERROR;
}

void gui_radiobutton_set_text(gui_radiobutton_t* radiobutton, const char* text)
{
    radiobutton->text = text;
    gui_widget_repaint(GUI_WIDGET(radiobutton), NULL);
}

static void gui_radiobutton_uncheck_other(gui_radiobutton_t* radiobutton)
{
    gui_widget_t* widget = gui_widget_prev_child(GUI_WIDGET(radiobutton));
    
    while(widget){
        if(gui_widget_type_id(widget) == GUI_RADIOBUTTON_TYPE_ID){
            gui_radiobutton_set_checked(GUI_RADIOBUTTON(widget), false);
        }
        widget = gui_widget_prev_child(widget);
    }
    
    widget = gui_widget_next_child(GUI_WIDGET(radiobutton));
    
    while(widget){
        if(gui_widget_type_id(widget) == GUI_RADIOBUTTON_TYPE_ID){
            gui_radiobutton_set_checked(GUI_RADIOBUTTON(widget), false);
        }
        widget = gui_widget_next_child(widget);
    }
}

void gui_radiobutton_set_checked(gui_radiobutton_t* radiobutton, bool checked)
{
    if(radiobutton->checked == checked) return;
    
    radiobutton->checked = checked;
    
    if(radiobutton->on_toggled) radiobutton->on_toggled(radiobutton, checked);
    
    if(checked){
        gui_radiobutton_uncheck_other(radiobutton);
    }
    
    gui_widget_repaint(GUI_WIDGET(radiobutton), NULL);
}

void gui_radiobutton_set_radio_size(gui_radiobutton_t* radiobutton, size_t size)
{
    if(radiobutton->radio_size == size) return;
    radiobutton->radio_size = size;
    gui_widget_repaint(GUI_WIDGET(radiobutton), NULL);
}

// Размеры переключателя,
#define RADIOBUTTON_LEFT 1
#define RADIOBUTTON_BOX_DELTA 2

void gui_radiobutton_on_repaint(gui_radiobutton_t* radiobutton, const rect_t* rect)
{
    gui_widget_on_repaint(GUI_WIDGET(radiobutton), rect);
    
    gui_theme_t* theme = gui_theme(gui_object_gui(GUI_OBJECT(radiobutton)));
    
    painter_t painter;
    
    gui_widget_begin_paint(GUI_WIDGET(radiobutton), &painter, rect);
    
    //graphics_pos_t widget_width = (graphics_pos_t)gui_widget_width(GUI_WIDGET(radiobutton));
    graphics_pos_t widget_height = (graphics_pos_t)gui_widget_height(GUI_WIDGET(radiobutton));
    graphics_pos_t radio_radius = radiobutton->radio_size / 2;
    graphics_pos_t check_center_x = RADIOBUTTON_LEFT + radio_radius;
    graphics_pos_t check_center_y = widget_height / 2;
    
    painter_set_pen(&painter, PAINTER_PEN_SOLID);
    painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
    painter_set_pen_color(&painter, theme->front_color);
    painter_set_brush_color(&painter, theme->back_color);
    
    painter_draw_circle(&painter, check_center_x, check_center_y, radio_radius);
    
    if(radiobutton->checked){
        painter_set_brush_color(&painter, theme->front_color);
        painter_draw_circle(&painter, check_center_x, check_center_y, radio_radius / 2);
    }
    
    if(radiobutton->text != NULL){
        
        painter_set_pen_color(&painter, theme->font_color);
        painter_set_font(&painter, theme->widget_font);
        painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
        
        graphics_pos_t text_x, text_y;
        painter_string_size(&painter, radiobutton->text, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);

        text_x = radio_radius * 3 + RADIOBUTTON_LEFT;
        text_y = (widget_height - text_y) / 2;

        painter_draw_string(&painter, text_x, text_y, radiobutton->text);
    }
    
    gui_widget_end_paint(GUI_WIDGET(radiobutton), &painter);
}

void gui_radiobutton_on_key_press(gui_radiobutton_t* radiobutton, key_t key)
{
    if(key == KEY_ENTER || key == KEY_SPACE){
        gui_radiobutton_set_checked(radiobutton, true);
    }
}
