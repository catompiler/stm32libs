/**
 * @file tft9341_cache.h
 * Библиотека для работы с кэшем экрана TFT на контроллере ILI9341.
 */

#ifndef TFT9341_CACHE_H
#define TFT9341_CACHE_H

#include <stdint.h>
#include <stddef.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "tft9341/tft9341.h"
#include "graphics/graphics.h"

//! Тип позиции буфера.
typedef enum _Tft9341_Cache_buf_pos {
    TFT9341_CACHE_BUF_POS_UNALIGNED = 0, //!< Не выбрана позиция (один пиксел).
    TFT9341_CACHE_BUF_POS_HORISONTAL = 1, //!< Горизонтальная позиция.
    TFT9341_CACHE_BUF_POS_VERTICAL = 2, //!< Вертикальная позиция.
} tft9341_cache_buf_pos_t;

/**
 * Структура буфера кэша.
 */
typedef struct _Tft9341_Cache_Buffer {
    uint8_t* data; //!< Данные буфера.
    size_t size; //!< Размер данных.
    size_t byte_index; //!< Текущий индекс байта.
    size_t pixels_count; //!< Число пикселов в буфере.
    graphics_pos_t x; //!< Координата X буфера.
    graphics_pos_t y; //!< Координата Y буфера.
    tft9341_cache_buf_pos_t pos; //!< Положение буфера.
} tft9341_cache_buffer_t;

/**
 * Структура кэша экрана.
 */
typedef struct _Tft9341_Cache {
    tft9341_t* tft; //!< TFT.
    size_t pixel_size; //!< Размер пиксела.
    tft9341_cache_buffer_t* buffers; //!< Буферы кэша.
    size_t buffers_count; //!< Число буферов.
    size_t current_buffer; //!< Текущий буфер.
    tft9341_row_col_exchange_t row_col_exchange; //!< Ориентация экрана.
} tft9341_cache_t;

#define make_tft9341_cache_buffer(arg_data, arg_size)\
    { .data = (uint8_t*)arg_data, .size = arg_size, .byte_index = 0, .pixels_count = 0,\
      .x = 0, .y = 0, .pos = TFT9341_CACHE_BUF_POS_UNALIGNED }

#define make_tft9341_cache(arg_tft, arg_pixel_size, arg_buffers, arg_buffers_count, arg_row_col_exchange)\
    { .tft = arg_tft, .pixel_size = arg_pixel_size, .buffers = arg_buffers,\
      .buffers_count = arg_buffers_count, .current_buffer = 0, .row_col_exchange = arg_row_col_exchange }

/**
 * Инициализирует буфер кэша.
 * @param buffer Буфер.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
extern err_t tft9341_cache_buffer_init(tft9341_cache_buffer_t* buffer, void* data, size_t size);

/**
 * Инициализирует кэш.
 * @param cache Кэш.
 * @param tft TFT.
 * @param pixel_size Размер пиксела в байтах.
 * @param buffers Буферы кэша.
 * @param buffers_count Число буферов.
 * @return Код ошибки.
 */
extern err_t tft9341_cache_init(tft9341_cache_t* cache, tft9341_t* tft, size_t pixel_size,
                                tft9341_cache_buffer_t* buffers, size_t buffers_count,
                                tft9341_row_col_exchange_t row_col_exchange);

/**
 * Получает ориентацию TFT.
 * @param cache Кэш TFT.
 * @return Ориентация TFT.
 */
ALWAYS_INLINE static tft9341_row_col_exchange_t tft9341_cache_tft_row_col_exchange(tft9341_cache_t* cache)
{
    return cache->row_col_exchange;
}

/**
 * Устанавливает ориентацию TFT.
 * @param cache Кэш TFT.
 * @param row_col_exchange Ориентация TFT.
 */
ALWAYS_INLINE static void tft9341_cache_set_tft_row_col_exchange(tft9341_cache_t* cache, tft9341_row_col_exchange_t row_col_exchange)
{
    cache->row_col_exchange = row_col_exchange;
}


/**
 * Устанавливает пиксел.
 * @param cache Кэш.
 * @param x Координата X.
 * @param y Координата Y.
 * @param color Цвет.
 * @return Код ошибки.
 */
extern err_t tft9341_cache_set_pixel(tft9341_cache_t* cache, graphics_pos_t x, graphics_pos_t y, graphics_color_t color);

/**
 * Сбрасывает кэш в экран.
 * @param cache Кэш.
 * @return Код ошибки.
 */
extern err_t tft9341_cache_flush(tft9341_cache_t* cache);

/**
 * Заливает TFT заданным цветом.
 * @param cache Кэш.
 * @param color Цвет.
 * @return Код ошибки.
 */
extern err_t tft9341_cache_fill(tft9341_cache_t* cache, graphics_color_t color);

/**
 * Заливает регион TFT заданным цветом.
 * @param cache Кэш.
 * @param x0 Координата X верхнего левого угла региона.
 * @param y0 Координата Y верхнего левого угла региона.
 * @param x1 Координата X правого нижнего угла региона.
 * @param y1 Координата Y правого нижнего угла региона.
 * @param color Цвет.
 * @return Код ошибки.
 */
extern err_t tft9341_cache_fill_region(tft9341_cache_t* cache, graphics_pos_t x0, graphics_pos_t y0, graphics_pos_t x1, graphics_pos_t y1, graphics_color_t color);

#endif	//TFT9341_CACHE_H
