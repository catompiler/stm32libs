#include "key_input.h"
#include <string.h>
#include "bits/bits.h"
#include "utils/utils.h"


//! Вычисляет размер битовой таблицы символов.
#define KEYS_TABLE_SIZE(max_key) ((max_key + 1 + 7) / 8)

//! Размер битовой таблицы символов.
#define KEYS_BITMAP_SIZE KEYS_TABLE_SIZE(KEY_MAX)


//! Тип состояния ввода.
typedef struct _KeyInput {
    keymods_t mods; //!< Модификаторы ввода.
    key_input_on_pressed_callback_t on_pressed; //!< Каллбэк нажатия клавиши.
    key_input_on_released_callback_t on_released; //!< Каллбэк отпускания клавиши.
    uint8_t keys_bitmap[KEYS_BITMAP_SIZE]; //!< Состояния клавишь.
    const key_layout_t** layouts; //!< Раскладки клавиатуры.
    size_t layouts_count; //!< Количество раскладок клавиатуры.
    size_t cur_layout; //!< Текущая раскладка клавиатуры.
} key_input_t;

//! Состояние ввода.
static key_input_t input;



err_t key_input_init(void)
{
    input.mods = 0;
    input.on_pressed = NULL;
    input.on_released = NULL;
    memset(input.keys_bitmap, 0x0, KEYS_BITMAP_SIZE);
    input.layouts = NULL;
    input.layouts_count = 0;
    input.cur_layout = 0;
    
    return E_NO_ERROR;
}

err_t key_input_pressed(key_t key)
{
    if(key > KEY_MAX) return E_INVALID_VALUE;
    
    bits_on(input.keys_bitmap, key);
    
    switch(key){
        case KEY_SHIFT:
            input.mods |= KEYMOD_SHIFT;
            break;
        case KEY_CTRL:
            input.mods |= KEYMOD_CTRL;
            break;
        case KEY_ALT:
            input.mods |= KEYMOD_ALT;
            break;
        case KEY_CAPSLOCK:
            input.mods |= KEYMOD_CAPS;
            break;
        case KEY_SCROLL:
            input.mods |= KEYMOD_SCROLL;
            break;
        case KEY_NUMLOCK:
            input.mods |= KEYMOD_NUM;
            break;
        default:
            break;
    }
    
    if(input.on_pressed) input.on_pressed(key);
    
    return E_NO_ERROR;
}

err_t key_input_released(key_t key)
{
    if(key > KEY_MAX) return E_INVALID_VALUE;
    
    bits_off(input.keys_bitmap, key);
    
    switch(key){
        case KEY_SHIFT:
            input.mods &= ~KEYMOD_SHIFT;
            break;
        case KEY_CTRL:
            input.mods &= ~KEYMOD_CTRL;
            break;
        case KEY_ALT:
            input.mods &= ~KEYMOD_ALT;
            break;
        case KEY_CAPSLOCK:
            input.mods &= ~KEYMOD_CAPS;
            break;
        case KEY_SCROLL:
            input.mods &= ~KEYMOD_SCROLL;
            break;
        case KEY_NUMLOCK:
            input.mods &= ~KEYMOD_NUM;
            break;
        default:
            break;
    }
    
    if(input.on_released) input.on_released(key);
    
    return E_NO_ERROR;
}

key_state_t key_input_key_state(key_t key)
{
    if(key > KEY_MAX) return KEY_RELEASED;
    return bits_value(input.keys_bitmap, key);
}

key_state_t key_input_mod_state(keymod_t mod)
{
    return (input.mods & mod) ? KEY_PRESSED : KEY_RELEASED;
}

keymods_t key_input_mods(void)
{
    return input.mods;
}

err_t key_input_set_layouts(const key_layout_t** layouts, size_t count)
{
    if(layouts == NULL && count != 0) return E_NULL_POINTER;
    if(layouts != NULL && count == 0) return E_INVALID_VALUE;
    
    input.layouts = layouts;
    input.layouts_count = count;
    input.cur_layout = 0;
    
    return E_NO_ERROR;
}

const key_layout_t* key_input_current_layout(void)
{
    if(input.layouts == NULL || input.layouts_count == 0) return NULL;
    
    return input.layouts[input.cur_layout];
}

const key_layout_t* key_input_layout_by_index(size_t index)
{
    if(input.layouts == NULL || index >= input.layouts_count) return NULL;
    
    return input.layouts[index];
}

size_t key_input_current_layout_index(void)
{
    return input.cur_layout;
}

err_t key_input_set_layout_by_index(size_t index)
{
    if(index >= input.cur_layout) return E_OUT_OF_RANGE;
    
    input.cur_layout = index;
    
    return E_NO_ERROR;
}

err_t key_input_set_layout_by_id(key_layout_id_t id)
{
    const key_layout_t* layout = NULL;
    size_t i = 0;
    for(; i < input.layouts_count; i ++){
        layout = input.layouts[i];
        if(layout == NULL) continue;
        if(layout->id == id){
            input.cur_layout = i;
            return E_NO_ERROR;
        }
    }
    return E_INVALID_VALUE;
}

err_t key_input_set_layout_by_name(const char* name)
{
    const key_layout_t* layout = NULL;
    size_t i = 0;
    for(; i < input.layouts_count; i ++){
        layout = input.layouts[i];
        if(layout == NULL) continue;
        if(strcmp(layout->name, name) == 0){
            input.cur_layout = i;
            return E_NO_ERROR;
        }
    }
    return E_INVALID_VALUE;
}

err_t key_input_next_layout(void)
{
    CYCLIC_INC(input.cur_layout, 0, input.layouts_count);
}

err_t key_input_prev_layout(void)
{
    CYCLIC_INC(input.cur_layout, 0, input.layouts_count);
}

wchar_t key_input_key_to_char(key_t key)
{
    if(key >= KEY_MAX) return 0;
    
    const key_layout_t* layout = key_input_current_layout();
    
    if(layout == NULL) return 0;
    
    if(key >= KEY_NUM_0 && key <= KEY_NUM_9){
        if(input.mods & KEYMOD_NUM)
            return layout->map_shifted[key];
        else
            return layout->map_normal[key];
    }
    
    if(key >= KEY_A && key <= KEY_Z){
        if(input.mods & KEYMOD_CAPS){
            if(input.mods & KEYMOD_SHIFT)
                return layout->map_normal[key];
            else
                return layout->map_shifted[key];
        }else{
            if(input.mods & KEYMOD_SHIFT)
                return layout->map_shifted[key];
            else
                return layout->map_normal[key];
        }
    }
    if(input.mods & KEYMOD_SHIFT)
        return layout->map_shifted[key];
    
    return layout->map_normal[key];
}

key_input_on_pressed_callback_t key_input_on_pressed_callback()
{
    return input.on_pressed;
}

void key_input_set_on_pressed_callback(key_input_on_pressed_callback_t callback)
{
    input.on_pressed = callback;
}

key_input_on_released_callback_t key_input_on_released_callback()
{
    return input.on_released;
}

void key_input_set_on_released_callback(key_input_on_released_callback_t callback)
{
    input.on_released = callback;
}

