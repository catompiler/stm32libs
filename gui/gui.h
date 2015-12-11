/**
 * @file gui.h Библиотека работы с GUI.
 */

#ifndef GUI_H
#define	GUI_H

#include "defs/defs.h"
#include "graphics/graphics.h"
#include "graphics/font.h"
#include "graphics/rect.h"
#include "input/key_input.h"

//! Тип границы виджета.
typedef enum _Gui_Border {
    GUI_BORDER_NONE = 0, //!< Нет границы.
    GUI_BORDER_SOLID //!< Сплошная граница.
} gui_border_t;

//! Структура темы оформления GUI.
typedef struct _Gui_Theme {
    graphics_color_t back_color; //!< Цвет фона.
    graphics_color_t front_color; //!< Цвет переднего плана.
    graphics_color_t panel_color; //!< Цвет фона контейнеров.
    graphics_color_t widget_color; //!< Цвет элементов управления.
    graphics_color_t border_color; //!< Цвет границы.
    graphics_color_t font_color; //!< Цвет шрифта.
    graphics_color_t focus_color; //!< Цвет границы в фокусе.
    graphics_color_t pressed_color; //!< Цвет нажатого виджета.
    const font_t* widget_font; //!< Шрифт виджета.
    const font_t* menu_font; //!< Шрифт меню.
} gui_theme_t;

#define MAKE_GUI_THEME(arg_back_color, arg_front_color,\
                       arg_panel_color, arg_widget_color,\
                       arg_border_color, arg_font_color,\
                       arg_focus_color, arg_pressed_color,\
                       arg_widget_font, arg_menu_font)\
        { .back_color = arg_back_color, .front_color = arg_front_color,\
          .panel_color = arg_panel_color, .widget_color = arg_widget_color,\
          .border_color = arg_border_color, .font_color = arg_font_color,\
          .focus_color = arg_focus_color, .pressed_color = arg_pressed_color,\
          .widget_font = arg_widget_font, .menu_font = arg_menu_font }

#ifndef GUI_WIDGET_TYPE_DEFINED
#define GUI_WIDGET_TYPE_DEFINED
//! Тип структуры виджета.
typedef struct _Gui_Widget gui_widget_t;
#endif //GUI_WIDGET_TYPE_DEFINED

//! Структура графического интерфейса.
typedef struct _Gui {
    graphics_t* graphics; //!< Графический буфер.
    gui_theme_t* theme; //!< Тема оформления.
    gui_widget_t* root_widget; //!< Корневой виджет.
    gui_widget_t* focus_widget; //!< Виджет в фокусе.
} gui_t;

#define MAKE_GUI(arg_graphics, arg_theme)\
        { .graphics = arg_graphics, .theme = arg_theme,\
          .root_widget = NULL, .focus_widget = NULL }

/**
 * Инициализирует графический интерфейс.
 * @param gui Графический интерфейс.
 * @param graphics Графический буфер.
 * @param theme Тема графического интерфейса.
 * @return Код ошибки.
 */
extern err_t gui_init(gui_t* gui, graphics_t* graphics, gui_theme_t* theme);

/**
 * Получает графический буфер.
 * @param gui Графический интерфейс.
 * @return Графический буфер.
 */
ALWAYS_INLINE static graphics_t* gui_graphics(gui_t* gui)
{
    return gui->graphics;
}

/**
 * Устанавливает графический буфер.
 * @param gui
 * @param graphics
 * @return Код ошибки.
 */
extern err_t gui_set_graphics(gui_t* gui, graphics_t* graphics);

/**
 * Получает тему оформления.
 * @param gui Графический интерфейс.
 * @return Тема оформления.
 */
ALWAYS_INLINE static gui_theme_t* gui_theme(gui_t* gui)
{
    return gui->theme;
}

/**
 * Устанавливает тему оформления.
 * @param gui
 * @param theme Тема оформления.
 * @return Код ошибки.
 */
extern err_t gui_set_theme(gui_t* gui, gui_theme_t* theme);

/**
 * Получает корневой виджет.
 * @param gui Графический интерфейс.
 * @return Корневой виджет.
 */
ALWAYS_INLINE static gui_widget_t* gui_root_widget(gui_t* gui)
{
    return gui->root_widget;
}

/**
 * Устанавливает корневой виджет.
 * @param gui Графический интерфейс.
 * @param widget Корневой виджет.
 * @return Код ошибки.
 */
extern err_t gui_set_root_widget(gui_t* gui, gui_widget_t* widget);

/**
 * Получает виджет в фокусе.
 * @param gui Графический интерфейс.
 * @return Виджет в фокусе.
 */
ALWAYS_INLINE static gui_widget_t* gui_focus_widget(gui_t* gui)
{
    return gui->focus_widget;
}

/**
 * Получает флаг нахождения виджета в фокусе.
 * @param gui Графический интерфейс.
 * @return Флаг нахождения виджета в фокусе.
 */
ALWAYS_INLINE static bool gui_is_focus_widget(gui_t* gui, gui_widget_t* widget)
{
    return gui->focus_widget == widget;
}

/**
 * Устанавливает виджет в фокусе.
 * @param gui Графический интерфейс.
 * @param widget Виджет в фокусе, NULL для снятия фокуса.
 * @return true, если новый фокус установлен, иначе false.
 */
extern bool gui_set_focus_widget(gui_t* gui, gui_widget_t* widget);

/**
 * Очищает фокус.
 * @param gui Графический интерфейс.
 */
ALWAYS_INLINE static void gui_clear_focus_widget(gui_t* gui)
{
    gui_set_focus_widget(gui, NULL);
}

/**
 * Устанавливает фокус на следующий виджет.
 * @param gui Графический интерфейс.
 * @return true, если новый фокус установлен, иначе false.
 */
extern bool gui_focus_next_widget(gui_t* gui);

/**
 * Устанавливает фокус на предыдущий виджет.
 * @param gui Графический интерфейс.
 * @return true, если новый фокус установлен, иначе false.
 */
extern bool gui_focus_prev_widget(gui_t* gui);

/**
 * Получает виджет по заданным координатам.
 * @param gui Графический интерфейс.
 * @param x Координата X.
 * @param y Координата Y.
 * @return Виджет по заданным координатам.
 */
extern gui_widget_t* gui_widget_from_point(gui_t* gui, graphics_pos_t x, graphics_pos_t y);

/**
 * Перерисовывает графический интерфейс.
 * @param gui Графический интерфейс.
 * @param rect Область перерисовки, может быть NULL.
 */
extern void gui_repaint(gui_t* gui, rect_t* rect);

/**
 * Обрабатывает нажатие клавиши.
 * @param gui Графический интерфейс.
 * @param key Код клавиши.
 */
extern void gui_key_pressed(gui_t* gui, key_t key);

/**
 * Обрабатывает отпускание клавиши.
 * @param gui Графический интерфейс.
 * @param key Код клавиши.
 */
extern void gui_key_released(gui_t* gui, key_t key);

#endif	/* GUI_H */

