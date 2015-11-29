#include "gui_object.h"
#include "utils/utils.h"

err_t gui_object_init(gui_object_t* object, gui_t* gui)
{
    if(gui == NULL) return E_NULL_POINTER;
    
    RETURN_ERR_IF_FAIL(list_init(&object->childs));
    RETURN_ERR_IF_FAIL(list_item_init_data(&object->child, object));
    
    object->gui = gui;
    object->parent = NULL;
    
    return E_NO_ERROR;
}

err_t gui_object_init_parent(gui_object_t* object, gui_t* gui, gui_object_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_object_init(object, gui));
    
    gui_object_set_parent(object, parent);
    
    return E_NO_ERROR;
}

static bool gui_object_add_child_impl(gui_object_t* object, gui_object_t* child)
{
    if(!list_contains(&object->childs, &child->child)){
        list_append(&object->childs, &child->child);
        return true;
    }
    return false;
}

static bool gui_object_remove_child_impl(gui_object_t* object, gui_object_t* child)
{
    if(list_contains(&object->childs, &child->child)){
        list_remove(&object->childs, &child->child);
        return true;
    }
    return false;
}

static bool gui_object_set_parent_impl(gui_object_t* object, gui_object_t* parent)
{
    if(object->parent == parent) return false;
    
    if(object->parent) gui_object_remove_child_impl(object->parent, object);
    
    object->parent = parent;
    
    return true;
}

void gui_object_set_parent(gui_object_t* object, gui_object_t* parent)
{
    if(!gui_object_set_parent_impl(object, parent)) return;
    
    if(parent) gui_object_add_child_impl(parent, object);
}

void gui_object_add_child(gui_object_t* object, gui_object_t* child)
{
    if(gui_object_add_child_impl(object, child)){
        gui_object_set_parent_impl(child, object);
    }
}

void gui_object_remove_child(gui_object_t* object, gui_object_t* child)
{
    if(gui_object_remove_child_impl(object, child)){
        gui_object_set_parent_impl(child, NULL);
    }
}

