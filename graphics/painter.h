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
#include "rect.h"
#include "point.h"

//! Режимы рисования.
typedef enum _Painter_Mode {
    PAINTER_MODE_SET = 0, //!< Установка пиксела.
    PAINTER_MODE_OR, //!< OR значения пиксела.
    PAINTER_MODE_XOR, //!< XOR значения пиксела.
    PAINTER_MODE_AND //!< AND значения пиксела.
} painter_mode_t;

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
      * Чёрный цвет - не рисовать пиксел; Любой другой цвет - рисовать пиксел текущим цветом рисования пикселов.
      */
    PAINTER_SOURCE_IMAGE_MODE_BITMAP,
    /**
      * Использование пикселов изображения как флагов для рисования текущим цветом рисования пикселов, либо залики фона.
      * Чёрный цвет - рисовать пиксел текущим цветом заливки фона; Любой другой цвет - рисовать пиксел текущим цветом рисования пикселов.
      */
    PAINTER_SOURCE_IMAGE_MODE_BITMASK
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
    rect_t scissor_rect; //!< Прямоугольная область отсечения.
    bool scissor_enabled; //!< Разрешённость проверки пикселов на вхождение в область отсечения.
    point_t offset_point; //!< Смещение рисуемых пикселов.
    bool offset_enabled; //!< Разрешённость смещения рисуемых пикселов.
} painter_t;

/**
 * Заполняет структуру изображения по месту объявления.
 */
#define make_painter(arg_graphics) {.graphics = arg_graphics, .mode = PAINTER_MODE_SET, .pen = PAINTER_PEN_SOLID, .brush = PAINTER_BRUSH_NONE,\
                                    .font = NULL, .fill_mode = PAINTER_FILL_MODE_ALL, .source_image_mode = PAINTER_SOURCE_IMAGE_MODE_NORMAL,\
                                    .pen_color = GRAPHICS_COLOR_BLACK, .brush_color = GRAPHICS_COLOR_BLACK, .fill_color = GRAPHICS_COLOR_BLACK,\
                                    .fill_target_color = GRAPHICS_COLOR_BLACK, .transparent_color = GRAPHICS_COLOR_BLACK,\
                                    .transparent_color_enabled = false, .custom_pen_graphics = NULL, .custom_brush_graphics = NULL,\
                                    .scissor_rect = MAKE_RECT(0, 0, 0, 0), .scissor_enabled = false,\
                                    .offset_point = MAKE_POINT(0, 0), .offset_enabled = false}

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
EXTERN bool painter_set_graphics(painter_t* painter, graphics_t* graphics);

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
 * Получает указатель на прямоугольную область отсечения.
 * @param painter Рисовальщик.
 * @return Указатель на прямоугольную область отсечения.
 */
static ALWAYS_INLINE const rect_t* painter_scissor_rect(const painter_t* painter)
{
    return &painter->scissor_rect;
}

/**
 * Устанавливает прямоугольную область отсечения.
 * @param painter Рисовальщик.
 * @param rect Указатель на прямоугольную область отсечения.
 */
static ALWAYS_INLINE void painter_set_scissor_rect(painter_t* painter, const rect_t* rect)
{
    rect_set_left  (&painter->scissor_rect, rect_left  (rect));
    rect_set_right (&painter->scissor_rect, rect_right (rect));
    rect_set_top   (&painter->scissor_rect, rect_top   (rect));
    rect_set_bottom(&painter->scissor_rect, rect_bottom(rect));
}

/**
 * Устанавливает прямоугольную область отсечения.
 * @param painter Рисовальщик.
 * @param left Лево.
 * @param top Верх.
 * @param right Право.
 * @param bottom Низ.
 */
static ALWAYS_INLINE void painter_set_scissor(painter_t* painter, graphics_pos_t left, graphics_pos_t top, graphics_pos_t right, graphics_pos_t bottom)
{
    rect_set_left  (&painter->scissor_rect, left);
    rect_set_right (&painter->scissor_rect, right);
    rect_set_top   (&painter->scissor_rect, top);
    rect_set_bottom(&painter->scissor_rect, bottom);
}

/**
 * Получает разрешённость проверки пиксела на вхождение в область отсечения.
 * @param painter Рисовальщик.
 * @return Разрешённость проверки пиксела на вхождение в область отсечения.
 */
static ALWAYS_INLINE bool painter_scissor_enabled(const painter_t* painter)
{
    return painter->scissor_enabled;
}

/**
 * Устанавливает разрешённость проверки пиксела на вхождение в область отсечения.
 * @param painter Рисовальщик.
 * @param enabled Разрешённость проверки пиксела на вхождение в область отсечения.
 */
static ALWAYS_INLINE void painter_set_scissor_enabled(painter_t* painter, bool enabled)
{
    painter->scissor_enabled = enabled;
}

/**
 * Получает указатель на точку смещения.
 * @param painter Рисовальщик.
 * @return Указатель на точку смещения.
 */
static ALWAYS_INLINE const point_t* painter_offset_point(const painter_t* painter)
{
    return &painter->offset_point;
}

/**
 * Устанавливает точку смещения.
 * @param painter Рисовальщик.
 * @param point Указатель на точку смещения.
 */
static ALWAYS_INLINE void painter_set_offset_point(painter_t* painter, const point_t* point)
{
    point_set_x(&painter->offset_point, point_x(point));
    point_set_y(&painter->offset_point, point_y(point));
}

/**
 * Устанавливает точку смещения.
 * @param painter Рисовальщик.
 * @param x Координата X.
 * @param y Координата Y.
 */
static ALWAYS_INLINE void painter_set_offset(painter_t* painter, graphics_pos_t x, graphics_pos_t y)
{
    point_set_x(&painter->offset_point, x);
    point_set_y(&painter->offset_point, y);
}

/**
 * Получает разрешённость смещения рисуемых пикселов.
 * @param painter Рисовальщик.
 * @return Разрешённость смещения рисуемых пикселов.
 */
static ALWAYS_INLINE bool painter_offset_enabled(const painter_t* painter)
{
    return painter->offset_enabled;
}

/**
 * Устанавливает разрешённость смещения рисуемых пикселов.
 * @param painter Рисовальщик.
 * @param enabled Разрешённость смещения рисуемых пикселов.
 */
static ALWAYS_INLINE void painter_set_offset_enabled(painter_t* painter, bool enabled)
{
    painter->offset_enabled = enabled;
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
 * Сбрасывает буфер изображения в устройство.
 * Имеет смысл лишь для виртуального
 * буфера изображения.
 * @param painter Рисовальщик.
 * @return true в случае успеха, иначе false.
 */
EXTERN bool painter_flush(painter_t* painter);

/**
 * Заполняет изображение.
 * @param painter Рисовальщик.
 */
EXTERN void painter_fill(painter_t* painter);

/**
 * Рисует точку.
 * @param painter Рисовальщик.
 * @param x Координата X.
 * @param y Координата Y.
 */
EXTERN void painter_draw_point(painter_t* painter, graphics_pos_t x, graphics_pos_t y);

/**
 * Рисует вертикальную линию.
 * @param painter Рисовальщик.
 * @param x Координата X.
 * @param y0 Начальная координата Y.
 * @param y1 Конечная координата Y.
 */
EXTERN void painter_draw_vline(painter_t* painter, graphics_pos_t x, graphics_pos_t y0, graphics_pos_t y1);

/**
 * Рисует горизонтальную линию.
 * @param painter Рисовальщик.
 * @param y Rоордината Y.
 * @param x0 Начальная координата X.
 * @param x1 Конечная координата X.
 */
EXTERN void painter_draw_hline(painter_t* painter, graphics_pos_t y, graphics_pos_t x0, graphics_pos_t x1);

/**
 * Рисует линию.
 * @param painter Рисовальщик.
 * @param x0 Начальная координата X.
 * @param y0 Начальная координата Y.
 * @param x1 Конечная координата X.
 * @param y1 Конечная координата Y.
 */
EXTERN void painter_draw_line(painter_t* painter, graphics_pos_t x0, graphics_pos_t y0, graphics_pos_t x1, graphics_pos_t y1);

/**
 * Рисует прямоугольник.
 * @param painter Рисовальщик.
 * @param left Лево.
 * @param top Верх.
 * @param right Право.
 * @param bottom Низ.
 */
EXTERN void painter_draw_rect(painter_t* painter, graphics_pos_t left, graphics_pos_t top, graphics_pos_t right, graphics_pos_t bottom);

/**
 * Рисует прямоугольник без границ.
 * @param painter Рисовальщик.
 * @param left Лево.
 * @param top Верх.
 * @param right Право.
 * @param bottom Низ.
 */
EXTERN void painter_draw_fillrect(painter_t* painter, graphics_pos_t left, graphics_pos_t top, graphics_pos_t right, graphics_pos_t bottom);

/**
 * Рисует окружность.
 * @param painter Рисовальщик.
 * @param center_x Координата центра X.
 * @param center_y Координата центра Y.
 * @param radius Радиус.
 */
EXTERN void painter_draw_circle(painter_t* painter, graphics_pos_t center_x, graphics_pos_t center_y, graphics_pos_t radius);

/**
 * Рисует окружность.
 * @param painter Рисовальщик.
 * @param center_x Координата центра X.
 * @param center_y Координата центра Y.
 * @param a Большая полуось.
 * @param b Малая полуось.
 */
EXTERN void painter_draw_ellipse(painter_t* painter, graphics_pos_t center_x, graphics_pos_t center_y, graphics_pos_t a, graphics_pos_t b);

/**
 * Рисует треугольник.
 * @param painter Рисовальщик.
 * @param x0 Координата X первой точки треугольника.
 * @param y0 Координата Y первой точки треугольника.
 * @param x1 Координата X второй точки треугольника.
 * @param y1 Координата Y второй точки треугольника.
 * @param x2 Координата X третьей точки треугольника.
 * @param y2 Координата Y третьей точки треугольника.
 */
EXTERN void painter_draw_triangle(painter_t* painter, graphics_pos_t x0, graphics_pos_t y0,
                                                      graphics_pos_t x1, graphics_pos_t y1,
                                                      graphics_pos_t x2, graphics_pos_t y2);

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
EXTERN void painter_bitblt(painter_t* painter, graphics_pos_t dst_x, graphics_pos_t dst_y,
                           const graphics_t* src_graphics, graphics_pos_t src_x, graphics_pos_t src_y,
                           graphics_size_t src_width, graphics_size_t src_height);

/**
 * Приводит угол к диапазону [0...359].
 * @param angle Угол.
 * @return Нормализованный угол.
 */
EXTERN int32_t painter_normalize_angle(int32_t angle);

/**
 * Получает табличное значение синуса угла.
 * @param angle Угол.
 * @return Синус угла.
 */
EXTERN fixed32_t painter_sin(int32_t angle);

/**
 * Получает табличное значение косинуса угла.
 * @param angle Угол.
 * @return Косинус угла.
 */
EXTERN fixed32_t painter_cos(int32_t angle);

/**
 * Поворачивает координату X.
 * result = x * cos(angle);
 * @param x Координата X.
 * @param angle Угол поворота.
 * @return Повёрнутая координата.
 */
EXTERN graphics_pos_t painter_rotate_x(graphics_pos_t x, int32_t angle);

/**
 * Поворачивает координату Y.
 * result = y * sin(angle);
 * @param x Координата Y.
 * @param angle Угол поворота.
 * @return Повёрнутая координата.
 */
EXTERN graphics_pos_t painter_rotate_y(graphics_pos_t y, int32_t angle);

/**
 * Выполняет вращение вектора (x, y) на угол angle.
 * @param x Координата X вектора.
 * @param y Координата Y вектора.
 * @param angle Угол поворота.
 */
EXTERN void painter_rotate(graphics_pos_t* x, graphics_pos_t* y, int32_t angle);

/**
 * Рисует дугу окружности.
 * @param painter Рисовальщик.
 * @param center_x Координата X центра дуги.
 * @param center_y Координата Y центра дуги.
 * @param radius Радиус дуги.
 * @param from_angle Начальный угол дуги.
 * @param to_angle Конечный угол дуги.
 */
EXTERN void painter_draw_arc(painter_t* painter, graphics_pos_t center_x, graphics_pos_t center_y,
                             graphics_pos_t radius, int32_t from_angle, int32_t to_angle);

/**
 * Рисует дугу эллипса.
 * @param painter Рисовальщик.
 * @param center_x Координата X центра дуги.
 * @param center_y Координата Y центра дуги.
 * @param a Большая полуось.
 * @param b Малая полуось.
 * @param from_angle Начальный угол дуги.
 * @param to_angle Конечный угол дуги.
 */
EXTERN void painter_draw_ellipse_arc(painter_t* painter, graphics_pos_t center_x, graphics_pos_t center_y,
                             graphics_pos_t a, graphics_pos_t b, int32_t from_angle, int32_t to_angle);

/**
 * Осуществляет заливку области,
 * ограниченной линиями цвета линии,
 * либо состоящей из целевого цвета,
 * цветом заливки.
 * @param painter Рисовальщик.
 * @param x Координата X.
 * @param y Координата Y.
 */
EXTERN void painter_flood_fill(painter_t* painter, graphics_pos_t x, graphics_pos_t y);

/**
 * Отрисовывает символ.
 * @param painter Рисовальщик.
 * @param font Шрифт.
 * @param x Абсцисса.
 * @param y Ордината.
 * @param c Символ.
 * @return Число отрисованных символов.
 */
EXTERN size_t painter_draw_char(painter_t* painter, graphics_pos_t x, graphics_pos_t y, font_char_t c);

/**
 * Отрисовывает строку символов.
 * @param painter Рисовальщик.
 * @param font Шрифт.
 * @param x Абсцисса.
 * @param y Ордината.
 * @param s Строка символов.
 * @return Число отрисованных символов.
 */
EXTERN size_t painter_draw_string(painter_t* painter, graphics_pos_t x, graphics_pos_t y, const char* s);

/**
 * Вычисляет размер отрисованной строки символов.
 * @param painter Рисовальщик.
 * @param s Строка символов.
 * @param width Ширина отрисованной строки символов.
 * @param height Высота отрисованной строки символов.
 */
EXTERN void painter_string_size(painter_t* painter, const char* s, graphics_size_t* width, graphics_size_t* height);

#endif  //_PAINTER_H_
