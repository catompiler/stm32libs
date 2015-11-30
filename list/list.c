#include "list.h"
#include <stddef.h>
#include <malloc.h>
#include "utils/utils.h"



ALWAYS_INLINE static void list_item_link(list_item_t* prev, list_item_t* next)
{
    prev->next = next;
    next->prev = prev;
}

ALWAYS_INLINE static void list_item_link_safe(list_item_t* prev, list_item_t* next)
{
    if(prev) prev->next = next;
    if(next) next->prev = prev;
}

ALWAYS_INLINE static list_item_t* list_alloc_item(void* data)
{
    list_item_t* res = (list_item_t*)malloc(sizeof(list_item_t));
    
    list_item_init_data(res, data);
    
    return res;
}

ALWAYS_INLINE static void list_free_item(list_item_t* item)
{
    free(item);
}

err_t list_init(list_t* list)
{
    list->head = NULL;
    list->tail = NULL;
    return E_NO_ERROR;
}

err_t list_append(list_t* list, list_item_t* item)
{
    if(item == NULL) return E_NULL_POINTER;
    
    if(list->tail == NULL){
        list->head = item;
        list->tail = item;
        return E_NO_ERROR;
    }
    
    list_item_link(list->tail, item);
    
    list->tail = item;
    item->next = NULL;
    
    return E_NO_ERROR;
}

err_t list_prepend(list_t* list, list_item_t* item)
{
    if(list->head == NULL){
        list->head = item;
        list->tail = item;
        return E_NO_ERROR;
    }
    
    list_item_link(item, list->head);
    
    list->head = item;
    item->prev = NULL;
    
    return E_NO_ERROR;
}

err_t list_remove(list_t* list, list_item_t* item)
{
    if(item == NULL) return E_NULL_POINTER;
    if(list->head == NULL || list->tail == NULL) return E_INVALID_VALUE;
    
    if(item == list->head){
        list->head = list->head->next;
    }
    
    if(item == list->tail){
        list->tail = list->tail->prev;
    }
    
    list_item_link_safe(item->prev, item->next);
    
    return E_NO_ERROR;
}

list_item_t* list_append_new(list_t* list, void* data)
{
    list_item_t* item = list_alloc_item(data);
    
    list_append(list, item);
    
    return item;
}

list_item_t* list_prepend_new(list_t* list, void* data)
{
    list_item_t* item = list_alloc_item(data);
    
    list_prepend(list, item);
    
    return item;
}

err_t list_delete(list_t* list, list_item_t* item)
{
    RETURN_ERR_IF_FAIL(list_remove(list, item));
    list_free_item(item);
    
    return E_NO_ERROR;
}

list_item_t* list_find(list_t* list, void* data)
{
    list_item_t* item = list_first(list);
    
    while(item){
        if(list_item_data(item) == data) return item;
        
        item = list_next(item);
    }
    
    return item;
}

bool list_contains(list_t* list, list_item_t* item)
{
    list_item_t* iter_item = list_first(list);
    
    while(iter_item){
        if(iter_item == item) return true;
        
        iter_item = list_next(iter_item);
    }
    
    return false;
}

void list_foreach(list_t* list, void (*proc)(void* item_data))
{
    list_item_t* item = list_first(list);
    
    while(item){
        proc(list_item_data(item));
        
        item = list_next(item);
    }
}

void list_foreach2_first(list_t* list, void (*proc)(void* user_data, void* item_data), void* user_data)
{
    list_item_t* item = list_first(list);
    
    while(item){
        proc(user_data, list_item_data(item));
        
        item = list_next(item);
    }
}

void list_foreach2_second(list_t* list, void (*proc)(void* item_data, void* user_data), void* user_data)
{
    list_item_t* item = list_first(list);
    
    while(item){
        proc(list_item_data(item), user_data);
        
        item = list_next(item);
    }
}

void list_rforeach(list_t* list, void (*proc)(void* item_data))
{
    list_item_t* item = list_last(list);
    
    while(item){
        proc(list_item_data(item));
        
        item = list_prev(item);
    }
}

void list_rforeach2_first(list_t* list, void (*proc)(void* user_data, void* item_data), void* user_data)
{
    list_item_t* item = list_last(list);
    
    while(item){
        proc(user_data, list_item_data(item));
        
        item = list_prev(item);
    }
}

void list_rforeach2_second(list_t* list, void (*proc)(void* item_data, void* user_data), void* user_data)
{
    list_item_t* item = list_last(list);
    
    while(item){
        proc(list_item_data(item), user_data);
        
        item = list_prev(item);
    }
}

size_t list_size(list_t* list)
{
    size_t size = 0;
    list_item_t* item = list_first(list);
    
    while(item){
        size ++;
        
        item = list_next(item);
    }
    
    return size;
}

err_t list_item_init(list_item_t* item)
{
    item->data = NULL;
    item->prev = NULL;
    item->next = NULL;
    
    return E_NO_ERROR;
}

err_t list_item_init_data(list_item_t* item, void* data)
{
    item->data = data;
    item->prev = NULL;
    item->next = NULL;
    
    return E_NO_ERROR;
}
