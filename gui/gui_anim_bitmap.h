/**
 * @file gui_anim_bitmap.h Реализация виджета анимированного битмапа графического интерфейса.
 */

#ifndef GUI_ANIM_BITMAP_H
#define	GUI_ANIM_BITMAP_H

#include "gui.h"
#include "gui_widget.h"
#include "graphics/graphics.h"
#include "graphics/point.h"
#include "errors/errors.h"
#include "defs/defs.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


//! Тип анимации.
typedef enum _Gui_Anim_Bitmap_Effect {
    GUI_ANIM_BITMAP_EFFECT_NONE = 0, //!< Нет эффекта.
    GUI_ANIM_BITMAP_EFFECT_MOVE, //!< Перемещение.
    GUI_ANIM_BITMAP_EFFECT_RESIZE, //!< Изменение размера.
    GUI_ANIM_BITMAP_EFFECT_GRAVITY //!< Гравитация.
} gui_anim_bitmap_effect_t;

//! Тип действия для элемента.
typedef enum _Gui_Anim_Item_Action {
    GUI_ANIM_ITEM_ADD = 0, //! Добавление.
    GUI_ANIM_ITEM_DEL //! Удаление.
} gui_anim_item_action_t;

//! Анимированный элемент битмапа.
typedef struct _Gui_Anim_Bitmap_Item {
    point_t pos; //!< Текущая позиция.
    point_t size; //!< Текущий размер.
    gui_anim_item_action_t action; //!< Действие для элемента.
    point_t dst_bitmap_pos; //!< Позиция пиксела в битмапе.
    graphics_pos_t delta; //!< Величина изменения за шаг.
    union _Effects {
        struct _Move {
            //point_t src_pos; //!< Исходная позиция.
            point_t dst_pos; //!< Целевая позиция.
            //graphics_pos_t err; //!< Ошибка для алгоритма Брезенхема.
        } move;
        struct _Resize {
            graphics_size_t size; //!< Текущий размер.
            graphics_size_t dst_size; //!< Целевой размер.
        } resize;
        struct _Gravity {
            size_t delay_steps; //!< Задержка анимации элемента.
            graphics_pos_t dst_y; //!< Целевая позиция.
            graphics_size_t v_cur; //!< Текущая скорость падения.
        } gravity;
    } effects;
    gui_anim_bitmap_effect_t effect_type; //!< Тип эффекта.
    bool done; //!< Флаг окончания анимирования.
} gui_anim_bitmap_item_t;

typedef struct _Gui_Anim_Bitmap gui_anim_bitmap_t;

//! Зеначение идентификатора типа анимированного битмапа.
#define GUI_ANIM_BITMAP_TYPE_ID 7

struct _Gui_Anim_Bitmap {
    gui_widget_t super; //!< Суперкласс.
    graphics_color_t front_color; //!< Цвет элементов.
    graphics_size_t margin; //!< Отступ.
    graphics_t* bitmap; //!< Отображаемый битмап.
    const graphics_t* target_bitmap; //!< Целевой битмап для анимации.
    /**
     * Анимированные элементы.
     * Количество должно быть равно числу пикселов отображаемого битмапа.
     */
    gui_anim_bitmap_item_t* anim_items;
    size_t anim_items_count; //!< Число анимированных элементов.
    gui_anim_bitmap_effect_t effect_add_type; //!< Тип эффекта добавления элемента.
    gui_anim_bitmap_effect_t effect_del_type; //!< Тип эффекта удаления элемента.
    bool anim_done; //!< Флаг завершения анимации.
    graphics_size_t item_width; //!< Ширина элемента.
    graphics_size_t item_height; //!< Высота элемента.
    size_t cur_pixel; //!< Текущий анимируемый пиксел битмапа.
    size_t max_steps; //!< Максимум шагов анимации.
};

//! Приводит указатель anim_bitmap к типу анимированного битмапа.
#define GUI_ANIM_BITMAP(anim_bitmap) ((gui_anim_bitmap_t*)(anim_bitmap))


/**
 * Инициализирует анимированный битмап.
 * @param anim_bitmap Анимированный битмап.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
extern err_t gui_anim_bitmap_init(gui_anim_bitmap_t* anim_bitmap, gui_t* gui);

/**
 * Инициализирует анимированный битмап как потомок заданного родителя.
 * @param anim_bitmap Анимированный битмап.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
extern err_t gui_anim_bitmap_init_parent(gui_anim_bitmap_t* anim_bitmap, gui_t* gui, gui_widget_t* parent);

/**
 * Получает битмап анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Битмап анимированного битмапа.
 */
ALWAYS_INLINE static graphics_t* gui_anim_bitmap_bitmap(gui_anim_bitmap_t* anim_bitmap)
{
    return anim_bitmap->bitmap;
}

/**
 * Устанавливает битмап анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @param bitmap Битмап анимированного битмапа.
 */
extern void gui_anim_bitmap_set_bitmap(gui_anim_bitmap_t* anim_bitmap, graphics_t* bitmap);

/**
 * Получает целевой битмап анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Целевой битмап анимированного битмапа.
 */
ALWAYS_INLINE static const graphics_t* gui_anim_bitmap_target_bitmap(gui_anim_bitmap_t* anim_bitmap)
{
    return anim_bitmap->target_bitmap;
}

/**
 * Устанавливает целевой битмап анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @param bitmap Целевой битмап анимированного битмапа.
 */
ALWAYS_INLINE static void gui_anim_bitmap_set_target_bitmap(gui_anim_bitmap_t* anim_bitmap, const graphics_t* bitmap)
{
    anim_bitmap->target_bitmap = bitmap;
}

/**
 * Получает анимированные элементы анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Анимированные элементы анимированного битмапа.
 */
ALWAYS_INLINE static gui_anim_bitmap_item_t* gui_anim_bitmap_anim_items(gui_anim_bitmap_t* anim_bitmap)
{
    return anim_bitmap->anim_items;
}

/**
 * Устанавливает анимированные элементы анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @param anim_items Анимированные элементы анимированного битмапа.
 * @param count Число анимированных элементов.
 * @return Код ошибки.
 */
extern err_t gui_anim_bitmap_set_anim_items(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_item_t* anim_items, size_t count);

/**
 * Получает эффект добавления элемента анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Эффект добавления элемента анимированного битмапа.
 */
ALWAYS_INLINE static gui_anim_bitmap_effect_t gui_anim_bitmap_effect_add_type(gui_anim_bitmap_t* anim_bitmap)
{
    return anim_bitmap->effect_add_type;
}

/**
 * Устанавливает эффект добавления элемента анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @param effect Эффект добавления элемента анимированного битмапа.
 */
ALWAYS_INLINE static void gui_anim_bitmap_set_effect_add_type(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_effect_t effect)
{
    anim_bitmap->effect_add_type = effect;
}

/**
 * Получает эффект удаления элемента анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Эффект удаления элемента анимированного битмапа.
 */
ALWAYS_INLINE static gui_anim_bitmap_effect_t gui_anim_bitmap_effect_del_type(gui_anim_bitmap_t* anim_bitmap)
{
    return anim_bitmap->effect_del_type;
}

/**
 * Устанавливает эффект удаления элемента анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @param effect Эффект удаления элемента анимированного битмапа.
 */
ALWAYS_INLINE static void gui_anim_bitmap_set_effect_del_type(gui_anim_bitmap_t* anim_bitmap, gui_anim_bitmap_effect_t effect)
{
    anim_bitmap->effect_del_type = effect;
}

/**
 * Получает цвет элементов анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Цвет элементов анимированного битмапа.
 */
ALWAYS_INLINE static graphics_color_t gui_anim_bitmap_front_color(gui_anim_bitmap_t* anim_bitmap)
{
    return anim_bitmap->front_color;
}

/**
 * Устанавливает цвет элементов анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @param front_color Цвет элементов анимированного битмапа.
 */
extern void gui_anim_bitmap_set_front_color(gui_anim_bitmap_t* anim_bitmap, graphics_color_t front_color);

/**
 * Получает цвет фона анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Цвет фона анимированного битмапа.
 */
ALWAYS_INLINE static graphics_color_t gui_anim_bitmap_back_color(gui_anim_bitmap_t* anim_bitmap)
{
    return gui_widget_back_color(GUI_WIDGET(anim_bitmap));
}

/**
 * Устанавливает цвет фона анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @param back_color Цвет фона анимированного битмапа.
 */
ALWAYS_INLINE static void gui_anim_bitmap_set_back_color(gui_anim_bitmap_t* anim_bitmap, graphics_color_t back_color)
{
    gui_widget_set_back_color(GUI_WIDGET(anim_bitmap), back_color);
}

/**
 * Получает максимальное число кадров анимации анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Максимальное число кадров анимации анимированного битмапа.
 */
ALWAYS_INLINE static size_t gui_anim_bitmap_max_steps(gui_anim_bitmap_t* anim_bitmap)
{
    return anim_bitmap->max_steps;
}

/**
 * Устанавливает максимальное число кадров анимации анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @param max_steps Максимальное число кадров анимации анимированного битмапа.
 */
ALWAYS_INLINE static void gui_anim_bitmap_set_max_steps(gui_anim_bitmap_t* anim_bitmap, size_t max_steps)
{
    anim_bitmap->max_steps = max_steps;
}

/**
 * Получает отступ анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Отступ анимированного битмапа.
 */
ALWAYS_INLINE static graphics_size_t gui_anim_bitmap_margin(gui_anim_bitmap_t* anim_bitmap)
{
    return anim_bitmap->margin;
}

/**
 * Устанавливает отступ анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @param margin Отступ анимированного битмапа.
 */
extern void gui_anim_bitmap_set_margin(gui_anim_bitmap_t* anim_bitmap, graphics_size_t margin);

/**
 * Начинает анимацию анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Флаг завершения анимации.
 */
extern bool gui_anim_bitmap_animation_start(gui_anim_bitmap_t* anim_bitmap);

/**
 * Продолжает анимацию анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Флаг завершения анимации.
 */
extern bool gui_anim_bitmap_animation_step(gui_anim_bitmap_t* anim_bitmap);

/**
 * Завершает анимацию анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Флаг завершения анимации.
 */
extern bool gui_anim_bitmap_animation_flush(gui_anim_bitmap_t* anim_bitmap);

/**
 * Получает флаг завершения анимации анимированного битмапа.
 * @param anim_bitmap Анимированный битмап.
 * @return Флаг завершения анимации.
 */
ALWAYS_INLINE static bool gui_anim_bitmap_animation_done(gui_anim_bitmap_t* anim_bitmap)
{
    return anim_bitmap->anim_done;
}

/**
 * Обработчик перерисовки.
 * @param anim_bitmap Анимированный битмап.
 * @param rect Область перерисовки.
 */
extern void gui_anim_bitmap_on_repaint(gui_anim_bitmap_t* anim_bitmap, const rect_t* rect);

/**
 * Обработчик изменения размера.
 * @param anim_bitmap Анимированный битмап.
 * @param width Ширина.
 * @param height Высота.
 */
extern void gui_anim_bitmap_on_resize(gui_anim_bitmap_t* anim_bitmap, graphics_size_t width, graphics_size_t height);

#endif	/* GUI_ANIM_BITMAP_H */

