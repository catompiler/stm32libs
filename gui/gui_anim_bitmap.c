#include "gui_anim_bitmap.h"
#include "utils/utils.h"
#include "graphics/painter.h"
#include "graphics/graphics.h"
#include <string.h>

#define GUI_ANIM_BITMAP_OFF 0
#define GUI_ANIM_BITMAP_ON 1


err_t gui_anim_bitmap_init(gui_anim_bitmap_t* anim_bitmap, gui_t* gui)
{
    return gui_anim_bitmap_init_parent(anim_bitmap, gui, NULL);
}

err_t gui_anim_bitmap_init_parent(gui_anim_bitmap_t* anim_bitmap, gui_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(anim_bitmap), gui, parent));
    
    gui_theme_t* theme = gui_theme(gui_object_gui(GUI_OBJECT(anim_bitmap)));
    
    GUI_WIDGET(anim_bitmap)->type_id = GUI_ANIM_BITMAP_TYPE_ID;
    GUI_WIDGET(anim_bitmap)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_anim_bitmap_on_repaint);
    GUI_WIDGET(anim_bitmap)->on_resize = GUI_WIDGET_ON_RESIZE_PROC(gui_anim_bitmap_on_resize);
    GUI_WIDGET(anim_bitmap)->back_color = theme->back_color;
    GUI_WIDGET(anim_bitmap)->focusable = false;
    anim_bitmap->front_color = theme->front_color;
    anim_bitmap->margin = 0;
    anim_bitmap->bitmap = NULL;
    anim_bitmap->target_bitmap = NULL;
    anim_bitmap->anim_items = NULL;
    anim_bitmap->anim_items_count = 0;
    //anim_bitmap->effect_add_type = GUI_ANIM_BITMAP_EFFECT_RESIZE;//GUI_ANIM_BITMAP_EFFECT_MOVE;//GUI_ANIM_BITMAP_NONE
    //anim_bitmap->effect_del_type = GUI_ANIM_BITMAP_EFFECT_RESIZE;//GUI_ANIM_BITMAP_EFFECT_MOVE;//GUI_ANIM_BITMAP_NONE
    //anim_bitmap->effect_add_type = GUI_ANIM_BITMAP_EFFECT_MOVE;//GUI_ANIM_BITMAP_NONE
    //anim_bitmap->effect_del_type = GUI_ANIM_BITMAP_EFFECT_MOVE;//GUI_ANIM_BITMAP_NONE
    //anim_bitmap->effect_add_type = GUI_ANIM_BITMAP_EFFECT_GRAVITY;//GUI_ANIM_BITMAP_EFFECT_RESIZE;//GUI_ANIM_BITMAP_EFFECT_MOVE;//GUI_ANIM_BITMAP_NONE
    //anim_bitmap->effect_del_type = GUI_ANIM_BITMAP_EFFECT_GRAVITY;//GUI_ANIM_BITMAP_EFFECT_RESIZE;//GUI_ANIM_BITMAP_EFFECT_MOVE;//GUI_ANIM_BITMAP_NONE
    anim_bitmap->effect_add_type = GUI_ANIM_BITMAP_EFFECT_NONE;
    anim_bitmap->effect_del_type = GUI_ANIM_BITMAP_EFFECT_NONE;
    anim_bitmap->anim_done = true;
    anim_bitmap->item_width = 0;
    anim_bitmap->item_height = 0;
    anim_bitmap->cur_pixel = 0;
    anim_bitmap->max_steps = 10;
    
    return E_NO_ERROR;
}

static void gui_anim_items_update_sizes(gui_anim_bitmap_t* anim_bitmap)
{
    graphics_size_t width = gui_widget_width(GUI_WIDGET(anim_bitmap));
    graphics_size_t height = gui_widget_height(GUI_WIDGET(anim_bitmap));
    
    if(anim_bitmap->bitmap){
        if(graphics_width(anim_bitmap->bitmap) != 0){
            anim_bitmap->item_width = width / graphics_width(anim_bitmap->bitmap);
        }else{
            anim_bitmap->item_width = 0;
        }
        if(graphics_height(anim_bitmap->bitmap) != 0){
            anim_bitmap->item_height = height / graphics_height(anim_bitmap->bitmap);
        }else{
            anim_bitmap->item_height = 0;
        }
    }
}

static void gui_anim_item_reset(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_item)
{
    memset(anim_item, 0x0, sizeof(gui_anim_bitmap_item_t));
    anim_item->done = true;
    anim_item->size.x = anim_bitmap->item_width;
    anim_item->size.y = anim_bitmap->item_height;
}

static void gui_anim_items_reset(gui_anim_bitmap_t* anim_bitmap)
{
    anim_bitmap->cur_pixel = 0;
    anim_bitmap->anim_done = true;
    
    if(anim_bitmap->anim_items == NULL) return;
    
    size_t i = 0;
    
    for(; i < anim_bitmap->anim_items_count; i ++){
        gui_anim_item_reset(anim_bitmap, &anim_bitmap->anim_items[i]);
    }
}

ALWAYS_INLINE static bool gui_anim_item_done(gui_anim_bitmap_item_t* anim_item)
{
    return anim_item->done;
}

void gui_anim_bitmap_set_bitmap(gui_anim_bitmap_t* anim_bitmap, graphics_t* bitmap)
{
    if(anim_bitmap->bitmap == bitmap) return;
    
    anim_bitmap->bitmap = bitmap;
    
    gui_anim_items_update_sizes(anim_bitmap);
    
    gui_widget_repaint(GUI_WIDGET(anim_bitmap), NULL);
}

err_t gui_anim_bitmap_set_anim_items(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_items, size_t count)
{
    if(anim_items == NULL && count != 0) return E_NULL_POINTER;
    if(anim_items != NULL && count == 0) return E_INVALID_VALUE;
    
    anim_bitmap->anim_items = anim_items;
    anim_bitmap->anim_items_count = count;
    
    gui_anim_items_reset(anim_bitmap);
    
    return E_NO_ERROR;
}

void gui_anim_bitmap_set_front_color(gui_anim_bitmap_t* anim_bitmap, graphics_color_t front_color)
{
    if(anim_bitmap->front_color == front_color) return;
    
    anim_bitmap->front_color = front_color;
    
    gui_widget_repaint(GUI_WIDGET(anim_bitmap), NULL);
}

void gui_anim_bitmap_set_margin(gui_anim_bitmap_t* anim_bitmap, graphics_size_t margin)
{
    if(anim_bitmap->margin == margin) return;
    
    anim_bitmap->margin = margin;
    
    gui_widget_repaint(GUI_WIDGET(anim_bitmap), NULL);
}

static gui_anim_bitmap_item_t* gui_anim_get_free_item(gui_anim_bitmap_t* anim_bitmap)
{
    if(anim_bitmap->anim_items == NULL) return NULL;
    
    size_t i = 0;
    
    for(; i < anim_bitmap->anim_items_count; i ++){
        if(gui_anim_item_done(&anim_bitmap->anim_items[i])){
            return &anim_bitmap->anim_items[i];
        }
    }
    return NULL;
}

static bool anim_bitmap_cur_is_on(gui_anim_bitmap_t* anim_bitmap,
                graphics_pos_t x, graphics_pos_t y)
{
    if(anim_bitmap->bitmap == NULL) return false;
    return graphics_get_pixel(anim_bitmap->bitmap, x, y) != 0;
}

/*static bool anim_bitmap_new_is_on(gui_anim_bitmap_t* anim_bitmap,
                graphics_pos_t x, graphics_pos_t y)
{
    if(anim_bitmap->target_bitmap == NULL) return false;
    return graphics_get_pixel(anim_bitmap->target_bitmap, x, y) != 0;
}*/

static bool anim_bitmap_is_changed_to_on(gui_anim_bitmap_t* anim_bitmap,
                graphics_pos_t x, graphics_pos_t y)
{
    if(anim_bitmap->bitmap == NULL || anim_bitmap->target_bitmap == NULL) return false;
    return (graphics_get_pixel(anim_bitmap->bitmap, x, y) == 0) &&
           (graphics_get_pixel(anim_bitmap->target_bitmap, x, y) != 0);
}

static bool anim_bitmap_is_changed_to_off(gui_anim_bitmap_t* anim_bitmap,
                graphics_pos_t x, graphics_pos_t y)
{
    if(anim_bitmap->bitmap == NULL || anim_bitmap->target_bitmap == NULL) return false;
    return (graphics_get_pixel(anim_bitmap->bitmap, x, y) != 0) &&
           (graphics_get_pixel(anim_bitmap->target_bitmap, x, y) == 0);
}

/**
 * Поиск ближайшей позиции с заданными
 * предикатом характеристиками.
 * @param anim_bitmap Анимированный битмап.
 * @param pos_x Координата X исходной позиции.
 * @param pos_y Координата Y исходной позиции.
 * @param pred Предикат.
 * @param out_x Выходная координата X найденной позиции.
 * @param out_y Выходная координата Y найденной позиции.
 * @return true, если позиция найдена, иначе false.
 */
static bool gui_anim_item_get_nearest_pos(gui_anim_bitmap_t* anim_bitmap,
                graphics_pos_t pos_x, graphics_pos_t pos_y,
                bool (*pred)(gui_anim_bitmap_t*, graphics_pos_t, graphics_pos_t),
                graphics_pos_t* out_x, graphics_pos_t* out_y)
{
    graphics_pos_t left = pos_x - 1;
    graphics_pos_t top = pos_y - 1;
    graphics_pos_t right = pos_x + 1;
    graphics_pos_t bottom = pos_y + 1;
    
    graphics_pos_t x, y;
    graphics_pos_t dx, dy;
    graphics_pos_t min_x = 0, min_y = 0;
    graphics_pos_t x_from, x_to, y_from, y_to;
    graphics_pos_t bitmap_right = graphics_width(anim_bitmap->bitmap) - 1;
    graphics_pos_t bitmap_bottom = graphics_height(anim_bitmap->bitmap) - 1;
    
    graphics_pos_t dist = 0;
    graphics_pos_t cur_min_dist = 0;
    graphics_pos_t min_dist = 0;
    
    for(;;){
        // Если все стороны за границами - выход.
        if(left < 0 && top < 0 &&
                right > bitmap_right &&
                bottom > bitmap_bottom){
            return false;
        }
            
        // Вычислим минимально возможную дистанцию на данном периметре.
        min_dist = bottom - pos_y;
        // Расстояние оценивается в квадратах.
        min_dist *= min_dist;
        // Сбросим минимальную дистанцию.
        cur_min_dist = 0;
        // Отсечём границы.
        x_from = MAX(left, 0);
        x_to = MIN(right, bitmap_right);
        y_from = MAX(top, 0);
        y_to = MIN(bottom, bitmap_bottom);
        // Верхняя граница.
        if(top >= 0){
            dy = pos_y - y_from;
            dy *= dy;
            for(x = x_from; x <= x_to; x ++){
                if(pred(anim_bitmap, x, y_from)){
                    dx = pos_x - x;
                    dist = dx * dx + dy;
                    if(dist < cur_min_dist || cur_min_dist == 0){
                        cur_min_dist = dist;
                        min_x = x;
                        min_y = y_from;
                        if(cur_min_dist == min_dist) break;
                    }
                }
            }
        }
        if(cur_min_dist == min_dist) break;
        // Нижняя граница.
        if(bottom <= bitmap_bottom){
            dy = pos_y - y_to;
            dy *= dy;
            for(x = x_from; x <= x_to; x ++){
                if(pred(anim_bitmap, x, y_to)){
                    dx = pos_x - x;
                    dist = dx * dx + dy;
                    if(dist < cur_min_dist || cur_min_dist == 0){
                        cur_min_dist = dist;
                        min_x = x;
                        min_y = y_to;
                        if(cur_min_dist == min_dist) break;
                    }
                }
            }
        }
        if(cur_min_dist == min_dist) break;
        // Уберём уже пройденные пикселы.
        y_from ++;
        y_to --;
        // Левая граница.
        if(left >= 0){
            dx = pos_x - x_from;
            dx *= dx;
            for(y = y_from; y <= y_to; y ++){
                if(pred(anim_bitmap, x_from, y)){
                    dy = pos_y - y;
                    dist = dx + dy * dy;
                    if(dist < cur_min_dist || cur_min_dist == 0){
                        cur_min_dist = dist;
                        min_x = x_from;
                        min_y = y;
                        if(cur_min_dist == min_dist) break;
                    }
                }
            }
        }
        if(cur_min_dist == min_dist) break;
        // Правая граница.
        if(right >= 0){
            dx = pos_x - x_to;
            dx *= dx;
            for(y = y_from; y <= y_to; y ++){
                if(pred(anim_bitmap, x_to, y)){
                    dy = pos_y - y;
                    dist = dx + dy * dy;
                    if(dist < cur_min_dist || cur_min_dist == 0){
                        cur_min_dist = dist;
                        min_x = x_to;
                        min_y = y;
                        if(cur_min_dist == min_dist) break;
                    }
                }
            }
        }
        //if(cur_min_dist == min_dist) break;
        if(cur_min_dist != 0) break;

        left --;
        top --;
        right ++;
        bottom ++;
    }
    *out_x = min_x;
    *out_y = min_y;
    
    return true;
}

ALWAYS_INLINE static void gui_anim_bitmap_bit_on(gui_anim_bitmap_t* anim_bitmap, graphics_pos_t x, graphics_pos_t y)
{
    graphics_set_pixel(anim_bitmap->bitmap, x, y, GUI_ANIM_BITMAP_ON);
}

ALWAYS_INLINE static void gui_anim_bitmap_bit_off(gui_anim_bitmap_t* anim_bitmap, graphics_pos_t x, graphics_pos_t y)
{
    graphics_set_pixel(anim_bitmap->bitmap, x, y, GUI_ANIM_BITMAP_OFF);
}

ALWAYS_INLINE static void gui_anim_bitmap_item_on(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_item)
{
    gui_anim_bitmap_bit_on(anim_bitmap, anim_item->dst_bitmap_pos.x, anim_item->dst_bitmap_pos.y);
}

ALWAYS_INLINE static void gui_anim_bitmap_item_off(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_item)
{
    gui_anim_bitmap_bit_off(anim_bitmap, anim_item->dst_bitmap_pos.x, anim_item->dst_bitmap_pos.y);
}

static void gui_anim_item_get_nearest_border(gui_anim_bitmap_t* anim_bitmap,
                graphics_pos_t pos_x, graphics_pos_t pos_y,
                graphics_pos_t* out_x, graphics_pos_t* out_y)
{
    if(out_x == NULL || out_y == NULL) return;
    
    graphics_pos_t res_x = 0, res_y = 0;

    graphics_pos_t x_to_right = graphics_width(anim_bitmap->bitmap) - 1 - pos_x;
    graphics_pos_t x_to_left = pos_x;
    graphics_pos_t y_to_bottom = graphics_height(anim_bitmap->bitmap) - 1 - pos_y;
    graphics_pos_t y_to_top = pos_y;
    graphics_pos_t min_x = MIN(x_to_right, x_to_left);
    graphics_pos_t min_y = MIN(y_to_bottom, y_to_top);
    
    if((min_x < min_y)){
        if(x_to_right > x_to_left){
            res_x = -1;
        }else if(x_to_right < x_to_left){
            res_x = graphics_width(anim_bitmap->bitmap);
        }else{
            res_x = (pos_x & 0x1) ? graphics_width(anim_bitmap->bitmap) : -1;
        }
        res_y = pos_y;
    }else if(min_x > min_y){
        if(y_to_bottom > y_to_top){
            res_y = -1;
        }else if(y_to_bottom < y_to_top){
            res_y = graphics_height(anim_bitmap->bitmap);
        }else{
            res_y = (pos_y & 0x1) ? graphics_height(anim_bitmap->bitmap) : -1;
        }
        res_x = pos_x;
    }else{
        if(x_to_right > x_to_left){
            res_x = -1;
        }else{
            res_x = graphics_width(anim_bitmap->bitmap);
        }
        if(y_to_bottom > y_to_top){
            res_y = -1;
        }else{
            res_y = graphics_height(anim_bitmap->bitmap);
        }
    }
    
    if(out_x) *out_x = res_x;
    if(out_y) *out_y = res_y;
}

static bool gui_anim_item_start_move(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_item,
                                        gui_anim_item_action_t action, graphics_pos_t x, graphics_pos_t y)
{
    graphics_pos_t anim_x, anim_y;
    graphics_pos_t dx, dy;
    
    anim_item->action = action;
    anim_item->done = false;
    //anim_item->delay_steps = 0;
    anim_item->effect_type = GUI_ANIM_BITMAP_EFFECT_MOVE;
    
    // pixel changed to on
    if(action == GUI_ANIM_ITEM_ADD){
        if(!gui_anim_item_get_nearest_pos(anim_bitmap, x, y, anim_bitmap_is_changed_to_off, &anim_x, &anim_y)){
            if(!gui_anim_item_get_nearest_pos(anim_bitmap, x, y, anim_bitmap_cur_is_on, &anim_x, &anim_y)){
                gui_anim_item_get_nearest_border(anim_bitmap, x, y, &anim_x, &anim_y);
            }
        }else{
            gui_anim_bitmap_bit_off(anim_bitmap, anim_x, anim_y);
        }
        anim_item->pos.x = anim_x * anim_bitmap->item_width;
        anim_item->pos.y = anim_y * anim_bitmap->item_height;

        anim_item->dst_bitmap_pos.x = x;
        anim_item->dst_bitmap_pos.y = y;

        //anim_item->effects.move.src_pos.x = anim_item->pos.x;
        //anim_item->effects.move.src_pos.y = anim_item->pos.y;
        anim_item->effects.move.dst_pos.x = x * anim_bitmap->item_width;
        anim_item->effects.move.dst_pos.y = y * anim_bitmap->item_height;
        //
    }// pixel changed to off
    else{
        if(!gui_anim_item_get_nearest_pos(anim_bitmap, x, y, anim_bitmap_is_changed_to_on, &anim_x, &anim_y)){
            if(!gui_anim_item_get_nearest_pos(anim_bitmap, x, y, anim_bitmap_cur_is_on, &anim_x, &anim_y)){
                gui_anim_item_get_nearest_border(anim_bitmap, x, y, &anim_x, &anim_y);
            }
        }
        anim_item->pos.x = x * anim_bitmap->item_width;
        anim_item->pos.y = y * anim_bitmap->item_height;

        anim_item->dst_bitmap_pos.x = anim_x;
        anim_item->dst_bitmap_pos.y = anim_y;

        //anim_item->effects.move.src_pos.x = anim_item->pos.x;
        //anim_item->effects.move.src_pos.y = anim_item->pos.y;
        anim_item->effects.move.dst_pos.x = anim_x * anim_bitmap->item_width;
        anim_item->effects.move.dst_pos.y = anim_y * anim_bitmap->item_height;
    
        gui_anim_bitmap_bit_off(anim_bitmap, x, y);
    }
    
    //dx = anim_item->effects.move.dst_pos.x - anim_item->effects.move.src_pos.x;
    //dy = anim_item->effects.move.dst_pos.y - anim_item->effects.move.src_pos.y;
    dx = anim_item->effects.move.dst_pos.x - anim_item->pos.x;
    dy = anim_item->effects.move.dst_pos.y - anim_item->pos.y;
    
    dx = ABS(dx);
    dy = ABS(dy);
    
    dx = MAX(dx, dy);
    
    if(anim_bitmap->max_steps == 0) anim_item->delta = dx;
    else anim_item->delta = dx / anim_bitmap->max_steps;
    
    if(anim_item->delta == 0) anim_item->delta = 1;
    
    return true;
}

static bool gui_anim_item_step_move(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_item)
{
    graphics_pos_t dx = anim_item->effects.move.dst_pos.x - anim_item->pos.x;//anim_item->effects.move.src_pos.x;
    graphics_pos_t dy = anim_item->effects.move.dst_pos.y - anim_item->pos.y;//anim_item->effects.move.src_pos.y;

    graphics_pos_t x_step = (dx > 0) ? 1 : -1;
    graphics_pos_t y_step = (dy > 0) ? 1 : -1;
    
    dx = ABS(dx);
    dy = ABS(dy);
    
    graphics_pos_t err = dx - dy;
    graphics_pos_t err2 = 0;
    
    size_t i = 0;
    
    for(; i < anim_item->delta; i ++){
        
        err2 = err << 1;
        
        if(err2 > -dy){
            err -= dy;
            anim_item->pos.x += x_step;
        }

        if(err2 < dx){
            err += dx;
            anim_item->pos.y += y_step;
        }

        if(anim_item->pos.x == anim_item->effects.move.dst_pos.x &&
           anim_item->pos.y == anim_item->effects.move.dst_pos.y){
            anim_item->done = true;
            if(anim_item->action == GUI_ANIM_ITEM_ADD){
                gui_anim_bitmap_item_on(anim_bitmap, anim_item);
            }
            break;
        }
    }
    
    return anim_item->done;
}

static bool gui_anim_item_start_resize(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_item,
                                        gui_anim_item_action_t action, graphics_pos_t x, graphics_pos_t y)
{
    anim_item->action = action;
    anim_item->done = false;
    //anim_item->delay_steps = 0;
    anim_item->effect_type = GUI_ANIM_BITMAP_EFFECT_RESIZE;
    
    anim_item->dst_bitmap_pos.x = x;
    anim_item->dst_bitmap_pos.y = y;
    
    anim_item->pos.x = x * anim_bitmap->item_width;
    anim_item->pos.y = y * anim_bitmap->item_height;
    
    graphics_size_t size = MAX(anim_bitmap->item_width, anim_bitmap->item_height) / 2;
    
    if(anim_bitmap->max_steps == 0) anim_item->delta = size;
    else anim_item->delta = size / anim_bitmap->max_steps;
    
    if(anim_item->delta == 0) anim_item->delta = 1;
    
    // pixel changed to on
    if(action == GUI_ANIM_ITEM_ADD){
        if(anim_bitmap->item_width & 0x1){
            anim_item->size.x = 1;
        }else{
            anim_item->size.x = 0;
        }
        anim_item->pos.x += anim_bitmap->item_width / 2;
        if(anim_bitmap->item_height & 0x1){
            anim_item->size.y = 1;
        }else{
            anim_item->size.y = 0;
        }
        anim_item->pos.y += anim_bitmap->item_height / 2;
    }// pixel changed to off
    else{
        gui_anim_bitmap_bit_off(anim_bitmap, x, y);
    }
    
    return true;
}

static bool gui_anim_item_step_resize(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_item)
{
    graphics_pos_t delta2 = anim_item->delta * 2;
    if(anim_item->action == GUI_ANIM_ITEM_ADD){
        if(anim_item->size.x < anim_bitmap->item_width){
            anim_item->size.x += delta2;
            anim_item->pos.x -= anim_item->delta;
        }
        if(anim_item->size.y < anim_bitmap->item_height){
            anim_item->size.y += delta2;
            anim_item->pos.y -= anim_item->delta;
        }
        if(anim_item->size.x >= anim_bitmap->item_width &&
           anim_item->size.y >= anim_bitmap->item_height){
            anim_item->done = true;
            gui_anim_bitmap_item_on(anim_bitmap, anim_item);
        }
    }else{
        if(anim_item->size.x > delta2){
            anim_item->size.x -= delta2;
            anim_item->pos.x += anim_item->delta;
        }
        if(anim_item->size.y > delta2){
            anim_item->size.y -= delta2;
            anim_item->pos.y += anim_item->delta;
        }
        if(anim_item->size.x <= delta2 &&
            anim_item->size.y <= delta2){
            anim_item->done = true;
        }
    }
    return anim_item->done;
}

static bool gui_anim_item_start_gravity(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_item,
                                        gui_anim_item_action_t action, graphics_pos_t x, graphics_pos_t y)
{
    graphics_pos_t dy;
    
    anim_item->action = action;
    anim_item->done = false;
    anim_item->effect_type = GUI_ANIM_BITMAP_EFFECT_GRAVITY;
    
    anim_item->dst_bitmap_pos.x = x;
    anim_item->dst_bitmap_pos.y = y;
    
    anim_item->effects.gravity.v_cur = 1;
    
    anim_item->pos.x = x * anim_bitmap->item_width;
    
    anim_bitmap->cur_pixel ++;
    
    anim_item->effects.gravity.delay_steps = anim_bitmap->cur_pixel + 1;
    
    // pixel changed to on
    if(action == GUI_ANIM_ITEM_ADD){
        anim_item->pos.y = -anim_bitmap->item_height;
        anim_item->effects.gravity.dst_y = y * anim_bitmap->item_height;
    }// pixel changed to off
    else{
        anim_item->pos.y = y * anim_bitmap->item_height;
        anim_item->effects.gravity.dst_y = anim_bitmap->item_height * graphics_height(anim_bitmap->bitmap);
        if(anim_item->effects.gravity.delay_steps == 0) gui_anim_bitmap_bit_off(anim_bitmap, x, y);
    }
    
    //dy = anim_item->effects.gravity.dst_y - anim_item->pos.y;
    dy = anim_bitmap->item_height * graphics_height(anim_bitmap->bitmap);
    
    if(anim_bitmap->max_steps == 0) anim_item->delta = dy;
    else anim_item->delta = dy / anim_bitmap->max_steps;
    
    if(anim_item->delta == 0) anim_item->delta = 1;
    
    return true;
}

static bool gui_anim_item_step_gravity(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_item)
{
    if(anim_item->done) return false;
    
    if(anim_item->effects.gravity.delay_steps != 0){
        anim_item->effects.gravity.delay_steps --;
        if(anim_item->effects.gravity.delay_steps == 0){
            if(anim_item->action == GUI_ANIM_ITEM_DEL){
                gui_anim_bitmap_item_off(anim_bitmap, anim_item);
            }
        }
        return false;
    }

    anim_item->pos.y += anim_item->effects.gravity.v_cur;
    anim_item->effects.gravity.v_cur += anim_item->delta;
    if(anim_item->pos.y >= anim_item->effects.gravity.dst_y){
        anim_item->pos.y = anim_item->effects.gravity.dst_y;
        anim_item->done = true;
        if(anim_item->action == GUI_ANIM_ITEM_ADD){
            gui_anim_bitmap_item_on(anim_bitmap, anim_item);
        }
    }
    return anim_item->done;
}

static void gui_anim_bitmap_flush_pos(gui_anim_bitmap_t* anim_bitmap, graphics_pos_t x, graphics_pos_t y)
{
    if(anim_bitmap->bitmap == NULL || anim_bitmap->target_bitmap == NULL) return;
    graphics_set_pixel(anim_bitmap->bitmap, x, y, graphics_get_pixel(anim_bitmap->target_bitmap, x, y));
}

static bool gui_anim_flush(gui_anim_bitmap_t* anim_bitmap)
{
    if(anim_bitmap->bitmap == NULL) return false;
    if(anim_bitmap->target_bitmap == NULL) return false;
    
    graphics_size_t x, y;
    
    for(y = 0; y < graphics_height(anim_bitmap->bitmap); y ++){
        for(x = 0; x < graphics_width(anim_bitmap->bitmap); x ++){
            gui_anim_bitmap_flush_pos(anim_bitmap, x, y);
        }
    }
    
    gui_anim_items_reset(anim_bitmap);
    
    return true;
}

static bool gui_anim_item_start(gui_anim_bitmap_t* anim_bitmap, gui_anim_item_action_t action, graphics_pos_t x, graphics_pos_t y)
{
    gui_anim_bitmap_item_t* anim_item = gui_anim_get_free_item(anim_bitmap);
    if(anim_item == NULL){
        gui_anim_bitmap_flush_pos(anim_bitmap, x, y);
        return false;
    }
    
    // pixel changed to on
    if(action == GUI_ANIM_ITEM_ADD){
        switch(anim_bitmap->effect_add_type){
            case GUI_ANIM_BITMAP_EFFECT_NONE:
                gui_anim_bitmap_flush_pos(anim_bitmap, x, y);
                break;
            case GUI_ANIM_BITMAP_EFFECT_MOVE:
                return gui_anim_item_start_move(anim_bitmap, anim_item, GUI_ANIM_ITEM_ADD, x, y);
            case GUI_ANIM_BITMAP_EFFECT_RESIZE:
                return gui_anim_item_start_resize(anim_bitmap, anim_item, GUI_ANIM_ITEM_ADD, x, y);
            case GUI_ANIM_BITMAP_EFFECT_GRAVITY:
                return gui_anim_item_start_gravity(anim_bitmap, anim_item, GUI_ANIM_ITEM_ADD, x, y);
        }
    }// pixel changed to off
    else{
        switch(anim_bitmap->effect_del_type){
            case GUI_ANIM_BITMAP_EFFECT_NONE:
                gui_anim_bitmap_flush_pos(anim_bitmap, x, y);
                break;
            case GUI_ANIM_BITMAP_EFFECT_MOVE:
                return gui_anim_item_start_move(anim_bitmap, anim_item, GUI_ANIM_ITEM_DEL, x, y);
            case GUI_ANIM_BITMAP_EFFECT_RESIZE:
                return gui_anim_item_start_resize(anim_bitmap, anim_item, GUI_ANIM_ITEM_DEL, x, y);
            case GUI_ANIM_BITMAP_EFFECT_GRAVITY:
                return gui_anim_item_start_gravity(anim_bitmap, anim_item, GUI_ANIM_ITEM_DEL, x, y);
        }
    }
    
    return false;
}

static bool gui_anim_item_step(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_item)
{
    switch(anim_item->effect_type){
        case GUI_ANIM_BITMAP_EFFECT_NONE:
            break;
        case GUI_ANIM_BITMAP_EFFECT_MOVE:
            return gui_anim_item_step_move(anim_bitmap, anim_item);
        case GUI_ANIM_BITMAP_EFFECT_RESIZE:
            return gui_anim_item_step_resize(anim_bitmap, anim_item);
        case GUI_ANIM_BITMAP_EFFECT_GRAVITY:
            return gui_anim_item_step_gravity(anim_bitmap, anim_item);
    }
    
    return true;
}

static bool gui_anim_start_move(gui_anim_bitmap_t* anim_bitmap)
{
    bool animated = false;
    
    graphics_size_t x, y;
    // Сперва включающиеся пикселы.
    for(y = 0; y < graphics_height(anim_bitmap->bitmap); y ++){
        for(x = 0; x < graphics_width(anim_bitmap->bitmap); x ++){
            if(anim_bitmap_is_changed_to_on(anim_bitmap, x, y)){
                animated |= gui_anim_item_start(anim_bitmap, GUI_ANIM_ITEM_ADD, x, y);
            }
        }
    }
    // Потом выключающиеся пикселы.
    for(y = 0; y < graphics_height(anim_bitmap->bitmap); y ++){
        for(x = 0; x < graphics_width(anim_bitmap->bitmap); x ++){
            if(anim_bitmap_is_changed_to_off(anim_bitmap, x, y)){
                animated |= gui_anim_item_start(anim_bitmap, GUI_ANIM_ITEM_DEL, x, y);
            }
        }
    }
    
    return animated;
}

static bool gui_anim_start_resize(gui_anim_bitmap_t* anim_bitmap)
{
    bool animated = false;
    
    graphics_size_t x, y;
    
    for(y = 0; y < graphics_height(anim_bitmap->bitmap); y ++){
        for(x = 0; x < graphics_width(anim_bitmap->bitmap); x ++){
            if(anim_bitmap_is_changed_to_on(anim_bitmap, x, y)){
                animated |= gui_anim_item_start(anim_bitmap, GUI_ANIM_ITEM_ADD, x, y);
            }else if(anim_bitmap_is_changed_to_off(anim_bitmap, x, y)){
                animated |= gui_anim_item_start(anim_bitmap, GUI_ANIM_ITEM_DEL, x, y);
            }
        }
    }
    
    return animated;
}

static bool gui_anim_start_gravity(gui_anim_bitmap_t* anim_bitmap)
{
    if(anim_bitmap->bitmap == NULL) return false;
    if(anim_bitmap->target_bitmap == NULL) return false;
    
    bool animated = false;
    
    graphics_size_t x, y, y_inv;
    // Сперва выключающиеся пикселы.
    for(y = 0, y_inv = graphics_height(anim_bitmap->bitmap) - 1;
            y < graphics_height(anim_bitmap->bitmap); y ++, y_inv --){
        for(x = 0; x < graphics_width(anim_bitmap->bitmap); x ++){
            if(anim_bitmap_is_changed_to_off(anim_bitmap, x, y_inv)){
                animated |= gui_anim_item_start(anim_bitmap, GUI_ANIM_ITEM_DEL, x, y_inv);
            }
        }
    }
    // Потом включающиеся пикселы.
    for(y = 0, y_inv = graphics_height(anim_bitmap->bitmap) - 1;
            y < graphics_height(anim_bitmap->bitmap); y ++, y_inv --){
        for(x = 0; x < graphics_width(anim_bitmap->bitmap); x ++){
            if(anim_bitmap_is_changed_to_on(anim_bitmap, x, y_inv)){
                animated |= gui_anim_item_start(anim_bitmap, GUI_ANIM_ITEM_ADD, x, y_inv);
            }
        }
    }
    
    return animated;
}

/**
 * Запускает анимацию.
 * @param anim_bitmap Анимированный битмап.
 * @return Флаг запуска анимации.
 */
static bool gui_anim_start(gui_anim_bitmap_t* anim_bitmap)
{
    if(anim_bitmap->bitmap == NULL) return false;
    if(anim_bitmap->target_bitmap == NULL) return false;
    
    switch(anim_bitmap->effect_add_type){
        case GUI_ANIM_BITMAP_EFFECT_NONE:
            break;
        case GUI_ANIM_BITMAP_EFFECT_MOVE:
            return gui_anim_start_move(anim_bitmap);
        case GUI_ANIM_BITMAP_EFFECT_RESIZE:
            return gui_anim_start_resize(anim_bitmap);
        case GUI_ANIM_BITMAP_EFFECT_GRAVITY:
            return gui_anim_start_gravity(anim_bitmap);
    }
    
    switch(anim_bitmap->effect_del_type){
        case GUI_ANIM_BITMAP_EFFECT_NONE:
            break;
        case GUI_ANIM_BITMAP_EFFECT_MOVE:
            return gui_anim_start_move(anim_bitmap);
        case GUI_ANIM_BITMAP_EFFECT_RESIZE:
            return gui_anim_start_resize(anim_bitmap);
        case GUI_ANIM_BITMAP_EFFECT_GRAVITY:
            return gui_anim_start_gravity(anim_bitmap);
    }
    
    gui_anim_flush(anim_bitmap);
    
    return false;
}

/**
 * Анимирует очередной шаг.
 * @param anim_bitmap Анимированный битмап.
 * @return Флаг завершения анимации.
 */
static bool gui_anim_step(gui_anim_bitmap_t* anim_bitmap)
{
    if(anim_bitmap->anim_items == NULL) return true;
    
    bool done = true;
    size_t i = 0;
    
    for(; i < anim_bitmap->anim_items_count; i ++){
        if(!gui_anim_item_done(&anim_bitmap->anim_items[i])){
            done &= gui_anim_item_step(anim_bitmap, &anim_bitmap->anim_items[i]);
        }
    }
    
    return done;
}

bool gui_anim_bitmap_animation_start(gui_anim_bitmap_t* anim_bitmap)
{
    gui_anim_items_reset(anim_bitmap);
    
    anim_bitmap->anim_done = !gui_anim_start(anim_bitmap);
    
    return anim_bitmap->anim_done;
}

bool gui_anim_bitmap_animation_step(gui_anim_bitmap_t* anim_bitmap)
{
    if(anim_bitmap->anim_done) return true;
    
    anim_bitmap->anim_done = gui_anim_step(anim_bitmap);
    
    return anim_bitmap->anim_done;
}

bool gui_anim_bitmap_animation_flush(gui_anim_bitmap_t* anim_bitmap)
{
    anim_bitmap->anim_done = true;
    
    return gui_anim_flush(anim_bitmap);
}

static void gui_anim_bitmap_paint_bit(gui_anim_bitmap_t* anim_bitmap, painter_t* painter, graphics_pos_t x, graphics_pos_t y)
{
    graphics_pos_t left = x * anim_bitmap->item_width;
    graphics_pos_t top = y * anim_bitmap->item_height;
    graphics_pos_t right = (x + 1) * anim_bitmap->item_width - 1;
    graphics_pos_t bottom = (y + 1) * anim_bitmap->item_height - 1;
    
    if(graphics_get_pixel(anim_bitmap->bitmap, x, y)){
    
        if(anim_bitmap->margin){
            painter_set_brush_color(painter, gui_widget_back_color(GUI_WIDGET(anim_bitmap)));
            painter_draw_fillrect(painter, left, top, right, top + anim_bitmap->margin);
            painter_draw_fillrect(painter, left, bottom - anim_bitmap->margin, right, bottom);
            painter_draw_fillrect(painter, left, top + anim_bitmap->margin, left + anim_bitmap->margin, bottom - anim_bitmap->margin);
            painter_draw_fillrect(painter, right - anim_bitmap->margin, top + anim_bitmap->margin, right, bottom - anim_bitmap->margin);
        }
        painter_set_brush_color(painter, anim_bitmap->front_color);

        painter_draw_fillrect(painter, left + anim_bitmap->margin,
                                       top + anim_bitmap->margin,
                                       right - anim_bitmap->margin,
                                       bottom - anim_bitmap->margin);
    }else{
        painter_set_brush_color(painter, gui_widget_back_color(GUI_WIDGET(anim_bitmap)));
        painter_draw_fillrect(painter, left, top, right, bottom);
    }
}

static void gui_anim_bitmap_paint_bitmap(gui_anim_bitmap_t* anim_bitmap, painter_t* painter)
{
    if(anim_bitmap->bitmap == NULL) return;
    if(anim_bitmap->item_width == 0 || anim_bitmap->item_height == 0) return;
    
    graphics_size_t x, y;
    
    painter_set_brush(painter, PAINTER_BRUSH_SOLID);
    
    for(y = 0; y < graphics_height(anim_bitmap->bitmap); y ++){
        for(x = 0; x < graphics_width(anim_bitmap->bitmap); x ++){
            gui_anim_bitmap_paint_bit(anim_bitmap, painter, x, y);
        }
    }
}

static void gui_anim_bitmap_paint_item(gui_anim_bitmap_t* anim_bitmap, painter_t* painter, gui_anim_bitmap_item_t* anim_item)
{
    painter_draw_fillrect(painter, anim_item->pos.x + anim_bitmap->margin,
                                   anim_item->pos.y + anim_bitmap->margin,
                                   anim_item->pos.x + anim_item->size.x - 1 - anim_bitmap->margin,
                                   anim_item->pos.y + anim_item->size.y - 1 - anim_bitmap->margin);
}

static void gui_anim_bitmap_paint_items(gui_anim_bitmap_t* anim_bitmap, painter_t* painter)
{
    if(anim_bitmap->anim_items == NULL) return;
    
    size_t i = 0;
    
    painter_set_brush_color(painter, anim_bitmap->front_color);
    
    for(; i < anim_bitmap->anim_items_count; i ++){
        if(!gui_anim_item_done(&anim_bitmap->anim_items[i])){
            gui_anim_bitmap_paint_item(anim_bitmap, painter, &anim_bitmap->anim_items[i]);
        }
    }
}

void gui_anim_bitmap_on_repaint(gui_anim_bitmap_t* anim_bitmap, const rect_t* rect)
{
    if(anim_bitmap->bitmap == NULL) return;
    
    gui_theme_t* theme = gui_theme(gui_object_gui(GUI_OBJECT(anim_bitmap)));
    
    painter_t painter;
    
    gui_widget_begin_paint(GUI_WIDGET(anim_bitmap), &painter, rect);
    
    if(gui_widget_border(GUI_WIDGET(anim_bitmap)) != GUI_BORDER_NONE || gui_widget_has_focus(GUI_WIDGET(anim_bitmap))){
        painter_set_brush(&painter, PAINTER_BRUSH_NONE);
        painter_set_pen(&painter, PAINTER_PEN_SOLID);
        if(!gui_widget_has_focus(GUI_WIDGET(anim_bitmap))){
            painter_set_pen_color(&painter, theme->border_color);
        }else{
            painter_set_pen_color(&painter, theme->focus_color);
        }
        painter_draw_rect(&painter, 0, 0, gui_widget_width(GUI_WIDGET(anim_bitmap)) - 1, gui_widget_height(GUI_WIDGET(anim_bitmap)) - 1);
    }
    
    gui_anim_bitmap_paint_bitmap(anim_bitmap, &painter);
    gui_anim_bitmap_paint_items(anim_bitmap, &painter);
    
    gui_widget_end_paint(GUI_WIDGET(anim_bitmap), &painter);
}

void gui_anim_bitmap_on_resize(gui_anim_bitmap_t* anim_bitmap, graphics_size_t width, graphics_size_t height)
{
    gui_anim_items_update_sizes(anim_bitmap);
}
