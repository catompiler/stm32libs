/**
 * @file graphics.h Библиотека работы с изображениями.
 */

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "errors/errors.h"
#include "defs/defs.h"



// Если задан хотя бы один формат пиксела - объявим перечисление.
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H) ||\
    defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H) ||\
    defined(USE_GRAPHICS_FORMAT_GRAY_2_VFD) ||\
    defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H) ||\
    defined(USE_GRAPHICS_FORMAT_RGB_332) || defined(USE_GRAPHICS_FORMAT_RGB_565) ||\
    defined(USE_GRAPHICS_FORMAT_RGB_8)

//! Перечисление форматов изображения.
typedef enum _Graphics_format {
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
    GRAPHICS_FORMAT_BW_1_V, //!< Чёрно-белый, 1 бит, байт вертикально.
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
    GRAPHICS_FORMAT_BW_1_H, //!<  Чёрно-белый, 1 бит, байт горизонтально.
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
    GRAPHICS_FORMAT_GRAY_2_V, //!<  Оттенки серого, 2 бита, байт вертикально.
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
    GRAPHICS_FORMAT_GRAY_2_H, //!<  Оттенки серого, 2 бита, байт горизонтально.
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_VFD
    GRAPHICS_FORMAT_GRAY_2_VFD, //!<  Оттенки серого, 2 бита, байт вертикально. Рост адреса сверху вниз. Обратный порядок бит.
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
    GRAPHICS_FORMAT_RGB_121_V, //!<  Цветной, 4 бита, байт вертикально.
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
    GRAPHICS_FORMAT_RGB_121_H, //!<  Цветной, 4 бита, байт горизонтально.
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_332
    GRAPHICS_FORMAT_RGB_332, //!<  Цветной, 8 бит.
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_565
    GRAPHICS_FORMAT_RGB_565, //!<  Цветной, 16 бит.
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_8
    GRAPHICS_FORMAT_RGB_8 //!<  Цветной, 24 бита.
#endif
} graphics_format_t;

#else

// Иначе определим тип как число.
typedef uint32_t graphics_format_t;

#endif // GRAPHICS_FORMAT_*



// Макросы цветов.
#define GRAPHICS_COLOR_BLACK 0x00000000
#define GRAPHICS_COLOR_WHITE 0x00ffffff
// BW & GRAY.
#define GRAPHICS_COLOR_BW_1(c) (c & 0x1)
#define GRAPHICS_COLOR_GRAY_2(c) (c & 0x3)
// RGB.
// RGB 121.
#define GRAPHICS_COLOR_RGB_121(r, g, b) (((r) & 0x1) | (((g) & 0x3) << 1) | (((b) & 0x1) << 3))
#define GRAPHICS_RED_RGB_121(c) ((c) & 0x1)
#define GRAPHICS_GREEN_RGB_121(c) (((c) >> 1) & 0x3)
#define GRAPHICS_BLUE_RGB_121(c) (((c) >> 3) & 0x1)
// RGB 332.
#define GRAPHICS_COLOR_RGB_332(r, g, b) (((r) & 0x7) | (((g) & 0x7) << 3) | (((b) & 0x3) << 6))
#define GRAPHICS_RED_RGB_332(c) ((c) & 0x7)
#define GRAPHICS_GREEN_RGB_332(c) (((c) >> 3) & 0x7)
#define GRAPHICS_BLUE_RGB_332(c) (((c) >> 6) & 0x3)
// RGB 565.
#define GRAPHICS_COLOR_RGB_565(r, g, b) (((r) & 0x1f) | (((g) & 0x3f) << 5) | (((b) & 0x1f) << 11))
#define GRAPHICS_RED_RGB_565(c) ((c) & 0x1f)
#define GRAPHICS_GREEN_RGB_565(c) (((c) >> 5) & 0x3f)
#define GRAPHICS_BLUE_RGB_565(c) (((c) >> 11) & 0x1f)
// RGB 8.
#define GRAPHICS_COLOR_RGB_8(r, g, b) (((r) & 0xff) | (((g) & 0xff) << 8) | (((b) & 0xff) << 16))
#define GRAPHICS_RED_RGB_8(c) ((c) & 0xff)
#define GRAPHICS_GREEN_RGB_8(c) (((c) >> 8) & 0xff)
#define GRAPHICS_BLUE_RGB_8(c) (((c) >> 16) & 0xff)
// BGR.
// BGR 121.
#define GRAPHICS_COLOR_BGR_121(r, g, b) (((b) & 0x1) | (((g) & 0x3) << 1) | (((r) & 0x1) << 3))
#define GRAPHICS_BLUE_BGR_121(c) ((c) & 0x1)
#define GRAPHICS_GREEN_BGR_121(c) (((c) >> 1) & 0x3)
#define GRAPHICS_RED_BGR_121(c) (((c) >> 3) & 0x1)
// BGR 332.
#define GRAPHICS_COLOR_BGR_332(r, g, b) (((b) & 0x7) | (((g) & 0x7) << 3) | (((r) & 0x3) << 6))
#define GRAPHICS_BLUE_BGR_332(c) ((c) & 0x7)
#define GRAPHICS_GREEN_BGR_332(c) (((c) >> 3) & 0x7)
#define GRAPHICS_RED_BGR_332(c) (((c) >> 6) & 0x3)
// BGR 565.
#define GRAPHICS_COLOR_BGR_565(r, g, b) (((b) & 0x1f) | (((g) & 0x3f) << 5) | (((r) & 0x1f) << 11))
#define GRAPHICS_BLUE_BGR_565(c) ((c) & 0x1f)
#define GRAPHICS_GREEN_BGR_565(c) (((c) >> 5) & 0x3f)
#define GRAPHICS_RED_BGR_565(c) (((c) >> 11) & 0x1f)
// BGR 8.
#define GRAPHICS_COLOR_BGR_8(r, g, b) (((b) & 0xff) | (((g) & 0xff) << 8) | (((r) & 0xff) << 16))
#define GRAPHICS_BLUE_BGR_8(c) ((c) & 0xff)
#define GRAPHICS_GREEN_BGR_8(c) (((c) >> 8) & 0xff)
#define GRAPHICS_RED_BGR_8(c) (((c) >> 16) & 0xff)

// RGB -> GRAY.
#define GRAPHICS_RGB_TO_GRAY(r, g, b) ((r) * 30 + (g) * 59 + (b) * 11)
// RGB 121.
#define GRAPHICS_RGB_121_TO_GRAY(c) GRAPHICS_RGB_TO_GRAY(GRAPHICS_RED_RGB_121(c), GRAPHICS_GREEN_RGB_121(c), GRAPHICS_BLUE_RGB_121(c))
// RGB 332.
#define GRAPHICS_RGB_332_TO_GRAY(c) GRAPHICS_RGB_TO_GRAY(GRAPHICS_RED_RGB_332(c), GRAPHICS_GREEN_RGB_332(c), GRAPHICS_BLUE_RGB_332(c))
// RGB 565.
#define GRAPHICS_RGB_565_TO_GRAY(c) GRAPHICS_RGB_TO_GRAY(GRAPHICS_RED_RGB_565(c), GRAPHICS_GREEN_RGB_565(c), GRAPHICS_BLUE_RGB_565(c))
// RGB 8.
#define GRAPHICS_RGB_8_TO_GRAY(c) GRAPHICS_RGB_TO_GRAY(GRAPHICS_RED_RGB_8(c), GRAPHICS_GREEN_RGB_8(c), GRAPHICS_BLUE_RGB_8(c))
// Max RGB -> GRAY values
// RGB 121.
#define GRAPHICS_RGB_121_TO_GRAY_MAX 218
// RGB 332.
#define GRAPHICS_RGB_332_TO_GRAY_MAX 656
// RGB 565.
#define GRAPHICS_RGB_565_TO_GRAY_MAX 4988
// RGB 8.
#define GRAPHICS_RGB_8_TO_GRAY_MAX 25500

// BGR -> GRAY.
#define GRAPHICS_BGR_TO_GRAY(b, g, r) ((b) * 11 + (g) * 59 + (r) * 30)
// BGR 121.
#define GRAPHICS_BGR_121_TO_GRAY(c) GRAPHICS_BGR_TO_GRAY(GRAPHICS_BLUE_BGR_121(c), GRAPHICS_GREEN_BGR_121(c), GRAPHICS_RED_BGR_121(c))
// BGR 332.
#define GRAPHICS_BGR_332_TO_GRAY(c) GRAPHICS_BGR_TO_GRAY(GRAPHICS_BLUE_BGR_332(c), GRAPHICS_GREEN_BGR_332(c), GRAPHICS_RED_BGR_332(c))
// BGR 565.
#define GRAPHICS_BGR_565_TO_GRAY(c) GRAPHICS_BGR_TO_GRAY(GRAPHICS_BLUE_BGR_565(c), GRAPHICS_GREEN_BGR_565(c), GRAPHICS_RED_BGR_565(c))
// BGR 8.
#define GRAPHICS_BGR_8_TO_GRAY(c) GRAPHICS_BGR_TO_GRAY(GRAPHICS_BLUE_BGR_8(c), GRAPHICS_GREEN_BGR_8(c), GRAPHICS_RED_BGR_8(c))
// Max BGR -> GRAY values
// BGR 121.
#define GRAPHICS_BGR_121_TO_GRAY_MAX 218
// BGR 332.
#define GRAPHICS_BGR_332_TO_GRAY_MAX 656
// BGR 565.
#define GRAPHICS_BGR_565_TO_GRAY_MAX 4988
// BGR 8.
#define GRAPHICS_BGR_8_TO_GRAY_MAX 25500

// Приближённо делит число на DIV,
// равное числу POW2 - 1, которое является
// степенью двойки,
// используя преобразование к интервалу [0..256].
//#define GRAPHICS_FAST_DIV(N, DIV, POW2) (((N) * ((1 << (POW2) + 1) / (DIV))) >> (POW2 + POW2))
#define GRAPHICS_FAST_DIV(N, POW2) (((N) + ((N) >> POW2) + (1 << (POW2 - 1))) >> POW2)

// Приближённо делит число на 255,
// используя преобразование к интервалу [0..256].
//#define GRAPHICS_FAST_DIV_255(N) (((N) * 257) >> 16)
#define GRAPHICS_FAST_DIV_255(N) GRAPHICS_FAST_DIV(N, 8)

// Приближённо делит число на 127,
// используя преобразование к интервалу [0..128].
//#define GRAPHICS_FAST_DIV_127(N) (((N) * 129) >> 14)
#define GRAPHICS_FAST_DIV_127(N) GRAPHICS_FAST_DIV(N, 7)

// Приближённо делит число на 63,
// используя преобразование к интервалу [0..64].
//#define GRAPHICS_FAST_DIV_63(N) (((N) * 65) >> 12)
#define GRAPHICS_FAST_DIV_63(N) GRAPHICS_FAST_DIV(N, 6)

// Приближённо делит число на 31,
// используя преобразование к интервалу [0..32].
//#define GRAPHICS_FAST_DIV_31(N) (((N) * 33) >> 10)
#define GRAPHICS_FAST_DIV_31(N) GRAPHICS_FAST_DIV(N, 5)

// Приближённо делит число на 15,
// используя преобразование к интервалу [0..16].
//#define GRAPHICS_FAST_DIV_15(N) (((N) * 17) >> 8)
#define GRAPHICS_FAST_DIV_15(N) GRAPHICS_FAST_DIV(N, 4)

// Приближённо делит число на 7,
// используя преобразование к интервалу [0..8].
//#define GRAPHICS_FAST_DIV_7(N) (((N) * 9) >> 6)
#define GRAPHICS_FAST_DIV_7(N) GRAPHICS_FAST_DIV(N, 3)

// Приближённо делит число на 3,
// используя преобразование к интервалу [0..8].
//#define GRAPHICS_FAST_DIV_3(N) (((N) * 5) >> 4)
#define GRAPHICS_FAST_DIV_3(N) GRAPHICS_FAST_DIV(N, 2)


//! Тип цвета.
typedef uint32_t graphics_color_t;

//! Тип размера изображения.
typedef uint32_t graphics_size_t;

//! Тип позиции пиксела.
typedef int32_t graphics_pos_t;

/*
 * Виртуальный буфер - концепция, служащая для
 * работы без видеобуфера, при которой доступ
 * к пекселам осуществляется непосредственно
 * обращением к уствойству, используя каллбэки.
 * Библиотека считает буфер виртуальным, если
 * указатель на данные data имеет нулевой адрес.
 */
#ifdef USE_GRAPHICS_VIRTUAL_BUFFER

/**
 * Тип буфера изображения.
 */
typedef enum _Graphics_Type {
    //! Нормальный - буфер в памяти.
    GRAPHICS_TYPE_NORMAL = 0,
    //! Виртуальный - буфер где угодно, например  в экране.
    GRAPHICS_TYPE_VIRTUAL = 1
} graphics_type_t;

/*
 * Структура определена ниже.
 */
struct _Graphics;

/*
 * Прототипы функций для виртуального буфера.
 */
/**
 * Получение пиксела.
 */
typedef graphics_color_t (*graphics_get_pixel_proc_t)(const struct _Graphics* graphics, graphics_pos_t x, graphics_pos_t y);
/**
 * Установка пиксела.
 */
typedef bool (*graphics_set_pixel_proc_t)(struct _Graphics* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color);
/**
 * OR над пикселом.
 */
typedef bool (*graphics_or_pixel_proc_t)(struct _Graphics* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color);
/**
 * XOR над пикселом.
 */
typedef bool (*graphics_xor_pixel_proc_t)(struct _Graphics* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color);
/**
 * AND над пикселом.
 */
typedef bool (*graphics_and_pixel_proc_t)(struct _Graphics* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color);
/**
 * Сброс буфера в устройство.
 */
typedef bool (*graphics_flush_proc_t)(struct _Graphics* graphics);
/**
 * Быстрый вывод залитого прямоугольника.
 */
typedef bool (*graphics_fast_fillrect_proc_t)(struct _Graphics* graphics, graphics_pos_t left, graphics_pos_t top, graphics_pos_t right, graphics_pos_t bottom, graphics_color_t color);

/**
 * Структура виртуального буфера.
 */
typedef struct Graphics_Vbuf {
    graphics_get_pixel_proc_t virtual_get_pixel; //!< Получение пиксела.
    graphics_set_pixel_proc_t virtual_set_pixel; //!< Установка пиксела.
    graphics_or_pixel_proc_t virtual_or_pixel; //!< OR над пикселом.
    graphics_xor_pixel_proc_t virtual_xor_pixel; //!< XOR над пикселом.
    graphics_and_pixel_proc_t virtual_and_pixel; //!< AND над пикселом.
    graphics_flush_proc_t virtual_flush; //!< Сброс буфера в устройство.
    graphics_fast_fillrect_proc_t virtual_fast_fillrect; //!< Быстрый вывод залитого прямоугольника.
} graphics_vbuf_t;

#endif

//! Структура графических данныех изображения.
typedef struct _Graphics {
    uint8_t* data; //!< Графические данные.
    graphics_size_t width; //!< Ширина изображение.
    graphics_size_t height; //!< Высота изображения.
    graphics_format_t format; //!< Формат изображения.
#ifdef USE_GRAPHICS_VIRTUAL_BUFFER
    graphics_type_t type; //!< Тип буфера изображения.
    graphics_vbuf_t* vbuf; //!< Виртуальный буфер.
#endif
} graphics_t;

/**
 * Заполняет структуру изображения по месту объявления.
 */
#ifndef USE_GRAPHICS_VIRTUAL_BUFFER
#define make_graphics(arg_data, arg_width, arg_height, arg_format) { .data = (uint8_t*)arg_data, .width = arg_width, .height = arg_height, .format = arg_format }
#else
#define make_graphics(arg_data, arg_width, arg_height, arg_format)\
{\
 .data = (uint8_t*)arg_data, .width = arg_width, .height = arg_height, .format = arg_format,\
 .type = GRAPHICS_TYPE_NORMAL, .vbuf = NULL\
}
#endif


#ifdef USE_GRAPHICS_VIRTUAL_BUFFER
/**
 * Заполняет структуру виртуального буфера по месту объявления.
 */
#define make_graphics_vbuf(arg_virtual_get_pixel, arg_virtual_set_pixel,\
                           arg_virtual_or_pixel, arg_virtual_xor_pixel,\
                           arg_virtual_and_pixel, arg_virtual_flush,\
                           arg_virtual_fast_fillrect)\
{\
 .virtual_get_pixel = arg_virtual_get_pixel, .virtual_set_pixel = arg_virtual_set_pixel,\
 .virtual_or_pixel  = arg_virtual_or_pixel,  .virtual_xor_pixel = arg_virtual_xor_pixel,\
 .virtual_and_pixel = arg_virtual_and_pixel, .virtual_flush = arg_virtual_flush,\
 .virtual_fast_fillrect = arg_virtual_fast_fillrect\
}
/**
 * Заполняет структуру изображения с виртуальным буфером по месту объявления.
 */
#define make_graphics_virtual(arg_data, arg_width, arg_height, arg_format, arg_vbuf)\
{\
 .data = (uint8_t*)arg_data, .width = arg_width, .height = arg_height, .format = arg_format,\
 .type = GRAPHICS_TYPE_VIRTUAL, .vbuf = arg_vbuf\
}
#endif

/**
 * Инициализирует структуру изображения.
 * @param graphics Изображение.
 * @param data Графические данные.
 * @param width Ширина изображения.
 * @param height Высота изображения.
 * @param format Формат изображения.
 * @return Код ошибки.
 */
EXTERN err_t graphics_init(graphics_t* graphics, uint8_t* data, graphics_size_t width, graphics_size_t height, graphics_format_t format);

#ifdef USE_GRAPHICS_VIRTUAL_BUFFER
/**
 * Инициализирует структуру виртуального буфера.
 * @param vbuf Виртуальный буфер.
 * @param virtual_get_pixel Функция получения пиксела.
 * @param virtual_set_pixel Функция установки пиксела.
 * @param virtual_or_pixel Функция операции OR над пикселом.
 * @param virtual_xor_pixel Функция операции XOR над пикселом.
 * @param virtual_and_pixel Функция операции AND над пикселом.
 * @return Код ошибки.
 */
EXTERN err_t graphics_init_vbuf(graphics_vbuf_t* vbuf, graphics_get_pixel_proc_t virtual_get_pixel,
                                graphics_set_pixel_proc_t virtual_set_pixel, graphics_or_pixel_proc_t virtual_or_pixel,
                                graphics_xor_pixel_proc_t virtual_xor_pixel, graphics_and_pixel_proc_t virtual_and_pixel);
/**
 * Инициализирует структуру изображения с виртуальным буфером.
 * @param graphics Изображение.
 * @param width Ширина изображения.
 * @param height Высота изображения.
 * @param format Формат изображения.
 * @param vbuf Виртуальный буфер.
 * @return Код ошибки.
 */
EXTERN err_t graphics_init_virtual(graphics_t* graphics, void* data, graphics_size_t width, graphics_size_t height, graphics_format_t format, graphics_vbuf_t* vbuf);
#endif

/**
 * Получает адрес буфера изображения.
 * @param graphics Изображение.
 * @return адрес буфера изображения.
 */
static ALWAYS_INLINE const uint8_t* graphics_const_data(const graphics_t* graphics)
{
    return graphics->data;
}

/**
 * Получает адрес буфера изображения.
 * @param graphics Изображение.
 * @return адрес буфера изображения.
 */
static ALWAYS_INLINE uint8_t* graphics_data(graphics_t* graphics)
{
    return graphics->data;
}

/**
 * Получает ширину изображения.
 * @param graphics Изображение.
 * @return Ширину изображения.
 */
static ALWAYS_INLINE graphics_size_t graphics_width(const graphics_t* graphics)
{
    return graphics->width;
}

/**
 * Получает высоту изображения.
 * @param graphics Изображение.
 * @return Высоту изображения.
 */
static ALWAYS_INLINE graphics_size_t graphics_height(const graphics_t* graphics)
{
    return graphics->height;
}

/**
 * Получает формат изображения.
 * @param graphics Изображение.
 * @return Формат изображения.
 */
static ALWAYS_INLINE graphics_format_t graphics_format(const graphics_t* graphics)
{
    return graphics->format;
}

#ifdef USE_GRAPHICS_VIRTUAL_BUFFER
/**
 * Получает тип буфера.
 * @param graphics Изображение.
 * @return Тип буфера.
 */
static ALWAYS_INLINE graphics_type_t graphics_type(const graphics_t* graphics)
{
    return graphics->type;
}

/**
 * Получает виртуальный буфер.
 * @param graphics Изображение.
 * @return Виртуальный буфер.
 */
static ALWAYS_INLINE graphics_vbuf_t* graphics_virtual_buffer(graphics_t* graphics)
{
    return graphics->vbuf;
}

/**
 * Сбрасывает буфер в устройство.
 * @param graphics Изображение.
 * @return true в случае успеха, иначе false.
 */
EXTERN bool graphics_flush(graphics_t* graphics);

/**
 * Быстро выводит залитый прямоугольник.
 * @param graphics Изображение.
 * @param left Лево.
 * @param top Верх.
 * @param right Право.
 * @param bottom Низ.
 * @param color Цвет.
 * @return true в случае успеха, false в случае неудачи,
 * либо если функция быстрой заливки не поддерживается буферорм.
 */
EXTERN bool graphics_fast_fillrect(graphics_t* graphics, graphics_pos_t left, graphics_pos_t top, graphics_pos_t right, graphics_pos_t bottom, graphics_color_t color);
#endif

/**
 * Получает размер буфера изображения.
 * @param graphics Изображение.
 * @return Размер буфера изображения.
 */
EXTERN size_t graphics_data_size(const graphics_t* graphics);

/**
 * Очищает изображение.
 * @param graphics Изображение.
 */
EXTERN void graphics_clear(graphics_t* graphics);

/**
 * Заполняет буфер изображения заданным цветом.
 * @param graphics Изображение.
 * @param color Цвет.
 */
EXTERN void graphics_fill(graphics_t* graphics, graphics_color_t color);

/**
 * Получает позицию пиксела в массиве байт.
 * @param graphics Изображение.
 * @param x Координата X.
 * @param y Координата Y.
 * @param byte Первый байт пиксела.
 * @param bit Первый бит пиксела.
 * @return true в случае успеха, иначе false (например в случае выхода за границу массива).
 */
EXTERN bool graphics_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit);

/**
 * Получает цвет пиксела.
 * @param graphics Изображение.
 * @param x Координата X.
 * @param y Координата Y.
 * @return Цвет пиксела. Если координаты выходят за пределы изображения - возвращает 0.
 */
EXTERN graphics_color_t graphics_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y);

/**
 * Устанавливает цвет пиксела.
 * @param graphics Изображение.
 * @param x Координата X.
 * @param y Координата Y.
 * @param color Цвет пиксела.
 * @return true в случае успеха, иначе false (например в случае выхода за пределы изображения).
 */
EXTERN bool graphics_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color);

/**
 * Меняет цвет пиксела как OR.
 * @param graphics Изображение.
 * @param x Координата X.
 * @param y Координата Y.
 * @param color Цвет пиксела.
 * @return true в случае успеха, иначе false (например в случае выхода за пределы изображения).
 */
EXTERN bool graphics_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color);

/**
 * Меняет цвет пиксела как XOR.
 * @param graphics Изображение.
 * @param x Координата X.
 * @param y Координата Y.
 * @param color Цвет пиксела.
 * @return true в случае успеха, иначе false (например в случае выхода за пределы изображения).
 */
EXTERN bool graphics_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color);

/**
 * Меняет цвет пиксела как AND.
 * @param graphics Изображение.
 * @param x Координата X.
 * @param y Координата Y.
 * @param color Цвет пиксела.
 * @return true в случае успеха, иначе false (например в случае выхода за пределы изображения).
 */
EXTERN bool graphics_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color);

/**
 * Преобразует значение цвета из одного формата в другой.
 * Может незначительно искажать цвета из-за разной битности цветов.
 * @param to_format Целевой формат.
 * @param from_format Исходный формат.
 * @param color Значение цвета для преобразования.
 * @return Преобразованное значение цвета.
 */
EXTERN graphics_color_t graphics_convert_color(graphics_format_t to_format, graphics_format_t from_format, graphics_color_t color);

/**
 * Применяет к цвету значение маски яркости.
 * @param color_format Исходный цвет.
 * @param color Формат цвета.
 * @param mask_format Формат маски.
 * @param mask Маска цвета.
 * @return Значение цвета с применённой маской.
 */
EXTERN graphics_color_t graphics_apply_mask(graphics_format_t color_format, graphics_color_t color, graphics_format_t mask_format, graphics_color_t mask);

#endif  //_GRAPHICS_H_
