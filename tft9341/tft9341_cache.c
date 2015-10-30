#include "tft9341_cache.h"
#include <stdbool.h>
#include "utils/utils.h"



err_t tft9341_cache_buffer_init(tft9341_cache_buffer_t* buffer, void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    if(size == 0) return E_INVALID_VALUE;
    
    buffer->data = data;
    buffer->size = size;
    buffer->byte_index = 0;
    buffer->pixels_count = 0;
    buffer->x = 0;
    buffer->y = 0;
    buffer->pos = TFT9341_CACHE_BUF_POS_UNALIGNED;
    
    return E_NO_ERROR;
}

err_t tft9341_cache_init(tft9341_cache_t* cache, tft9341_t* tft, size_t pixel_size,
                         tft9341_cache_buffer_t* buffers, size_t buffers_count)
{
    if(tft == NULL) return E_NULL_POINTER;
    if(buffers == NULL && buffers_count != 0) return E_NULL_POINTER;
    if(buffers != NULL && buffers_count == 0) return E_INVALID_VALUE;
    
    cache->tft = tft;
    cache->pixel_size = pixel_size;
    cache->buffers = buffers;
    cache->buffers_count = buffers_count;
    
    return E_NO_ERROR;
}

/**
 * Получает флаг наличия буферов кэша.
 * @param cache Кэш.
 * @return Флаг наличия буферов кэша.
 */
ALWAYS_INLINE static bool tft9341_cache_has_buffers(tft9341_cache_t* cache)
{
    return cache->buffers != NULL;
}

/**
 * Получает флаг наличия данных в буфере.
 * @param buffer Буфер.
 * @return true при наличии данных, false в случае пустого буфера.
 */
ALWAYS_INLINE static bool tft9341_cache_buffer_dirty(tft9341_cache_buffer_t* buffer)
{
    return buffer->pixels_count != 0;
}

ALWAYS_INLINE static bool tft9341_cache_buffer_valid(tft9341_cache_buffer_t* buffer)
{
    return buffer != NULL && buffer->data != NULL;
}

/**
 * Получает текущий буфер кэша.
 * @param cache Кэш.
 * @return Текущий буфер кэша.
 */
ALWAYS_INLINE static tft9341_cache_buffer_t* tft9341_cache_current_buffer(tft9341_cache_t* cache)
{
    return &cache->buffers[cache->current_buffer];
}

/**
 * Получает буфер с заданным индексом.
 * @param cache Кэш.
 * @param index Индекс буфера.
 * @return Буфер с заданным индексом.
 */
ALWAYS_INLINE static tft9341_cache_buffer_t* tft9341_cache_get_buffer(tft9341_cache_t* cache, size_t index)
{
    return &cache->buffers[index];
}

/**
 * Получает следующий буфер и устанавливает его как текущий.
 * @param cache Кэш.
 * @return Следующий буфер.
 */
ALWAYS_INLINE static tft9341_cache_buffer_t* tft9341_cache_next_buffer(tft9341_cache_t* cache)
{
    if(++ cache->current_buffer >= cache->buffers_count){
        cache->current_buffer = 0;
    }
    return &cache->buffers[cache->current_buffer];
}

/**
 * Записывает пиксел в буфер.
 * @param cache Кэш.
 * @param buf Буфер.
 * @param color Пиксел.
 */
ALWAYS_INLINE static void tft9341_cache_store_pixel(tft9341_cache_t* cache, void* buf, graphics_color_t color)
{
    ((uint8_t*)buf)[0] = ((uint8_t*)&color)[0];
    ((uint8_t*)buf)[1] = ((uint8_t*)&color)[1];
    if(cache->pixel_size == TFT9341_PIXEL_SIZE_18BIT){
        ((uint8_t*)buf)[2] = ((uint8_t*)&color)[2];
    }
}

/**
 * Проверяет попадание в горизонтальный кэш.
 * @param buffer Буфер кэша.
 * @param x Координата X пиксела.
 * @param y Координата Y пиксела.
 * @return true в случае промаха, false в случае попадания.
 */
ALWAYS_INLINE static bool tft9341_cache_buffer_miss_horizontal(tft9341_cache_buffer_t* buffer, graphics_pos_t x, graphics_pos_t y)
{
    // Если
    /*if((y == buffer->y) && // Пиксел на одной линии с буфером и
       (x >= buffer->x) && // Правее начала буфера и
       (x <= buffer->x + buffer->pixels_count)){ // Левее или после последнего пиксела, то
        return false; // Попадание в кэш.
    }*/
    if(y != buffer->y) return true;
    if(x < buffer->x) return true;
    if(x > buffer->x + buffer->pixels_count) return true;
    return false;
}

/**
 * Проверяет попадание в вертикальный кэш.
 * @param buffer Буфер кэша.
 * @param x Координата X пиксела.
 * @param y Координата Y пиксела.
 * @return true в случае промаха, false в случае попадания.
 */
ALWAYS_INLINE static bool tft9341_cache_buffer_miss_vertical(tft9341_cache_buffer_t* buffer, graphics_pos_t x, graphics_pos_t y)
{
    // Если
    /*if((x == buffer->x) && // Пиксел на одной линии с буфером и
       (y >= buffer->y) && // Ниже начала буфера и
       (y <= buffer->y + buffer->pixels_count)){ // Выше или после последнего пиксела, то
        return false; // Попадание в кэш.
    }*/
    if(x != buffer->x) return true;
    if(y < buffer->y) return true;
    if(y > buffer->y + buffer->pixels_count) return true;
    return false;
}

/**
 * Проверяет попадание в кэш.
 * @param cache Кэш.
 * @param buffer Буфер кэша.
 * @param x Координата X пиксела.
 * @param y Координата Y пиксела.
 * @return true в случае промаха, false в случае попадания.
 */
static bool tft9341_cache_buffer_miss(tft9341_cache_t* cache, tft9341_cache_buffer_t* buffer, graphics_pos_t x, graphics_pos_t y)
{
    // Если буфер не занят ранее - можно использовать.
    if(!tft9341_cache_buffer_dirty(buffer)) return false;
    // Если выход за пределы буфера - промах.
    if(buffer->byte_index + cache->pixel_size > buffer->size) return true;
    // Проверим попадание в зависимости от положения.
    switch(buffer->pos){
        case TFT9341_CACHE_BUF_POS_UNALIGNED:
            return tft9341_cache_buffer_miss_vertical(buffer, x, y) &&
                   tft9341_cache_buffer_miss_horizontal(buffer, x, y);
        case TFT9341_CACHE_BUF_POS_HORISONTAL:
            return tft9341_cache_buffer_miss_horizontal(buffer, x, y);
        case TFT9341_CACHE_BUF_POS_VERTICAL:
            return tft9341_cache_buffer_miss_vertical(buffer, x, y);
    }
    return true;
}

/**
 * Сбрасывает состояние буфера.
 * @param buffer Буфер.
 */
ALWAYS_INLINE static void tft9341_cache_buffer_reset(tft9341_cache_buffer_t* buffer)
{
    buffer->byte_index = 0;
    buffer->pixels_count = 0;
    buffer->pos = TFT9341_CACHE_BUF_POS_UNALIGNED;
}

/**
 * Записывает в экран заданный буфер.
 * @param cache Кэш.
 * @param buffer Буфер.
 */
static void tft9341_cache_buffer_flush(tft9341_cache_t* cache, tft9341_cache_buffer_t* buffer)
{
    if(!tft9341_cache_buffer_dirty(buffer)) return;
    
    uint16_t x1 = buffer->x;
    uint16_t y1 = buffer->y;
    
    switch(buffer->pos){
        case TFT9341_CACHE_BUF_POS_HORISONTAL:
            x1 += buffer->pixels_count - 1;
            break;
        case TFT9341_CACHE_BUF_POS_VERTICAL:
            y1 += buffer->pixels_count - 1;
            break;
        default:
            break;
    }
    
    tft9341_write_region(cache->tft, buffer->x, buffer->y, x1, y1, buffer->data, buffer->byte_index);
    
    tft9341_cache_buffer_reset(buffer);
}

/**
 * Настраивает незанятый буфер.
 * @param buffer Буфер.
 * @param x Координата X начала буфера.
 * @param y Координата Y начала буфера.
 */
ALWAYS_INLINE static void tft9341_cache_buffer_setup(tft9341_cache_buffer_t* buffer, graphics_pos_t x, graphics_pos_t y)
{
    buffer->x = x;
    buffer->y = y;
}

/**
 * Помещает пиксел в буфер кэша.
 * @param cache Кэш.
 * @param buffer Буфер кэша.
 * @param x Координата X пиксела.
 * @param y Координата Y пиксела.
 * @param color Пиксел.
 */
ALWAYS_INLINE static void tft9341_cache_buffer_put(tft9341_cache_t* cache, tft9341_cache_buffer_t* buffer, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    if(buffer->pos == TFT9341_CACHE_BUF_POS_UNALIGNED){
        if(x > buffer->x) buffer->pos = TFT9341_CACHE_BUF_POS_HORISONTAL;
        else if(y > buffer->y) buffer->pos = TFT9341_CACHE_BUF_POS_VERTICAL;
    }
    
    size_t index = 0;
    
    switch(buffer->pos){
        case TFT9341_CACHE_BUF_POS_UNALIGNED:
            break;
        case TFT9341_CACHE_BUF_POS_HORISONTAL:
            index = (x - buffer->x) * cache->pixel_size;
            break;
        case TFT9341_CACHE_BUF_POS_VERTICAL:
            index = (y - buffer->y) * cache->pixel_size;
            break;
    }
    
    tft9341_cache_store_pixel(cache, &buffer->data[index], color);
    
    if(index == buffer->byte_index){
        buffer->byte_index += cache->pixel_size;
        buffer->pixels_count ++;
    }
}

err_t tft9341_cache_set_pixel(tft9341_cache_t* cache, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    uint8_t put_buffer[TFT9341_PIXEL_SIZE_MAX];
    
    tft9341_cache_buffer_t* buffer = NULL;
    
    size_t retry = 0;
    
    if(tft9341_cache_has_buffers(cache)){
    
        for(; retry < cache->buffers_count; retry ++){
            buffer = tft9341_cache_current_buffer(cache);

            if(!tft9341_cache_buffer_valid(buffer)){
                tft9341_cache_next_buffer(cache);
                continue;
            }
            
            if(!tft9341_cache_buffer_dirty(buffer)){
                tft9341_cache_buffer_setup(buffer, x, y);
                tft9341_cache_buffer_put(cache, buffer, x, y, color);
                return E_NO_ERROR;
            }

            if(!tft9341_cache_buffer_miss(cache, buffer, x, y)){
                tft9341_cache_buffer_put(cache, buffer, x, y, color);
                return E_NO_ERROR;
            }

            tft9341_cache_buffer_flush(cache, buffer);

            if(buffer == tft9341_cache_next_buffer(cache)){
                tft9341_wait(cache->tft);
            }
        }
    }
    
    tft9341_cache_store_pixel(cache, put_buffer, color);
    return tft9341_set_pixel(cache->tft, x, y, put_buffer, cache->pixel_size);
}

void tft9341_cache_flush(tft9341_cache_t* cache)
{
    if(!tft9341_cache_has_buffers(cache)) return;
    
    tft9341_cache_buffer_t* buffer = NULL;
    
    size_t i = 0;
    for(;i < cache->buffers_count; i++){
        buffer = tft9341_cache_get_buffer(cache, i);
        if(tft9341_cache_buffer_valid(buffer)) tft9341_cache_buffer_flush(cache, buffer);
    }
}

static tft9341_cache_buffer_t* tft9341_cache_get_largest_buffer(tft9341_cache_t* cache)
{
    size_t i = 0;
    size_t size = 0;
    
    tft9341_cache_buffer_t* buffer = NULL;
    tft9341_cache_buffer_t* res_buffer = NULL;
    
    for(;i < cache->buffers_count; i ++){
        
        buffer = tft9341_cache_get_buffer(cache, i);
        
        if(!tft9341_cache_buffer_valid(buffer)) continue;
        
        if(buffer->size > size){
            size = buffer->size;
            res_buffer = buffer;
        }
    }
    return res_buffer;
}

/**
 * Заливает регион TFT заданным цветом.
 * Без использования буфера.
 * @param cache Кэш.
 * @param x0 Координата X верхнего левого угла региона.
 * @param y0 Координата Y верхнего левого угла региона.
 * @param x1 Координата X правого нижнего угла региона.
 * @param y1 Координата Y правого нижнего угла региона.
 * @param total_bytes Размер данных для записи.
 * @param color Цвет.
 * @return Код ошибки.
 */
static err_t tft9341_cache_fill_region_tft_impl(tft9341_cache_t* cache, graphics_pos_t x0, graphics_pos_t y0, graphics_pos_t x1, graphics_pos_t y1, size_t total_bytes, graphics_color_t color)
{
    uint8_t put_buffer[TFT9341_PIXEL_SIZE_MAX];
    
    tft9341_cache_store_pixel(cache, put_buffer, color);
    
    err_t err = E_NO_ERROR;
    
    err = tft9341_set_column_address(cache->tft, x0, x1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_set_page_address(cache->tft, y0, y1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_begin_write(cache->tft);
    if(err != E_NO_ERROR) return err;
    
    //size_t total_bytes = ((x1 - x0) * (y1 - y0)) * cache->pixel_size;
    size_t i = 0;
    
    for(; i < total_bytes; i += cache->pixel_size){
        err = tft9341_data(cache->tft, put_buffer, cache->pixel_size);
        if(err != E_NO_ERROR) break;
    }
    return err;
}

/**
 * Заливает регион TFT заданным цветом.
 * Без использования буфера.
 * @param cache Кэш.
 * @param x0 Координата X верхнего левого угла региона.
 * @param y0 Координата Y верхнего левого угла региона.
 * @param x1 Координата X правого нижнего угла региона.
 * @param y1 Координата Y правого нижнего угла региона.
 * @param total_bytes Размер данных для записи.
 * @param color Цвет.
 * @return Код ошибки.
 */
err_t tft9341_cache_fill_region_impl(tft9341_cache_t* cache, graphics_pos_t x0, graphics_pos_t y0, graphics_pos_t x1, graphics_pos_t y1, size_t total_bytes, graphics_color_t color)
{
    tft9341_cache_flush(cache);
    
    tft9341_cache_buffer_t* buffer = tft9341_cache_get_largest_buffer(cache);
    
    if(!tft9341_cache_buffer_valid(buffer)) return tft9341_cache_fill_region_tft_impl(cache, x0, y0, x1, y1, total_bytes, color);
    
    //size_t total_bytes = ((x1 - x0) * (y1 - y0)) * cache->pixel_size;
    size_t bytes_count = buffer->size;
    size_t data_size = MIN(total_bytes, bytes_count);
    size_t i = 0;
    
    for(; i < data_size; i += cache->pixel_size){
        tft9341_cache_store_pixel(cache, &buffer->data[i], color);
    }
    
    err_t err = E_NO_ERROR;
    
    err = tft9341_set_column_address(cache->tft, x0, x1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_set_page_address(cache->tft, y0, y1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_begin_write(cache->tft);
    if(err != E_NO_ERROR) return err;
    
    for(i = total_bytes; i != 0; i -= bytes_count){
        
        bytes_count = (i > data_size) ? data_size : i;
        
        err = tft9341_data(cache->tft, buffer->data, bytes_count);
        if(err != E_NO_ERROR) break;
    }
    
    return err;
}

err_t tft9341_cache_fill(tft9341_cache_t* cache, graphics_color_t color)
{
    return tft9341_cache_fill_region_impl(cache, 0, 0, 0x1ff, 0x1ff, TFT9341_PIXELS_COUNT * cache->pixel_size, color);
}

err_t tft9341_cache_fill_region(tft9341_cache_t* cache, graphics_pos_t x0, graphics_pos_t y0, graphics_pos_t x1, graphics_pos_t y1, graphics_color_t color)
{
    if(x0 < 0) x0 = 0;
    if(x1 < 0) x1 = 0;
    if(y0 < 0) y0 = 0;
    if(y1 < 0) y1 = 0;
    
    graphics_pos_t swap_var;
    if(x0 > x1) SWAP(x0, x1, swap_var);
    if(y0 > y1) SWAP(y0, y1, swap_var);
    
    return tft9341_cache_fill_region_impl(cache, x0, y0, x1, y1, ((x1 - x0) * (y1 - y0)) * cache->pixel_size, color);
}
