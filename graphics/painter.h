/**
 * @file painter.h Библиотека рисования.
 */

#ifndef _PAINTER_H_
#define _PAINTER_H_

// ютф8!

#include <stdbool.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "graphics.h"
#include "font.h"
#include "fixed/fixed32.h"

//! Режимы рисования.
typedef enum _Painter_Mode {
    PAINTER_MODE_SET = 0, //!< Установка пиксела.
    PAINTER_MODE_OR, //!< OR значения пиксела.
    PAINTER_MODE_XOR, //!< XOR значения пиксела.
    PAINTER_MODE_AND //!< AND значения пиксела.
} painter_mode_t;

//! Кисть.
typedef enum _Painter_Brush {
    PAINTER_BRUSH_NONE = 0, //!< Нет заливки.
    PAINTER_BRUSH_SOLID, //!< Заливка полностью.
    PAINTER_BRUSH_DOT, //!< Заливка из точек.
    PAINTER_BRUSH_DOT2, //!< Заливка из более редких точек.
    PAINTER_BRUSH_DOT_DIAG, //!< Заливка из диагональных точек.
    PAINTER_BRUSH_DOT2_DIAG, //!< Заливка из более редких диагональных точек.
    PAINTER_BRUSH_DOT3_DIAG, //!< Заливка из более редких диагональных точек.
    PAINTER_BRUSH_DASH_VER, //!< Заливка из линий по вертикали.
    PAINTER_BRUSH_DASH2_VER, //!< Заливка из более редких линий по горизонтали.
    PAINTER_BRUSH_DASH_HOR, //!< Заливка из линий по вертикали.
    PAINTER_BRUSH_DASH2_HOR, //!< Заливка из более редких линий по горизонтали.
    PAINTER_BRUSH_DASH_CROSS, //!< Заливка из линий.
    PAINTER_BRUSH_DASH2_CROSS, //!< Заливка из более редких линий.
    PAINTER_BRUSH_DASH_DIAG_VER, //!< Заливка из диагональных линий по вертикали.
    PAINTER_BRUSH_DASH2_DIAG_VER, //!< Заливка из более редких диагональных линий по вертикали.
    PAINTER_BRUSH_DASH_DIAG_HOR, //!< Заливка из диагональных линий по горизонтали.
    PAINTER_BRUSH_DASH2_DIAG_HOR, //!< Заливка из более редких диагональных линий по горизонтали.
    PAINTER_BRUSH_DASH_DIAG_CROSS, //!< Заливка из диагональных линий по горизонтали.
    PAINTER_BRUSH_DASH2_DIAG_CROSS, //!< Заливка из более редких диагональных линий по горизонтали.
    PAINTER_BRUSH_CUSTOM //!< Пользовательский тип заливки.
} painter_brush_t;

//! Режим линии.
typedef enum _Painter_Pen {
    PAINTER_PEN_NONE = 0, //!< Нет линии.
    PAINTER_PEN_SOLID, //!< Цельная линия.
    PAINTER_PEN_DOT, //!< Линия из точек.
    PAINTER_PEN_DASH, //!< Линия из линий.
    PAINTER_PEN_DASH2, //!< Линия из более длинных линий.
    PAINTER_PEN_DASH_DOT, //!< Линия из точек и линий.
    PAINTER_PEN_CUSTOM //!< Пользовательский тип линии.
} painter_pen_t;

//! Режимы заливки.
typedef enum _Painter_Fill_Mode {
    PAINTER_FILL_MODE_ALL = 0, //!< Заливает все цвета внутри области.
    PAINTER_FILL_MODE_TARGET_COLOR //!< Заливает только область заданного цвета.
} painter_fill_mode_t;

//! Режим изображения-источника.
typedef enum _Painter_Source_Image_Mode {
    PAINTER_SOURCE_IMAGE_MODE_NORMAL = 0, //!< Нормальное изображение, пикселы копируются.
    PAINTER_SOURCE_IMAGE_MODE_MASK, //!< Использование изображения как маски для текущего цвета пиксела изображения.
    PAINTER_SOURCE_IMAGE_MODE_MAP, //!< Использование изображения как маски для текущего цвета рисования пикселов.
    /**
      * Использование пикселов изображения как флагов для рисования текущим цветом рисования пикселов.
      * Чёрный цвет - не рисовать пиксел; Любой другой цвет - рисовать пиксел текущим цветом рисования пикселов (Pen или Brush).
      */
    PAINTER_SOURCE_IMAGE_MODE_BITMAP
} painter_source_image_mode_t;

//! Структура рисовальщика.
typedef struct _Painter {
    graphics_t* graphics; //!< Холст.
    painter_mode_t mode; //!< Режим рисования.
    painter_pen_t pen; //!< Линия.
    painter_brush_t brush; //!< Кисть.
    const font_t* font; //!< Шрифт.
    painter_fill_mode_t fill_mode; //!< Режим заливки.
    painter_source_image_mode_t source_image_mode; //!< Режим изображения-источника.
    graphics_color_t pen_color; //!< Цвет линии.
    graphics_color_t brush_color; //!< Цвет кисти.
    graphics_color_t fill_color; //!< Цвет заливки примитивов.
    graphics_color_t fill_target_color; //!< Целевой цвет заливки примитивов.
    graphics_color_t transparent_color; //!< Прозрачный цвет.
    bool transparent_color_enabled; //!< Разрешённость прозрачного цвета.
    const graphics_t* custom_pen_graphics; //!< Изображение для пользовательской линии.
    const graphics_t* custom_brush_graphics; //!< Изображение для пользовательской кисти.
} painter_t;

/**
 * Заполняет структуру изображения по месту объявления.
 */
#define make_painter(arg_graphics) {.graphics = arg_graphics}

/**
 * Инициализирует рисовальщик.
 * Устанавливает значения по-умолчанию.
 * @param painter Рисовальщик.
 * @param graphics Холст.
 * @return Код ошибки.
 */
err_t painter_init(painter_t* painter, graphics_t* graphics);

/**
 * Устанавливает холст.
 * @param painter Рисовальщик.
 * @param graphics Холст.
 * @return true, если холст установлен, false в случае ошибки.
 */
extern bool painter_set_graphics(painter_t* painter, graphics_t* graphics);

/**
 * Получает холст.
 * @param painter Рисовальщик.
 * @return Холст.
 */
static ALWAYS_INLINE graphics_t* painter_graphics(painter_t* painter)
{
    return painter->graphics;
}

/**
 * Устанавливает режим рисования.
 * @param painter Рисовальщик.
 * @param mode Режим рисования.
 */
static ALWAYS_INLINE void painter_set_mode(painter_t* painter, painter_mode_t mode)
{
    painter->mode = mode;
}

/**
 * Получает режим рисования.
 * @param painter Рисовальщик.
 * @return Режим рисования.
 */
static ALWAYS_INLINE painter_mode_t painter_mode(const painter_t* painter)
{
    return painter->mode;
}

/**
 * Устанавливает режим рисования линии.
 * @param painter Рисовальщик.
 * @param line_mode Режим рисования линии.
 */
static ALWAYS_INLINE void painter_set_pen(painter_t* painter, painter_pen_t pen)
{
    painter->pen = pen;
}

/**
 * Получает режим рисования линии.
 * @param painter Рисовальщик.
 * @return Режим рисования линии.
 */
static ALWAYS_INLINE painter_pen_t painter_pen(const painter_t* painter)
{
    return painter->pen;
}

/**
 * Устанавливает изображение для пользовательского режима рисования линии.
 * @param painter Рисовальщик.
 * @param graphics Изображение.
 */
static ALWAYS_INLINE void painter_set_pen_graphics(painter_t* painter, const graphics_t* graphics)
{
    painter->custom_pen_graphics = graphics;
}

/**
 * Получает изображение для пользовательского режима рисования линии.
 * @param painter Рисовальщик.
 * @return Изображение.
 */
static ALWAYS_INLINE const graphics_t* painter_pen_graphics(painter_t* painter)
{
    return painter->custom_pen_graphics;
}

/**
 * Устанавливает кисть.
 * @param painter Рисовальщик.
 * @param brush Кисть.
 */
static ALWAYS_INLINE void painter_set_brush(painter_t* painter, painter_brush_t brush)
{
    painter->brush = brush;
}

/**
 * Получает кисть.
 * @param painter Рисовальщик.
 * @return Кисть.
 */
static ALWAYS_INLINE painter_brush_t painter_brush(const painter_t* painter)
{
    return painter->brush;
}

/**
 * Устанавливает изображение для пользовательской кисти.
 * @param painter Рисовальщик.
 * @param graphics Изображение.
 */
static ALWAYS_INLINE void painter_set_brush_graphics(painter_t* painter, const graphics_t* graphics)
{
    painter->custom_brush_graphics = graphics;
}

/**
 * Получает изображение для пользовательской кисти.
 * @param painter Рисовальщик.
 * @return Изображение.
 */
static ALWAYS_INLINE const graphics_t* painter_brush_graphics(painter_t* painter)
{
    return painter->custom_brush_graphics;
}

/**
 * Устанавливает шрифт.
 * @param painter Рисовальщик.
 * @param font Шрифт.
 */
static ALWAYS_INLINE void painter_set_font(painter_t* painter, const font_t* font)
{
    painter->font = font;
}

/**
 * Получает шрифт.
 * @param painter Рисовальщик.
 * @return Шрифт.
 */
static ALWAYS_INLINE const font_t* painter_font(const painter_t* painter)
{
    return painter->font;
}

/**
 * Устанавливает режим заливки.
 * @param painter Рисовальщик.
 * @param fill_mode Режим заливки.
 */
static ALWAYS_INLINE void painter_set_fill_mode(painter_t* painter, painter_fill_mode_t fill_mode)
{
    painter->fill_mode = fill_mode;
}

/**
 * Получает режим заливки.
 * @param painter Рисовальщик.
 * @return Режим заливки.
 */
static ALWAYS_INLINE painter_fill_mode_t painter_fill_mode(const painter_t* painter)
{
    return painter->fill_mode;
}

/**
 * Устанавливает режим изображения-источника.
 * @param painter Рисовальщик.
 * @param source_image_mode Режим изображения-источника.
 */
static ALWAYS_INLINE void painter_set_source_image_mode(painter_t* painter, painter_source_image_mode_t source_image_mode)
{
    painter->source_image_mode = source_image_mode;
}

/**
 * Получает режим изображения-источника.
 * @param painter Рисовальщик.
 * @return Режим изображения-источника.
 */
static ALWAYS_INLINE painter_source_image_mode_t painter_source_image_mode(const painter_t* painter)
{
    return painter->source_image_mode;
}

/**
 * Устанавливает цвет линии.
 * @param painter Рисовальщик.
 * @param color Цвет.
 */
static ALWAYS_INLINE void painter_set_pen_color(painter_t* painter, graphics_color_t color)
{
    painter->pen_color = color;
}

/**
 * Получает цвет линии.
 * @param painter Рисовальщик.
 * @return Цвет.
 */
static ALWAYS_INLINE graphics_color_t painter_pen_color(const painter_t* painter)
{
    return painter->pen_color;
}

/**
 * Устанавливает цвет кисти.
 * @param painter Рисовальщик.
 * @param brush_color Цвет кисти.
 */
static ALWAYS_INLINE void painter_set_brush_color(painter_t* painter, graphics_color_t brush_color)
{
    painter->brush_color = brush_color;
}

/**
 * Получает цвет кисти.
 * @param painter Рисовальщик.
 * @return Цвет кисти.
 */
static ALWAYS_INLINE graphics_color_t painter_brush_color(const painter_t* painter)
{
    return painter->brush_color;
}

/**
 * Устанавливает цвет заливки.
 * @param painter Рисовальщик.
 * @param fill_color Цвет заливки.
 */
static ALWAYS_INLINE void painter_set_fill_color(painter_t* painter, graphics_color_t fill_color)
{
    painter->fill_color = fill_color;
}

/**
 * Получает цвет заливки.
 * @param painter Рисовальщик.
 * @return Цвет заливки.
 */
static ALWAYS_INLINE graphics_color_t painter_fill_color(const painter_t* painter)
{
    return painter->fill_color;
}

/**
 * Устанавливает целевой цвет заливки.
 * @param painter Рисовальщик.
 * @param fill_target_color Целевой цвет заливки.
 */
static ALWAYS_INLINE void painter_set_fill_target_color(painter_t* painter, graphics_color_t fill_target_color)
{
    painter->fill_target_color = fill_target_color;
}

/**
 * Получает целевой цвет заливки.
 * @param painter Рисовальщик.
 * @return Целевой цвет заливки.
 */
static ALWAYS_INLINE graphics_color_t painter_fill_target_color(const painter_t* painter)
{
    return painter->fill_target_color;
}

/**
 * Устанавливает прозрачный цвет.
 * @param painter Рисовальщик.
 * @param transparent_color Прозрачный цвет.
 */
static ALWAYS_INLINE void painter_set_transparent_color(painter_t* painter, graphics_color_t transparent_color)
{
    painter->transparent_color = transparent_color;
}

/**
 * Получает прозрачный цвет.
 * @param painter Рисовальщик.
 * @return Прозрачный цвет.
 */
static ALWAYS_INLINE graphics_color_t painter_transparent_color(const painter_t* painter)
{
    return painter->transparent_color;
}

/**
 * Устанавливает разрешённость прозрачного цвета.
 * @param painter Рисовальщик.
 * @param transparent_color_enabled Разрешённость прозрачного цвета.
 */
static ALWAYS_INLINE void painter_set_transparent_color_enabled(painter_t* painter, graphics_color_t transparent_color_enabled)
{
    painter->transparent_color_enabled = transparent_color_enabled;
}

/**
 * Получает разрешённость прозрачного цвета.
 * @param painter Рисовальщик.
 * @return Разрешённость прозрачного цвета.
 */
static ALWAYS_INLINE graphics_color_t painter_transparent_color_enabled(const painter_t* painter)
{
    return painter->transparent_color_enabled;
}

/**
 * Получает цвет пиксела.
 * @param painter Рисовальщик.
 * @param x Координата X.
 * @param y Координата Y.
 * @return Цвет пиксела. Если координаты выходят за пределы изображения - возвращает 0.
 */
static ALWAYS_INLINE graphics_color_t painter_get_pixel(const painter_t* painter, graphics_pos_t x, graphics_pos_t y)
{
    return graphics_get_pixel(painter->graphics, x, y);
}

/**
 * Рисует точку.
 * @param painter Рисовальщик.
 * @param x Координата X.
 * @param y Координата Y.
 */
extern void painter_draw_point(painter_t* painter, graphics_pos_t x, graphics_pos_t y);

/**
 * Рисует вертикальную линию.
 * @param painter Рисовальщик.
 * @param x Координата X.
 * @param y0 Начальная координата Y.
 * @param y1 Конечная координата Y.
 */
extern void painter_draw_vline(painter_t* painter, graphics_pos_t x, graphics_pos_t y0, graphics_pos_t y1);

/**
 * Рисует горизонтальную линию.
 * @param painter Рисовальщик.
 * @param y Rоордината Y.
 * @param x0 Начальная координата X.
 * @param x1 Конечная координата X.
 */
extern void painter_draw_hline(painter_t* painter, graphics_pos_t y, graphics_pos_t x0, graphics_pos_t x1);

/**
 * Рисует линию.
 * @param painter Рисовальщик.
 * @param x0 Начальная координата X.
 * @param y0 Начальная координата Y.
 * @param x1 Конечная координата X.
 * @param y1 Конечная координата Y.
 */
extern void painter_draw_line(painter_t* painter, graphics_pos_t x0, graphics_pos_t y0, graphics_pos_t x1, graphics_pos_t y1);

/**
 * Рисует прямоугольник.
 * @param painter Рисовальщик.
 * @param left Лево.
 * @param top Верх.
 * @param right Право.
 * @param bottom Низ.
 */
extern void painter_draw_rect(painter_t* painter, graphics_pos_t left, graphics_pos_t top, graphics_pos_t right, graphics_pos_t bottom);

/**
 * Рисует окружность.
 * @param painter Рисовальщик.
 * @param center_x Координата центра X.
 * @param center_y Координата центра Y.
 * @param radius Радиус.
 */
extern void painter_draw_circle(painter_t* painter, graphics_pos_t center_x, graphics_pos_t center_y, graphics_pos_t radius);

/**
 * Копирует часть одного изображения в другое.
 * @param painter Рисовальщик.
 * @param dst_x Координата X цели.
 * @param dst_y Координата Y цели.
 * @param src_graphics Исходное изображение.
 * @param src_x Координата X источника.
 * @param src_y Координата Y источника.
 * @param src_width Ширина источника.
 * @param src_height Высота источника.
 */
extern void painter_bitblt(painter_t* painter, graphics_pos_t dst_x, graphics_pos_t dst_y,
                           const graphics_t* src_graphics, graphics_pos_t src_x, graphics_pos_t src_y,
                           graphics_size_t src_width, graphics_size_t src_height);

/**
 * Приводит угол к диапазону [0...359].
 * @param angle Угол.
 * @return Нормализованный угол.
 */
extern int32_t painter_normalize_angle(int32_t angle);

/**
 * Получает табличное значение синуса угла.
 * @param angle Угол.
 * @return Синус угла.
 */
extern fixed32_t painter_sin(int32_t angle);

/**
 * Получает табличное значение косинуса угла.
 * @param angle Угол.
 * @return Косинус угла.
 */
extern fixed32_t painter_cos(int32_t angle);

/**
 * Поворачивает координату X.
 * @param x Координата X.
 * @param angle Угол поворота.
 * @return Повёрнутая координата.
 */
extern graphics_pos_t painter_rotate_x(graphics_pos_t x, int32_t angle);

/**
 * Поворачивает координату Y.
 * @param x Координата Y.
 * @param angle Угол поворота.
 * @return Повёрнутая координата.
 */
extern graphics_pos_t painter_rotate_y(graphics_pos_t y, int32_t angle);

/**
 * Рисует дугу окружности.
 * @param painter Рисовальщик.
 * @param center_x Координата X центра дуги.
 * @param center_y Координата Y центра дуги.
 * @param radius Радиус дуги.
 * @param from_angle Начальный угол дуги.
 * @param to_angle Конечный угол дуги.
 */
extern void painter_draw_arc(painter_t* painter, graphics_pos_t center_x, graphics_pos_t center_y,
                             graphics_pos_t radius, int32_t from_angle, int32_t to_angle);

/**
 * Осуществляет заливку области,
 * ограниченной линиями цвета линии,
 * либо состоящей из целевого цвета,
 * цветом заливки.
 * @param painter Рисовальщик.
 * @param x Координата X.
 * @param y Координата Y.
 */
extern void painter_fill(painter_t* painter, graphics_pos_t x, graphics_pos_t y);

/**
 * Отрисовывает символ.
 * @param painter Рисовальщик.
 * @param font Шрифт.
 * @param x Абсцисса.
 * @param y Ордината.
 * @param c Символ.
 */
extern void painter_draw_char(painter_t* painter, graphics_pos_t x, graphics_pos_t y, font_char_t c);

/**
 * Отрисовывает строку символов.
 * @param painter Рисовальщик.
 * @param font Шрифт.
 * @param x Абсцисса.
 * @param y Ордината.
 * @param c Символ.
 */
extern void painter_draw_string(painter_t* painter, graphics_pos_t x, graphics_pos_t y, const char* s);

#endif  //_PAINTER_H_
