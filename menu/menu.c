#include "menu.h"



err_t menu_init(menu_t* menu, menu_item_t* root)
{
    if(root == NULL) return E_NULL_POINTER;
    
    menu->root = root;
    menu->current = root;
    
    return E_NO_ERROR;
}

err_t menu_set_root(menu_t* menu, menu_item_t* root)
{
    if(root == NULL) return E_NULL_POINTER;
    
    menu->root = root;
    menu->current = root;
    
    return E_NO_ERROR;
}

err_t menu_set_current(menu_t* menu, menu_item_t* current)
{
    if(current == NULL) return E_NULL_POINTER;
    
    menu->current = current;
    
    return E_NO_ERROR;
}

void menu_reset_current(menu_t* menu)
{
    menu->current = menu->root;
}

bool menu_up(menu_t* menu)
{
    if(menu->current == NULL) return false;
    if(menu->current->parent == NULL) return false;
    
    menu->current = menu->current->parent;
    
    return true;
}

bool menu_down(menu_t* menu)
{
    if(menu->current == NULL) return false;
    if(menu->current->child == NULL) return false;
    
    menu->current = menu->current->child;
    
    return true;
}

bool menu_next(menu_t* menu)
{
    if(menu->current == NULL) return false;
    if(menu->current->next == NULL) return false;
    
    menu->current = menu->current->next;
    
    return true;
}

bool menu_prev(menu_t* menu)
{
    if(menu->current == NULL) return false;
    if(menu->current->prev == NULL) return false;
    
    menu->current = menu->current->prev;
    
    return true;
}

err_t menu_item_init(menu_item_t* item, const char* text)
{
    item->id = 0;
    item->parent = NULL;
    item->child = NULL;
    item->prev = NULL;
    item->next = NULL;
    item->text = text;
    item->value = NULL;
    item->flags = 0;
    
    return E_NO_ERROR;
}

menu_item_t* menu_item_first(menu_item_t* item)
{
    while(item->prev) item = item->prev;
    return item;
}

menu_item_t* menu_item_last(menu_item_t* item)
{
    while(item->next) item = item->next;
    return item;
}

void menu_item_link(menu_item_t* prev, menu_item_t* next)
{
    if(prev) prev->next = next;
    if(next) next->prev = prev;
}

err_t menu_value_init(menu_value_t* value)
{
    value->type = MENU_VALUE_TYPE_NONE;
    return E_NO_ERROR;
}

err_t menu_value_set_enum(menu_value_t* value, menu_value_enum_t* val)
{
    if(val == NULL) return E_NULL_POINTER;
    if(val->values == NULL) return E_INVALID_VALUE;
    if(val->current >= val->count) return E_OUT_OF_RANGE;
    
    value->type = MENU_VALUE_TYPE_ENUM;
    value->value_enum.count = val->count;
    value->value_enum.current = val->current;
    value->value_enum.values = val->values;
    
    return E_NO_ERROR;
}

err_t menu_value_enum_set_count(menu_value_t* value, size_t count)
{
    if(count == 0) return E_INVALID_VALUE;
    
    value->value_enum.count = count;
    if(value->value_enum.current >= count){
        value->value_enum.current = count - 1;
    }
    return E_NO_ERROR;
}

err_t menu_value_enum_set_values(menu_value_t* value, menu_value_t* values)
{
    if(values == NULL) return E_NULL_POINTER;
    
    value->value_enum.values = values;
    
    return E_NO_ERROR;
}

menu_value_t* menu_value_enum_current_value(menu_value_t* value)
{
    if(value->type != MENU_VALUE_TYPE_ENUM) return NULL;
    if(value->value_enum.values == NULL) return NULL;
    if(value->value_enum.current >= value->value_enum.count) return NULL;
    
    return &value->value_enum.values[value->value_enum.current];
}
