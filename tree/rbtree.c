#include "rbtree.h"



err_t rbtree_init(rbtree_t* tree, rbtree_compare_t compare, rbtree_alloc_node_t alloc_node, rbtree_free_node_t free_node)
{
    if(compare == NULL) return E_NULL_POINTER;
    
    tree->root = NULL;
    tree->size = 0;
    tree->compare = compare;
    tree->alloc_node = alloc_node;
    tree->free_node = free_node;
    
    return E_NO_ERROR;
}

void rbtree_node_init(rbtree_node_t* node, rbtree_key_t key, rbtree_value_t value)
{
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    
    node->key = key;
    node->value = value;
    
    node->color = RBTREE_COLOR_RED;
}

//! Выделяет память и инициализирует узел дерева.
static rbtree_node_t* rbtree_node_alloc(rbtree_t* tree, rbtree_key_t key, rbtree_value_t value)
{
    if(!tree->alloc_node) return NULL;
    
    rbtree_node_t* node = tree->alloc_node();
    
    if(node) rbtree_node_init(node, key, value);
    
    return node;
}

//! Освобождает память узла дерева.
static void rbtree_node_free(rbtree_t* tree, rbtree_node_t* node)
{
    if(node && tree->free_node) tree->free_node(node);
}

//! Возвращает самый левый элемент, начиная с node.
ALWAYS_INLINE static rbtree_node_t* rbtree_node_deepest_left(rbtree_node_t* node)
{
    while(node->left) node = node->left;
    return node;
}

//! Возвращает самый правый элемент, начиная с node.
ALWAYS_INLINE static rbtree_node_t* rbtree_node_deepest_right(rbtree_node_t* node)
{
    while(node->right) node = node->right;
    return node;
}

/*rbtree_node_t* rbtree_preorder_successor(rbtree_t* tree, rbtree_node_t* node)
{
    if(node == NULL){
        return tree->root;
    }
    
    if(node->left){
        return node->left;
    }else if(node->right){
        return node->right;
    }else{
        while(node->parent && (node == node->parent->right || !node->parent->right)){
            node = node->parent;
        }
        
        if(node->parent) return node->parent->right;
    }
    
    return NULL;
}*/

rbtree_node_t* rbtree_successor(rbtree_t* tree, rbtree_node_t* node)
{
    if(node == NULL){
        if(tree->root == NULL) return NULL;
        return rbtree_node_deepest_left(tree->root);
    }
    
    if(node->right){
        return rbtree_node_deepest_left(node->right);
    }else if(node->parent){
        if(node == node->parent->left) return node->parent;
        do { node = node->parent; } while(node->parent && node == node->parent->right);
        return node->parent;
    }
    
    return NULL;
}

rbtree_node_t* rbtree_predecessor(rbtree_t* tree, rbtree_node_t* node)
{
    if(node == NULL){
        if(tree->root == NULL) return NULL;
        return rbtree_node_deepest_right(tree->root);
    }
    
    if(node->left){
        return rbtree_node_deepest_right(node->left);
    }else if(node->parent){
        if(node == node->parent->right) return node->parent;
        do { node = node->parent; } while(node->parent && node == node->parent->left);
        return node->parent;
    }
    
    return NULL;
}

rbtree_node_t* rbtree_search(rbtree_t* tree, rbtree_key_t key)
{
    if(tree->root == NULL){
        return NULL;
    }
    
    rbtree_node_t* cur_node = tree->root;
    
    for(;;){
        int cmp = tree->compare(key, cur_node->key);
        
        if(cmp > 0){
            if(cur_node->right){
                cur_node = cur_node->right;
            }else{
                break;
            }
        }else if(cmp < 0){
            if(cur_node->left){
                cur_node = cur_node->left;
            }else{
                break;
            }
        }else{ // cmp == 0
            return cur_node;
        }
    }
    
    return NULL;
}

//! Получает цвет элемента.
ALWAYS_INLINE static rbtree_color_t rbtree_node_color(rbtree_node_t* node)
{
    // Листовые узлы - чёрные.
    if(node == NULL) return RBTREE_COLOR_BLACK;
    return node->color;
}

//! Получает цвет левого потомка элемента.
ALWAYS_INLINE static rbtree_color_t rbtree_node_left_child_color(rbtree_node_t* node)
{
    // Листовые узлы - чёрные.
    if(node == NULL || node->left == NULL) return RBTREE_COLOR_BLACK;
    return node->left->color;
}

//! Получает цвет правого потомка элемента.
ALWAYS_INLINE static rbtree_color_t rbtree_node_right_child_color(rbtree_node_t* node)
{
    // Листовые узлы - чёрные.
    if(node == NULL || node->right == NULL) return RBTREE_COLOR_BLACK;
    return node->right->color;
}

//! Получает флаг нахождения элемента слева.
ALWAYS_INLINE static bool rbtree_node_is_left(rbtree_node_t* parent, rbtree_node_t* node)
{
    return parent->left == node;
}

//! Получает флаг нахождения элемента справа.
ALWAYS_INLINE static bool rbtree_node_is_right(rbtree_node_t* parent, rbtree_node_t* node)
{
    return parent->right == node;
}

//! Связывает узел с родителем слева.
ALWAYS_INLINE static void rbtree_node_link_left(rbtree_node_t* parent, rbtree_node_t* node)
{
    parent->left = node;
    if(node) node->parent = parent;
}

//! Связывает узел с родителем справа.
ALWAYS_INLINE static void rbtree_node_link_right(rbtree_node_t* parent, rbtree_node_t* node)
{
    parent->right = node;
    if(node) node->parent = parent;
}

//! Заменяет потомка у родителя.
ALWAYS_INLINE static void rbtree_node_change_child(rbtree_node_t* parent, rbtree_node_t* old_child, rbtree_node_t* new_child)
{
    if(rbtree_node_is_left(parent, old_child)){
        rbtree_node_link_left(parent, new_child);
    }else /*if(rbtree_node_is_right(parent, old_child))*/{
        rbtree_node_link_right(parent, new_child);
    }
}

//! Заменяет корень дерева.
ALWAYS_INLINE static void rbtree_change_root(rbtree_t* tree, rbtree_node_t* node)
{
    tree->root = node;
    if(node) node->parent = NULL;
}

//! Заменяет потомка элемента в дереве.
static void rbtree_change_child(rbtree_t* tree, rbtree_node_t* old_child, rbtree_node_t* new_child)
{
    if(old_child->parent){
        rbtree_node_change_child(old_child->parent, old_child, new_child);
    }else{
        rbtree_change_root(tree, new_child);
    }
}

/**
 * Вырезает элемент дерева
 * с одним и менее потомком.
 * @param tree Дерево.
 * @param node Элемент дерева.
 * @return Вставленного вместо элемента потомка.
 */
ALWAYS_INLINE static rbtree_node_t* rbtree_node_cut_one_child(rbtree_t* tree, rbtree_node_t* node)
{
    rbtree_node_t* child = node->left ? node->left : node->right;
    
    rbtree_change_child(tree, node, child);
    
    return child;
}

//! Заменяет элемент в дереве.
ALWAYS_INLINE static void rbtree_replace(rbtree_t* tree, rbtree_node_t* old_node, rbtree_node_t* new_node)
{
    new_node->color = old_node->color;
    
    rbtree_node_link_left(new_node, old_node->left);
    rbtree_node_link_right(new_node, old_node->right);
    
    rbtree_change_child(tree, old_node, new_node);
}

//! Поворачивает дерево влево относительно элемента.
static void rbtree_rotate_left(rbtree_t* tree, rbtree_node_t* node)
{
    if(node == NULL) return;
    
    rbtree_node_t* right_node = node->right;
    
    if(right_node == NULL) return;
    
    rbtree_change_child(tree, node, right_node);
    
    if(right_node->left){
        rbtree_node_link_right(node, right_node->left);
    }else{
        node->right = NULL;
    }
    
    rbtree_node_link_left(right_node, node);
}

//! Поворачивает дерево вправо относительно элемента.
static void rbtree_rotate_right(rbtree_t* tree, rbtree_node_t* node)
{
    if(node == NULL) return;
    
    rbtree_node_t* left_node = node->left;
    
    if(left_node == NULL) return;
    
    rbtree_change_child(tree, node, left_node);
    
    if(left_node->right){
        rbtree_node_link_left(node, left_node->right);
    }else{
        node->left = NULL;
    }
    
    rbtree_node_link_right(left_node, node);
}

ALWAYS_INLINE static rbtree_node_t* rbtree_node_brother(rbtree_node_t* parent, rbtree_node_t* node)
{
    if(rbtree_node_is_left(parent, node)) return parent->right;
    return parent->left;
}

ALWAYS_INLINE static rbtree_node_t* rbtree_node_grandparent(rbtree_node_t* node)
{
    if(node->parent) return node->parent->parent;
    return NULL;
}

ALWAYS_INLINE static rbtree_node_t* rbtree_node_uncle(rbtree_node_t* node)
{
    rbtree_node_t* grandparent = rbtree_node_grandparent(node);
    if(grandparent){
        return rbtree_node_brother(grandparent, node->parent);
    }
    return NULL;
}

//! Балансирует дерево при вставке элемента.
static void rbtree_insert_balance_fix(rbtree_t* tree, rbtree_node_t* node)
{
    rbtree_node_t* uncle = NULL;
    rbtree_node_t* grandparent = NULL;
    rbtree_node_t* parent = NULL;
    
    do{
        // 1) Если узел - корень дерева.
        if(node->parent == NULL){
            // И его цвет - красный
            if(node->color == RBTREE_COLOR_RED){
                // Перекрашиваем его в чёрный цвет.
                node->color = RBTREE_COLOR_BLACK;
            }
            // Дерево сбалансировано.
            break;
        }// 2) Если у предка узла чёрный цвет.
        else if(rbtree_node_color(node->parent) == RBTREE_COLOR_BLACK){
            // Дерево сбалансировано.
            break;
        }// Случаи с дядей.
        else{
            // Узел node - не корень и не следующий после корня,
            // поэтому имеет ненулевого родителя и дедушку,
            // дядя может быть нулевым.
            // Предок узла красного цвета (проверено в случае 2).
            
            parent = node->parent;
            grandparent = parent->parent;
            uncle = rbtree_node_brother(grandparent, parent);
            // 3) Если предок и дядя - красные.
            if(rbtree_node_color(uncle) == RBTREE_COLOR_RED){
                
                // Значит дядя не нулевой, иначе бы он был чёрным,
                // т.к. нулевой(листовой) узел всегда чёрный.
                
                // Перекрашиваем предка и дядю в красный,
                parent->color = RBTREE_COLOR_BLACK;
                uncle->color = RBTREE_COLOR_BLACK;
                // дедушку - в чёрный.
                grandparent->color = RBTREE_COLOR_RED;
                
                // И продолжим с перекрашенного дедушки.
                node = grandparent;
            } // 4) Предок - красный, дядя - чёрный.
            else{//if(rbtree_node_color(uncle) == RBTREE_COLOR_BLACK){
                // 4.1) Узел node находится справа, предок - слева.
                if(node == parent->right && parent == grandparent->left){
                    // Повернём поддерево влево.
                    rbtree_rotate_left(tree, parent);
                    // Новый порядок в поддереве.
                    node = node->left;
                    parent = node->parent;
                    
                }// 4.2) Узел node находится слева, предок - справа.
                else if(node == parent->left && parent == grandparent->right){
                    // Повернём поддерево вправо.
                    rbtree_rotate_right(tree, parent);
                    // Новый порядок в поддереве.
                    node = node->right;
                    parent = node->parent;
                }
                
                // 5) Красный узел, его красный предок и дед на одной линии.
                // 5.1) Линия налево.
                if(node == parent->left && parent == grandparent->left){
                    // Перекрасим предка в чёрный,
                    parent->color = RBTREE_COLOR_BLACK;
                    // деда - в красный.
                    grandparent->color = RBTREE_COLOR_RED;
                    // Повернём дерево вправо.
                    rbtree_rotate_right(tree, grandparent);
                    // Дерево сбалансировано.
                    break;
                }// 5.2) Линия вправо.
                else if(node == parent->right && parent == grandparent->right){
                    // Перекрасим предка в чёрный,
                    parent->color = RBTREE_COLOR_BLACK;
                    // деда - в красный.
                    grandparent->color = RBTREE_COLOR_RED;
                    // Повернём дерево влево.
                    rbtree_rotate_left(tree, grandparent);
                    // Дерево сбалансировано.
                    break;
                }
            }
        }
    }while(node);
}

/**
 * Общая функция вставки элемента.
 * В случае если node == NULL
 * выделяет память.
 */
static bool rbtree_insert_impl(rbtree_t* tree, rbtree_key_t key, rbtree_value_t value, rbtree_node_t* node)
{
    if(tree->root){
    
        rbtree_node_t* cur_node = tree->root;

        for(;;){
            int cmp = tree->compare(key, cur_node->key);

            if(cmp > 0){
                if(cur_node->right){
                    cur_node = cur_node->right;
                }else{
                    if(!node) if(!(node = rbtree_node_alloc(tree, key, value))) return false;
                    rbtree_node_link_right(cur_node, node);
                    break;
                }
            }else if(cmp < 0){
                if(cur_node->left){
                    cur_node = cur_node->left;
                }else{
                    if(!node) if(!(node = rbtree_node_alloc(tree, key, value))) return false;
                    rbtree_node_link_left(cur_node, node);
                    break;
                }
            }else{ // cmp == 0
                return false;
            }
        }
    }else{
        if(!node) if(!(node = rbtree_node_alloc(tree, key, value))) return false;
        
        tree->root = node;
    }
    
    node->color = RBTREE_COLOR_RED;
    
    rbtree_insert_balance_fix(tree, node);
    
    tree->size ++;
    
    return true;
}

bool rbtree_insert_node(rbtree_t* tree, rbtree_node_t* node)
{
    if(node == NULL) return false;
    
    return rbtree_insert_impl(tree, node->key, node->value, node);
}

bool rbtree_insert(rbtree_t* tree, rbtree_key_t key, rbtree_value_t value)
{
    return rbtree_insert_impl(tree, key, value, NULL);
}

//! Балансирует дерево при удалении элемента.
static void rbtree_remove_balance_fix(rbtree_t* tree, rbtree_node_t* parent, rbtree_node_t* node)
{
    rbtree_node_t* brother = NULL;
    
    for(;;){
        // 1) Если удаляется чёрный корень.
        if(parent == NULL){
            // Дерево сбалансировано.
            return;
        }
        brother = rbtree_node_brother(parent, node);
        
        // 2) Если брат узла красный.
        if(rbtree_node_color(brother) == RBTREE_COLOR_RED){
            // Перекрашиваем
            // предка в красный цвет
            parent->color = RBTREE_COLOR_RED;
            // и брата в чёрный цвет.
            brother->color = RBTREE_COLOR_BLACK;
            // И вращаем дерево в сторону узла.
            if(rbtree_node_is_left(parent, node)){
                rbtree_rotate_left(tree, parent);
            }else{
                rbtree_rotate_right(tree, parent);
            }
            // Новый брат узла.
            brother = rbtree_node_brother(parent, node);
        }
        
        // 3) Предок, брат и его потомки - чёрные.
        if(rbtree_node_color(parent) == RBTREE_COLOR_BLACK &&
           /*rbtree_node_color(brother) == RBTREE_COLOR_BLACK &&*/ // Брат либо был чёрным, либо стал чёрным в п.2.
           rbtree_node_left_child_color(brother) == RBTREE_COLOR_BLACK &&
           rbtree_node_right_child_color(brother) == RBTREE_COLOR_BLACK){
            // Перекрашиваем брата в красный.
            if(brother) brother->color = RBTREE_COLOR_RED;
            // Переходим к балансировке на уровень выше.
            node = parent;
            parent = parent->parent;
        }
        // Балансировка верхних элементов не требуется.
        else{
            break;
        }
    }
    
    // Красный брат всегда перекрашивается в п.2 в чёрный цвет,
    // т.о. в п.4 и п.5 его проверка на цвет не нужна.
    
    // 4) Предок - красный, брат и его потомки - чёрные.
    if(rbtree_node_color(parent) == RBTREE_COLOR_RED &&
       /*rbtree_node_color(brother) == RBTREE_COLOR_BLACK &&*/ // Брат в любом случае чёрный.
       rbtree_node_left_child_color(brother) == RBTREE_COLOR_BLACK &&
       rbtree_node_right_child_color(brother) == RBTREE_COLOR_BLACK){
        // Перекрашиваем
        // предка в чёрный цвет
        parent->color = RBTREE_COLOR_BLACK;
        // и брата в красный цвет.
        if(brother) brother->color = RBTREE_COLOR_RED;
        // Дерево сбалансировано.
        return;
    }
    
    // 5) Брат чёрный и у него есть красный потомок.
    //if(rbtree_node_color(brother) == RBTREE_COLOR_BLACK){ // Брат в любом случае чёрный.
    // 5.1) Если узел - левый потомок предка
    // и левый потомк брата красный
    // и правый потомок брата чёрный.
    if(rbtree_node_is_left(parent, node) &&
       rbtree_node_left_child_color(brother) == RBTREE_COLOR_RED &&
       rbtree_node_right_child_color(brother) == RBTREE_COLOR_BLACK){
        // Перекрашиваем
        // брата в красный цвет
        brother->color = RBTREE_COLOR_RED;
        // и левого потомка брата в чёрный цвет.
        brother->left->color = RBTREE_COLOR_BLACK;
        // Вращаем дерево вправо относительно брата.
        rbtree_rotate_right(tree, brother);
        // Новый брат узла.
        brother = rbtree_node_brother(parent, node);
    }
    // 5.2) Если узел - правый потомок предка
    // и левый потомк брата чёрный
    // и правый потомок брата красный.
    else if(rbtree_node_is_right(parent, node) &&
            rbtree_node_left_child_color(brother) == RBTREE_COLOR_BLACK &&
            rbtree_node_right_child_color(brother) == RBTREE_COLOR_RED){
        // Перекрашиваем
        // брата в красный цвет
        brother->color = RBTREE_COLOR_RED;
        // и правого потомка брата в чёрный цвет.
        brother->right->color = RBTREE_COLOR_BLACK;
        // Вращаем дерево влево относительно брата.
        rbtree_rotate_left(tree, brother);
        // Новый брат узла.
        brother = rbtree_node_brother(parent, node);
    }
    
    // В п.5 чёрный брат либо не замещается,
    // либо происходит его замена при вращении
    // на другой чёрный узел.
    
    // В случае отсутствия у брата красных потомков,
    // будет выполняться п.3, т.о. у брата в этом месте
    // гарантированно есть красный потомок.
    // п. 5 так же переносит красного потомка брата в нужное положение.
    
    // Перекрашиваем брата в цвет предка,
    brother->color = parent->color;
    // предка в чёрный цвет
    parent->color = RBTREE_COLOR_BLACK;
    // 6) Брат - чёрный.
    //if(rbtree_node_color(brother) == RBTREE_COLOR_BLACK){ // Брат в любом случае чёрный.
    // 6.1) Узел - левый потомок предка
    // и правый потомок брата - красный.
    if(rbtree_node_is_left(parent, node) //&&
       /*rbtree_node_right_child_color(brother) == RBTREE_COLOR_RED*/){
        // Перекрашиваем брата в цвет предка,
        //brother->color = parent->color;
        // предка в чёрный цвет
        //parent->color = RBTREE_COLOR_BLACK;
        // и правого потомка брата в чёрный цвет.
        brother->right->color = RBTREE_COLOR_BLACK;
        // Вращаем дерево влево относительно предка.
        rbtree_rotate_left(tree, parent);
    }
    // 6.2) Узел - правый потомок предка
    // и левый потомок брата - красный.
    else if(rbtree_node_is_right(parent, node) //&&
            /*rbtree_node_left_child_color(brother) == RBTREE_COLOR_RED*/){
        // Перекрашиваем брата в цвет предка,
        //brother->color = parent->color;
        // предка в чёрный цвет
        //parent->color = RBTREE_COLOR_BLACK;
        // и левого потомка брата в чёрный цвет.
        brother->left->color = RBTREE_COLOR_BLACK;
        // Вращаем дерево вправо относительно предка.
        rbtree_rotate_right(tree, parent);
    }
}

//! Удаляет элемент с не более чем одним потомком.
static void rbtree_remove_node_one_child(rbtree_t* tree, rbtree_node_t* node)
{
    rbtree_node_t* child = rbtree_node_cut_one_child(tree, node);
    
    // Балансировка дерева нужна лишь при удалении чёрного узла.
    if(rbtree_node_color(node) == RBTREE_COLOR_BLACK){
        // Если замещаемый узел - красный.
        if(rbtree_node_color(child) == RBTREE_COLOR_RED){
            // Просто перекрасим его в чёрный.
            child->color = RBTREE_COLOR_BLACK;
        }// Оба узла чёрные - балансируем дерево.
        else{
            rbtree_remove_balance_fix(tree, node->parent, child);
        }
    }
}

bool rbtree_remove_node(rbtree_t* tree, rbtree_node_t* node)
{
    if(node == NULL) return false;
    
    if(node->left && node->right){
        rbtree_node_t* deepest = rbtree_node_deepest_left(node->right);
        
        rbtree_remove_node_one_child(tree, deepest);
        rbtree_replace(tree, node, deepest);
    }else{
        rbtree_remove_node_one_child(tree, node);
    }
    
    rbtree_node_free(tree, node);
    
    tree->size --;
    
    return true;
}

bool rbtree_remove(rbtree_t* tree, rbtree_key_t key)
{
    rbtree_node_t* node = rbtree_search(tree, key);
    
    return rbtree_remove_node(tree, node);
}

//! Находит узел без потомков.
static rbtree_node_t* rbtree_node_find_end(rbtree_node_t* node)
{
    for(;;){
        while(node->left) node = node->left;
        if(!node->right) break;
        node = node->right;
    }
    return node;
}

//! Удаляет узел без потомков и освободжает память.
static void rbtree_remove_end_node(rbtree_t* tree, rbtree_node_t* node)
{
    rbtree_change_child(tree, node, NULL);
    rbtree_node_free(tree, node);
}

void rbtree_clear(rbtree_t* tree)
{
    if(tree->root == NULL) return;
    
    rbtree_node_t* node = tree->root;
    
    rbtree_node_t* endnode = NULL;
    
    while(node){
        endnode = rbtree_node_find_end(node);
        node = endnode->parent;
        rbtree_remove_end_node(tree, endnode);
    }
    
    tree->size = 0;
}
