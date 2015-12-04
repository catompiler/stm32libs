#include "gui_checkbox.h"
#include "utils/utils.h"
#include "graphics/painter.h"

err_t gui_checkbox_init(gui_checkbox_t* checkbox, gui_t* gui)
{
    return gui_checkbox_init_parent(checkbox, gui, NULL);
}

err_t gui_checkbox_init_parent(gui_checkbox_t* checkbox, gui_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(checkbox), gui, parent));
    
    GUI_WIDGET(checkbox)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_checkbox_on_repaint);
    GUI_WIDGET(checkbox)->focusable = true;
    checkbox->text = NULL;
    checkbox->checked = false;
    checkbox->on_toggled = NULL;
    
    return E_NO_ERROR;
}

void gui_checkbox_set_text(gui_checkbox_t* checkbox, const char* text)
{
    checkbox->text = text;
    gui_widget_repaint(GUI_WIDGET(checkbox), NULL);
}

void gui_checkbox_set_checked(gui_checkbox_t* checkbox, bool checked)
{
    checkbox->checked = checked;
    
    if(checkbox->on_toggled) checkbox->on_toggled(checkbox, checked);
    
    gui_widget_repaint(GUI_WIDGET(checkbox), NULL);
}

// Типы флажка
// Крестик.
#define CHECKBOX_CHECK_X 0
// Галочка.
#define CHECKBOX_CHECK_V 1
// Тип флажка.
#ifndef CHECKBOX_CHECK
#define CHECKBOX_CHECK CHECKBOX_CHECK_V
#endif
// Размеры флажка,
#define CHECKBOX_SIZE 10
#define CHECKBOX_LEFT 1
#define TEXT_LEFT (CHECKBOX_SIZE + (CHECKBOX_SIZE) / 2 + CHECKBOX_LEFT)
#define CHECKBOX_BOX_DELTA 2

void gui_checkbox_on_repaint(gui_checkbox_t* checkbox, const rect_t* rect)
{
    gui_widget_on_repaint(GUI_WIDGET(checkbox), rect);
    
    gui_theme_t* theme = gui_theme(gui_object_gui(GUI_OBJECT(checkbox)));
    
    painter_t painter;
    
    gui_widget_begin_paint(GUI_WIDGET(checkbox), &painter, rect);
    
    //graphics_pos_t widget_width = (graphics_pos_t)gui_widget_width(GUI_WIDGET(checkbox));
    graphics_pos_t widget_height = (graphics_pos_t)gui_widget_height(GUI_WIDGET(checkbox));
    
    graphics_pos_t check_left = 1;
    graphics_pos_t check_top = (widget_height - CHECKBOX_SIZE) / 2;
    graphics_pos_t check_right = check_left + CHECKBOX_SIZE - 1;
    graphics_pos_t check_bottom = check_top + CHECKBOX_SIZE - 1;
    
    painter_set_pen(&painter, PAINTER_PEN_SOLID);
    painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
    painter_set_pen_color(&painter, theme->front_color);
    painter_set_brush_color(&painter, theme->back_color);
    
    painter_draw_rect(&painter, check_left, check_top, check_right, check_bottom);
    
    if(checkbox->checked){
#if CHECKBOX_CHECK == CHECKBOX_CHECK_X
        check_left += CHECKBOX_BOX_DELTA; check_top += CHECKBOX_BOX_DELTA;
        check_right -= CHECKBOX_BOX_DELTA; check_bottom -= CHECKBOX_BOX_DELTA;
        
        painter_draw_line(&painter, check_left, check_top, check_right, check_bottom);
        painter_draw_line(&painter, check_left, check_bottom, check_right, check_top);
#elif CHECKBOX_CHECK == CHECKBOX_CHECK_V  
        graphics_pos_t check_left_half = check_left + (CHECKBOX_SIZE / 2);// - CHECKBOX_BOX_DELTA;
        graphics_pos_t check_top_half = check_top + (CHECKBOX_SIZE / 2);// - CHECKBOX_BOX_DELTA;
        
        check_left += CHECKBOX_BOX_DELTA; check_top += CHECKBOX_BOX_DELTA;
        check_right -= CHECKBOX_BOX_DELTA; check_bottom -= CHECKBOX_BOX_DELTA;
        
        painter_draw_line(&painter, check_left, check_top_half, check_left_half - 1, check_bottom);
        painter_draw_line(&painter, check_left_half, check_bottom - 1, check_right, check_top);
#else
#error CHECKBOX_CHECK define: invalid value (0 - [X], 1 - [V])!
#endif
    }
    
    if(checkbox->text != NULL){
        
        painter_set_pen_color(&painter, theme->font_color);
        painter_set_font(&painter, theme->widget_font);
        painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
        
        graphics_pos_t text_x, text_y;
        painter_string_size(&painter, checkbox->text, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);

        text_x = TEXT_LEFT;// + (widget_width - text_x) / 2;
        text_y = (widget_height - text_y) / 2;

        painter_draw_string(&painter, text_x, text_y, checkbox->text);
    }
    
    gui_widget_end_paint(GUI_WIDGET(checkbox), &painter);
}


