/* 
 * @file rbtree.h Библиотека красно-чёрного двоичного дерева поиска.
 */

#ifndef RBTREE_H
#define RBTREE_H

#include "errors/errors.h"
#include "defs/defs.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>



// Цвет элементов дерева.
//! Чёрный.
#define RBTREE_COLOR_BLACK 0
//! Красный.
#define RBTREE_COLOR_RED 1

//! Тип цвета элемента дерева.
typedef int8_t rbtree_color_t;

//! Тип ключа двоичного дерева поиска.
typedef void* rbtree_key_t;

//! Тип значения двоичного дерева поиска.
typedef void* rbtree_value_t;

//! Тип элемента двоичного дерева поиска.
typedef struct _RBTree_Node rbtree_node_t;

//! Тип двоичного дерева поиска.
typedef struct _RBTree rbtree_t;

//! Тип функции сравнения ключей двоичного дерева поиска.
typedef int (*rbtree_compare_t)(rbtree_key_t lkey, rbtree_key_t rkey);

//! Тип функции выделения памяти элемента дерева.
typedef rbtree_node_t* (*rbtree_alloc_node_t)(void);

//! Тип функции освобождения памяти элемента дерева.
typedef void (*rbtree_free_node_t)(rbtree_node_t* node);

//! Структура элемента двоичного дерева поиска.
struct _RBTree_Node {
    rbtree_node_t* parent; //!< Предок.
    rbtree_node_t* left; //!< Левый потомок.
    rbtree_node_t* right; //!< Правый потомок.
    rbtree_key_t key; //!< Ключ.
    rbtree_value_t value; //!< Значение.
    rbtree_color_t color; //!< Цвет.
};

//! Структура двоичного дерева поиска.
struct _RBTree {
    rbtree_node_t* root; //!< Корень дерева.
    size_t size; //!< Число элементов в дереве.
    rbtree_compare_t compare; //!< Функция сравнения ключей.
    rbtree_alloc_node_t alloc_node; //!< Функция выделения памяти.
    rbtree_free_node_t free_node; //!< Функция освобождения памяти.
};


/**
 * Инициализирует дерево.
 * @param tree Дерево.
 * @param compare Функция сравнения ключей элементов дерева.
 * @return Код ошибки.
 */
EXTERN err_t rbtree_init(rbtree_t* tree, rbtree_compare_t compare, rbtree_alloc_node_t alloc_node, rbtree_free_node_t free_node);

/**
 * Получает флаг отсутствия элементов в дереве.
 * @param tree Дерево.
 * @return Флаг отсутствия элементов в дереве.
 */
ALWAYS_INLINE static bool rbtree_empty(rbtree_t* tree)
{
    return tree->root == NULL;
}

/**
 * Получает количество элементов в дереве.
 * @param tree Дерево.
 * @return Количество элементов.
 */
ALWAYS_INLINE static size_t rbtree_size(rbtree_t* tree)
{
    return tree->size;
}

/**
 * Получает корневой элемент дерева.
 * @param tree Дерево.
 * @return Корневой элемент дерева.
 */
ALWAYS_INLINE static rbtree_node_t* rbtree_root(rbtree_t* tree)
{
    return tree->root;
}

/**
 * Получает следующий элемент дерева.
 * @param tree Дерево.
 * @param node Элемент дерева или NULL.
 * @return Следующий элемент или NULL.
 */
EXTERN rbtree_node_t* rbtree_successor(rbtree_t* tree, rbtree_node_t* node);

/**
 * Получает предыдущий элемент дерева.
 * @param tree Дерево.
 * @param node Элемент дерева или NULL.
 * @return Предыдущий элемент или NULL.
 */
EXTERN rbtree_node_t* rbtree_predecessor(rbtree_t* tree, rbtree_node_t* node);

/**
 * Получает первый элемент дерева.
 * @param tree Дерево.
 * @return Первый элемент дерева.
 */
ALWAYS_INLINE static rbtree_node_t* rbtree_first(rbtree_t* tree)
{
    return rbtree_successor(tree, NULL);
}

/**
 * Получает следующий элемент дерева.
 * @param tree Дерево.
 * @param node Текущий элемент дерева.
 * @return Следующий элемент дерева.
 */
ALWAYS_INLINE static rbtree_node_t* rbtree_next(rbtree_t* tree, rbtree_node_t* node)
{
    return rbtree_successor(tree, node);
}

/**
 * Получает последний элемент дерева.
 * @param tree Дерево.
 * @return Последний элемент дерева.
 */
ALWAYS_INLINE static rbtree_node_t* rbtree_last(rbtree_t* tree)
{
    return rbtree_predecessor(tree, NULL);
}

/**
 * Получает предыдущий элемент дерева.
 * @param tree Дерево.
 * @param node Текущий элемент дерева.
 * @return Предыдущий элемент дерева.
 */
ALWAYS_INLINE static rbtree_node_t* rbtree_prev(rbtree_t* tree, rbtree_node_t* node)
{
    return rbtree_predecessor(tree, node);
}

/**
 * Ищет элемент дерева по ключу.
 * @param tree Дерево.
 * @param key Ключ.
 * @return Найденныый элемент дерева, либо NULL.
 */
EXTERN rbtree_node_t* rbtree_search(rbtree_t* tree, rbtree_key_t key);

/**
 * Вставляет элемент в дерево.
 * @param tree Дерево.
 * @param node Элемент дерева.
 * @return Флаг вставки элемента.
 */
EXTERN bool rbtree_insert_node(rbtree_t* tree, rbtree_node_t* node);

/**
 * Вставляет в дерево новый элемент.
 * @param tree Дерево.
 * @param key Ключ.
 * @param value Значение.
 * @return Флаг вставки элемента.
 */
EXTERN bool rbtree_insert(rbtree_t* tree, rbtree_key_t key, rbtree_value_t value);

/**
 * Удаляет элемент дерева.
 * @param tree Дерево.
 * @param node Элемент дерева.
 * @return Флаг удаления элемента.
 */
EXTERN bool rbtree_remove_node(rbtree_t* tree, rbtree_node_t* node);

/**
 * Удаляет элемент из дерева.
 * @param tree Дерево.
 * @param key Ключ.
 * @return Флаг удаления элемента.
 */
EXTERN bool rbtree_remove(rbtree_t* tree, rbtree_key_t key);

/**
 * Очищает дерево.
 * Удаляет все элементы без балансировки.
 * @param tree Дерево.
 */
EXTERN void rbtree_clear(rbtree_t* tree);

/**
 * Инициализирует элемент дерева.
 * @param node Элемент дерева.
 * @param key Ключ.
 * @param value Значение.
 * @return Код ошибки.
 */
EXTERN void rbtree_node_init(rbtree_node_t* node, rbtree_key_t key, rbtree_value_t value);

/**
 * Получает родителя элемента дерева.
 * @param node Элемент дерева.
 * @return Родитель элемента дерева.
 */
ALWAYS_INLINE static rbtree_node_t* rbtree_node_parent(rbtree_node_t* node)
{
    return node->parent;
}

/**
 * Получает левого потомка элемента дерева.
 * @param node Элемент дерева.
 * @return Левый потомок элемента дерева.
 */
ALWAYS_INLINE static rbtree_node_t* rbtree_node_left_child(rbtree_node_t* node)
{
    return node->left;
}

/**
 * Получает правого потомка элемента дерева.
 * @param node Элемент дерева.
 * @return Правый потомок элемента дерева.
 */
ALWAYS_INLINE static rbtree_node_t* rbtree_node_right_child(rbtree_node_t* node)
{
    return node->right;
}

/**
 * Получает ключ элемента дерева.
 * @param node Элемент дерева.
 * @return Ключ элемента дерева.
 */
ALWAYS_INLINE static rbtree_key_t rbtree_node_key(rbtree_node_t* node)
{
    return node->key;
}

/**
 * Получает значение элемента дерева.
 * @param node Элемент дерева.
 * @return Значение элемента дерева.
 */
ALWAYS_INLINE static rbtree_value_t rbtree_node_value(rbtree_node_t* node)
{
    return node->value;
}

/**
 * Устанавливает значение элемента дерева.
 * @param node Элемент дерева.
 * @param value Значение элемента дерева.
 */
ALWAYS_INLINE static void rbtree_node_set_value(rbtree_node_t* node, rbtree_value_t value)
{
    node->value = value;
}

#endif /* RBTREE_H */
