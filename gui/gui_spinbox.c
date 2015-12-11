#include "gui_spinbox.h"
#include <stdio.h>
#include "utils/utils.h"
#include "graphics/painter.h"
#include "painter.h"
#include "keys.h"
#include <limits.h>


err_t gui_spinbox_init(gui_spinbox_t* spinbox, gui_t* gui)
{
    return gui_spinbox_init_parent(spinbox, gui, NULL);
}

err_t gui_spinbox_init_parent(gui_spinbox_t* spinbox, gui_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(spinbox), gui, parent));
    
    gui_theme_t* theme = gui_theme(gui_object_gui(GUI_OBJECT(spinbox)));
    
    GUI_WIDGET(spinbox)->type_id = GUI_SPINBOX_TYPE_ID;
    GUI_WIDGET(spinbox)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_spinbox_on_repaint);
    GUI_WIDGET(spinbox)->on_key_press = GUI_WIDGET_ON_KEY_PRESS_PROC(gui_spinbox_on_key_press);
    GUI_WIDGET(spinbox)->on_key_release = GUI_WIDGET_ON_KEY_RELEASE_PROC(gui_spinbox_on_key_release);
    GUI_WIDGET(spinbox)->focusable = true;
    GUI_WIDGET(spinbox)->back_color = theme->back_color;
    GUI_WIDGET(spinbox)->border = GUI_BORDER_SOLID;
    spinbox->format = GUI_NUMBER_LABEL_DEC;
    spinbox->value = 0;
    spinbox->increment = 1;
    spinbox->minimum = INT32_MIN;
    spinbox->maximum = INT32_MAX;
    spinbox->spin_size = GUI_SPINBOX_DEFAULT_SPIN_SIZE;
    spinbox->on_value_changed = NULL;
    
    return E_NO_ERROR;
}

void gui_spinbox_set_value(gui_spinbox_t* spinbox, int value)
{
    if(spinbox->value == value) return;
    
    spinbox->value = value;
    
    if(spinbox->on_value_changed) spinbox->on_value_changed(spinbox, value);
    
    gui_widget_repaint(GUI_WIDGET(spinbox), NULL);
}

void gui_spinbox_inc_value(gui_spinbox_t* spinbox)
{
    int value = spinbox->value;
    
    CYCLIC_ADD(value, spinbox->increment, spinbox->minimum, spinbox->maximum);
    
    gui_spinbox_set_value(spinbox, value);
}

void gui_spinbox_dec_value(gui_spinbox_t* spinbox)
{
    int value = spinbox->value;
    
    CYCLIC_SUB(value, spinbox->increment, spinbox->minimum, spinbox->maximum);
    
    gui_spinbox_set_value(spinbox, value);
}

void gui_spinbox_set_maximum(gui_spinbox_t* spinbox, int maximum)
{
    gui_spinbox_set_range(spinbox, spinbox->minimum, maximum);
}

void gui_spinbox_set_minimum(gui_spinbox_t* spinbox, int minimum)
{
    gui_spinbox_set_range(spinbox, minimum, spinbox->maximum);
}

void gui_spinbox_set_range(gui_spinbox_t* spinbox, int minimum, int maximum)
{
    if(maximum < minimum) return;
    
    spinbox->maximum = maximum;
    spinbox->minimum = minimum;
    
    if(spinbox->value < minimum){
        gui_spinbox_set_value(spinbox, minimum);
    }else if(spinbox->value > maximum){
        gui_spinbox_set_value(spinbox, maximum);
    }
}

void gui_spinbox_set_format(gui_spinbox_t* spinbox, gui_number_label_format_t format)
{
    if(spinbox->format == format) return;
    spinbox->format = format;
    gui_widget_repaint(GUI_WIDGET(spinbox), NULL);
}

void gui_spinbox_set_spin_size(gui_spinbox_t* spinbox, size_t size)
{
    if(spinbox->spin_size == size) return;
    spinbox->spin_size = size;
    gui_widget_repaint(GUI_WIDGET(spinbox), NULL);
}

#define NUMBER_BUFFER_LEN 12 //-2 147 483 648

void gui_spinbox_on_repaint(gui_spinbox_t* spinbox, const rect_t* rect)
{
    gui_theme_t* theme = gui_theme(gui_object_gui(GUI_OBJECT(spinbox)));
    
    painter_t painter;
    
    gui_widget_begin_paint(GUI_WIDGET(spinbox), &painter, rect);
    
    painter_set_pen(&painter, PAINTER_PEN_SOLID);
    painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
    
    painter_set_brush_color(&painter, GUI_WIDGET(spinbox)->back_color);
    
    if(gui_widget_has_focus(GUI_WIDGET(spinbox))){
        painter_set_pen_color(&painter, theme->focus_color);
    }else{
        painter_set_pen_color(&painter, theme->border_color);
    }
    
    if(GUI_WIDGET(spinbox)->border == GUI_BORDER_NONE && !gui_widget_has_focus(GUI_WIDGET(spinbox))){
        painter_draw_fillrect(&painter, 0, 0, rect_width(&GUI_WIDGET(spinbox)->rect) - 1 - spinbox->spin_size, rect_height(&GUI_WIDGET(spinbox)->rect) - 1);
    }else{
        painter_set_brush(&painter, PAINTER_BRUSH_NONE);
        painter_draw_rect(&painter, 0, 0, rect_width(&GUI_WIDGET(spinbox)->rect) - 1, rect_height(&GUI_WIDGET(spinbox)->rect) - 1);
        painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
        painter_draw_fillrect(&painter, 1, 1, rect_width(&GUI_WIDGET(spinbox)->rect) - 1 - spinbox->spin_size, rect_height(&GUI_WIDGET(spinbox)->rect) - 2);
    }
    
    graphics_pos_t widget_width = (graphics_pos_t)gui_widget_width(GUI_WIDGET(spinbox));
    graphics_pos_t widget_height = (graphics_pos_t)gui_widget_height(GUI_WIDGET(spinbox));
    graphics_pos_t spinbox_halfwidth = spinbox->spin_size  / 2;
    graphics_pos_t height_half_top = widget_height / 2;
    graphics_pos_t height_half_bottom = widget_height / 2;
    if(!(widget_height & 0x1)) height_half_top --;
    graphics_pos_t top = 1;
    graphics_pos_t bottom = widget_height - 2;
    graphics_pos_t left = widget_width - spinbox->spin_size;
    graphics_pos_t right = widget_width - 2;
    
    
    painter_set_pen_color(&painter, theme->border_color);
    
    painter_draw_vline(&painter, left, top, bottom);
    painter_draw_hline(&painter, height_half_top, left, right);
    if(height_half_top != height_half_bottom){
        painter_draw_hline(&painter, height_half_bottom, left, right);
    }
    
    painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
    if(!spinbox->incrementing){
        painter_set_brush_color(&painter, theme->widget_color);
    }else{
        painter_set_brush_color(&painter, theme->pressed_color);
    }
    painter_draw_fillrect(&painter, left + 1, top, right, height_half_top - 1);
    if(!spinbox->decrementing){
        painter_set_brush_color(&painter, theme->widget_color);
    }else{
        painter_set_brush_color(&painter, theme->pressed_color);
    }
    painter_draw_fillrect(&painter, left + 1, height_half_bottom + 1, right, bottom);
    
    painter_set_pen_color(&painter, theme->front_color);
    painter_set_brush_color(&painter, theme->front_color);
    
    painter_draw_triangle(&painter, left + 2, height_half_top - 3, 
                          left + spinbox_halfwidth, top + 2,
                          right - 1, height_half_top - 3);
    painter_draw_triangle(&painter, left + 2, height_half_bottom + 3, 
                          left + spinbox_halfwidth, bottom - 2,
                          right - 1, height_half_bottom + 3);
    
    painter_set_pen_color(&painter, theme->font_color);
    painter_set_font(&painter, theme->widget_font);
    painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
    
    char num_buffer[NUMBER_BUFFER_LEN];
    switch(spinbox->format){
        default:
        case GUI_NUMBER_LABEL_DEC:
            snprintf(num_buffer, NUMBER_BUFFER_LEN, "%d", spinbox->value);
            break;
        case GUI_NUMBER_LABEL_HEX:
            snprintf(num_buffer, NUMBER_BUFFER_LEN, "0x%x", spinbox->value);
            break;
    }
    graphics_pos_t number_x, number_y;
    painter_string_size(&painter, num_buffer, (graphics_size_t*)&number_x, (graphics_size_t*)&number_y);
    
    number_x = widget_width - number_x - spinbox->spin_size - spinbox->spin_size / 2;
    number_y = (widget_height - number_y) / 2;
    
    painter_draw_string(&painter, number_x, number_y, num_buffer);
    
    gui_widget_end_paint(GUI_WIDGET(spinbox), &painter);
}

void gui_spinbox_on_key_press(gui_spinbox_t* spinbox, key_t key)
{
    if(key == KEY_UP){
        spinbox->incrementing = true;
        gui_spinbox_inc_value(spinbox);
    }else if(key == KEY_DOWN){
        spinbox->decrementing = true;
        gui_spinbox_dec_value(spinbox);
    }
}

void gui_spinbox_on_key_release(gui_spinbox_t* spinbox, key_t key)
{
    if(key == KEY_UP){
        spinbox->incrementing = false;
        gui_widget_repaint(GUI_WIDGET(spinbox), NULL);
    }else if(key == KEY_DOWN){
        spinbox->decrementing = false;
        gui_widget_repaint(GUI_WIDGET(spinbox), NULL);
    }
}
