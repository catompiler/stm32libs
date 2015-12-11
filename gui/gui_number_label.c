#include "gui_number_label.h"
#include <stdio.h>
#include "utils/utils.h"
#include "graphics/painter.h"

err_t gui_number_label_init(gui_number_label_t* label, gui_t* gui)
{
    return gui_number_label_init_parent(label, gui, NULL);
}

err_t gui_number_label_init_parent(gui_number_label_t* label, gui_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(label), gui, parent));
    
    GUI_WIDGET(label)->type_id = GUI_NUMBER_LABEL_TYPE_ID;
    GUI_WIDGET(label)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_number_label_on_repaint);
    label->number = 0;
    
    return E_NO_ERROR;
}

void gui_number_label_set_number(gui_number_label_t* label, int number)
{
    label->number = number;
    gui_widget_repaint(GUI_WIDGET(label), NULL);
}

void gui_number_label_set_format(gui_number_label_t* label, gui_number_label_format_t format)
{
    label->format = format;
    gui_widget_repaint(GUI_WIDGET(label), NULL);
}

#define NUMBER_BUFFER_LEN 12 //-2 147 483 648

void gui_number_label_on_repaint(gui_number_label_t* label, const rect_t* rect)
{
    gui_widget_on_repaint(GUI_WIDGET(label), rect);
    
    gui_theme_t* theme = gui_theme(gui_object_gui(GUI_OBJECT(label)));
    
    painter_t painter;
    
    gui_widget_begin_paint(GUI_WIDGET(label), &painter, rect);
    
    painter_set_pen(&painter, PAINTER_PEN_SOLID);
    painter_set_pen_color(&painter, theme->font_color);
    painter_set_font(&painter, theme->widget_font);
    painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
    
    char num_buffer[NUMBER_BUFFER_LEN];
    switch(label->format){
        default:
        case GUI_NUMBER_LABEL_DEC:
            snprintf(num_buffer, NUMBER_BUFFER_LEN, "%d", label->number);
            break;
        case GUI_NUMBER_LABEL_HEX:
            snprintf(num_buffer, NUMBER_BUFFER_LEN, "0x%x", label->number);
            break;
    }
    graphics_pos_t number_x, number_y;
    painter_string_size(&painter, num_buffer, (graphics_size_t*)&number_x, (graphics_size_t*)&number_y);
    
    number_x = ((graphics_pos_t)gui_widget_width(GUI_WIDGET(label)) - number_x) / 2;
    number_y = ((graphics_pos_t)gui_widget_height(GUI_WIDGET(label)) - number_y) / 2;
    
    painter_draw_string(&painter, number_x, number_y, num_buffer);
    
    gui_widget_end_paint(GUI_WIDGET(label), &painter);
}
