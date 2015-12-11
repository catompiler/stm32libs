#include "gui_label.h"
#include "utils/utils.h"
#include "graphics/painter.h"

err_t gui_label_init(gui_label_t* label, gui_t* gui)
{
    return gui_label_init_parent(label, gui, NULL);
}

err_t gui_label_init_parent(gui_label_t* label, gui_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(label), gui, parent));
    
    GUI_WIDGET(label)->type_id = GUI_LABEL_TYPE_ID;
    GUI_WIDGET(label)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_label_on_repaint);
    
    return E_NO_ERROR;
}

void gui_label_set_text(gui_label_t* label, const char* text)
{
    label->text = text;
    gui_widget_repaint(GUI_WIDGET(label), NULL);
}

void gui_label_on_repaint(gui_label_t* label, const rect_t* rect)
{
    gui_widget_on_repaint(GUI_WIDGET(label), rect);
    
    if(label->text == NULL) return;
    
    gui_theme_t* theme = gui_theme(gui_object_gui(GUI_OBJECT(label)));
    
    painter_t painter;
    
    gui_widget_begin_paint(GUI_WIDGET(label), &painter, rect);
    
    painter_set_pen(&painter, PAINTER_PEN_SOLID);
    painter_set_pen_color(&painter, theme->font_color);
    painter_set_font(&painter, theme->widget_font);
    painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMAP);
    
    graphics_pos_t text_x, text_y;
    painter_string_size(&painter, label->text, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
    
    text_x = ((graphics_pos_t)gui_widget_width(GUI_WIDGET(label)) - text_x) / 2;
    text_y = ((graphics_pos_t)gui_widget_height(GUI_WIDGET(label)) - text_y) / 2;
    
    painter_draw_string(&painter, text_x, text_y, label->text);
    
    gui_widget_end_paint(GUI_WIDGET(label), &painter);
}
