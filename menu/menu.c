#include "menu.h"
#include "utils/utils.h"



err_t menu_init(menu_t* menu, menu_item_t* root)
{
    if(root == NULL) return E_NULL_POINTER;
    
    menu->root = root;
    menu->current = root;
    
    return E_NO_ERROR;
}

err_t menu_make_from_descrs(menu_t* menu, menu_item_t* items, size_t items_count,
                                    const menu_descr_t* descrs, size_t descrs_count)
{
    if(descrs == NULL) return E_NULL_POINTER;
    if(items == NULL) return E_NULL_POINTER;
    if(descrs_count == 0) return E_INVALID_VALUE;
    if(items_count == 0) return E_INVALID_VALUE;
    
    if(descrs_count > items_count) descrs_count = items_count;
    
    menu_depth_t cur_depth = 0;
    menu_item_t* item = NULL;
    const menu_descr_t* descr = NULL;
    
    size_t index = 0;
    
    // Цикл по все дескрипторам.
    for(; index < descrs_count; index ++){
        // Текущий элемент меню.
        item = &items[index];
        // Текущий дескриптор.
        descr = &descrs[index];
        // Инициализирует элемент меню.
        RETURN_ERR_IF_FAIL(menu_item_init_from_descr(item, descr));
        // Если не начальный элемент (меню не пустое).
        if(index != 0){
            // Если текущая глубина не меняется (элементы одного уровня).
            if(cur_depth == descr->depth){
                // Соединим предыдущий элемент со следующим.
                menu_item_link(menu_current(menu), item);
                // Установим родителя для нового элемента.
                menu_item_set_parent(item, menu_item_parent(menu_current(menu)));
                // Перейдём на новый элемент.
                menu_next(menu);
            }else if(cur_depth < descr->depth){ // Если требуется пройти вглубь меню.
                // Добавим потомка в меню.
                menu_item_link_parent(menu_current(menu), item);
                // Пройдём вглубь меню.
                menu_down(menu);
                // Сохраним текущую глубину.
                cur_depth = descr->depth;
            }else{ // cur_depth > descr->depth - требуется пойти наружу меню.
                // Пройдём наружу до нужного уровня.
                for(;;){
                    // Если более невозможно подняться - прекращаем.
                    if(!menu_up(menu)) break;
                    // Если дошли до нужного уровня - прекращаем.
                    if(-- cur_depth <= descr->depth) break;
                }
                // Сохраним текущую глубину.
                cur_depth = descr->depth;
                // Дойдём до последнего элемента в списке.
                while(menu_next(menu));
                // Добавим новый элемент.
                menu_item_link(menu_current(menu), item);
                // Установим родителя для нового элемента.
                menu_item_set_parent(item, menu_item_parent(menu_current(menu)));
                // Перейдём на новый элемент.
                menu_next(menu);
            }
        }else{ // Если меню требуется инициализировать.
            // Инициализируем меню.
            RETURN_ERR_IF_FAIL(menu_init(menu, item));
            // Сохраним текущую глубину.
            cur_depth = descr->depth;
        }
    }
    
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
    item->user_data = NULL;
    
    return E_NO_ERROR;
}

err_t menu_item_init_from_descr(menu_item_t* item, const menu_descr_t* descr)
{
    if(descr == NULL) return E_NULL_POINTER;
    
    item->id = descr->id;
    item->parent = NULL;
    item->child = NULL;
    item->prev = NULL;
    item->next = NULL;
    item->text = descr->text;
    item->value = descr->value;
    item->flags = descr->flags;
    item->user_data = descr->user_data;
    
    return E_NO_ERROR;
}

size_t menu_item_childs_count(menu_item_t *item)
{
    size_t childs_count = 0;

    while(item->child){
        item = item->child;
        childs_count ++;
    }

    return childs_count;
}

menu_item_t* menu_item_child_at(menu_item_t* item, size_t index)
{
    return menu_item_next_at(item->child, index);
}

menu_item_t* menu_item_next_at(menu_item_t* item, size_t index)
{
    while(index > 0 && item){
        item = item->next;
        index --;
    }

    return item;
}

size_t menu_item_pos(menu_item_t* item)
{
    size_t pos = 0;

    while(item->prev){
        item = item->prev;
        pos ++;
    }

    return pos;
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

void menu_item_link_parent(menu_item_t* parent, menu_item_t* child)
{
    if(parent) parent->child = child;
    if(child)  child->parent = parent;
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
