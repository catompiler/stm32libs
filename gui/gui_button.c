#include "gui_button.h"
#include "utils/utils.h"
#include "graphics/painter.h"
#include "input/keys.h"

err_t gui_button_init(gui_button_t* button, gui_t* gui)
{
    return gui_button_init_parent(button, gui, NULL);
}

err_t gui_button_init_parent(gui_button_t* button, gui_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(button), gui, parent));
    
    gui_theme_t* theme = gui_theme(gui_object_gui(GUI_OBJECT(button)));
    
    GUI_WIDGET(button)->type_id = GUI_BUTTON_TYPE_ID;
    GUI_WIDGET(button)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_button_on_repaint);
    GUI_WIDGET(button)->on_key_press = GUI_WIDGET_ON_KEY_PRESS_PROC(gui_button_on_key_press);
    GUI_WIDGET(button)->on_key_release = GUI_WIDGET_ON_KEY_RELEASE_PROC(gui_button_on_key_release);
    GUI_WIDGET(button)->focusable = true;
    GUI_WIDGET(button)->back_color = theme->widget_color;
    GUI_WIDGET(button)->border = GUI_BORDER_SOLID;
    button->text = NULL;
    button->pressing = false;
    button->on_clicked = NULL;
    
    return E_NO_ERROR;
}

void gui_button_set_text(gui_button_t* button, const char* text)
{
    button->text = text;
    gui_widget_repaint(GUI_WIDGET(button), NULL);
}

void gui_button_on_repaint(gui_button_t* button, const rect_t* rect)
{
    gui_theme_t* theme = gui_theme(gui_object_gui(GUI_OBJECT(button)));
    
    painter_t painter;
    
    gui_widget_begin_paint(GUI_WIDGET(button), &painter, rect);
    
    painter_set_pen(&painter, PAINTER_PEN_SOLID);
    painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
    
    if(!button->pressing){
        painter_set_brush_color(&painter, GUI_WIDGET(button)->back_color);
    }else{
        painter_set_brush_color(&painter, theme->pressed_color);
    }
    
    if(gui_widget_has_focus(GUI_WIDGET(button))){
        painter_set_pen_color(&painter, theme->focus_color);
    }else{
        painter_set_pen_color(&painter, theme->border_color);
    }
    
    if(GUI_WIDGET(button)->border == GUI_BORDER_NONE && !gui_widget_has_focus(GUI_WIDGET(button))){
        painter_draw_fillrect(&painter, 0, 0, rect_width(&GUI_WIDGET(button)->rect) - 1, rect_height(&GUI_WIDGET(button)->rect) - 1);
    }else{
        painter_draw_rect(&painter, 0, 0, rect_width(&GUI_WIDGET(button)->rect) - 1, rect_height(&GUI_WIDGET(button)->rect) - 1);
    }
    
    if(button->text != NULL){
        painter_set_pen_color(&painter, theme->font_color);
        painter_set_font(&painter, theme->widget_font);
        painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);

        graphics_pos_t text_x, text_y;
        painter_string_size(&painter, button->text, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);

        text_x = ((graphics_pos_t)gui_widget_width(GUI_WIDGET(button)) - text_x) / 2;
        text_y = ((graphics_pos_t)gui_widget_height(GUI_WIDGET(button)) - text_y) / 2;

        painter_draw_string(&painter, text_x, text_y, button->text);
    }
    
    gui_widget_end_paint(GUI_WIDGET(button), &painter);
}

void gui_button_on_key_press(gui_button_t* button, keycode_t key)
{
    if(key == KEY_ENTER || key == KEY_SPACE){
        if(!button->pressing){
            button->pressing = true;
            gui_widget_repaint(GUI_WIDGET(button), NULL);
        }
    }
}

void gui_button_on_key_release(gui_button_t* button, keycode_t key)
{
    if(key == KEY_ENTER || key == KEY_SPACE){
        if(button->pressing){
            button->pressing = false;
            
            if(button->on_clicked) button->on_clicked(button);
            
            gui_widget_repaint(GUI_WIDGET(button), NULL);
        }
    }
}
