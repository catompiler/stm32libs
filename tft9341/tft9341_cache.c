#include "tft9341_cache.h"
#include <stdbool.h>



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
 * Заливает TFT заданным цветом.
 * Без использования буфера.
 * @param cache Кэш.
 * @param color Цвет.
 * @return Код ошибки.
 */
static err_t tft9341_cache_fill_tft(tft9341_cache_t* cache, graphics_color_t color)
{
    uint8_t put_buffer[TFT9341_PIXEL_SIZE_MAX];
    
    tft9341_cache_store_pixel(cache, put_buffer, color);
    
    err_t err = E_NO_ERROR;
    
    err = tft9341_set_column_address(cache->tft, 0, 0x1ff);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_set_page_address(cache->tft, 0, 0x1ff);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_begin_write(cache->tft);
    if(err != E_NO_ERROR) return err;
    
    size_t i = 0;
    
    for(; i < TFT9341_PIXELS_COUNT; i ++){
        err = tft9341_data(cache->tft, put_buffer, cache->pixel_size);
        if(err != E_NO_ERROR) break;
    }
    return err;
}

err_t tft9341_cache_fill(tft9341_cache_t* cache, graphics_color_t color)
{
    tft9341_cache_flush(cache);
    
    tft9341_cache_buffer_t* buffer = tft9341_cache_get_largest_buffer(cache);
    
    if(!tft9341_cache_buffer_valid(buffer)) return tft9341_cache_fill_tft(cache, color);
    
    size_t i = 0;
    
    for(; i < buffer->size; i += cache->pixel_size){
        tft9341_cache_store_pixel(cache, &buffer->data[i], color);
    }
    
    err_t err = E_NO_ERROR;
    
    err = tft9341_set_column_address(cache->tft, 0, 0x1ff);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_set_page_address(cache->tft, 0, 0x1ff);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_begin_write(cache->tft);
    if(err != E_NO_ERROR) return err;
    
    size_t total_bytes = TFT9341_PIXELS_COUNT * cache->pixel_size;
    size_t bytes_count = buffer->size;
    
    for(i = total_bytes; i != 0; i -= bytes_count){
        
        bytes_count = (i > buffer->size) ? buffer->size : i;
        
        err = tft9341_data(cache->tft, buffer->data, bytes_count);
        if(err != E_NO_ERROR) break;
    }
    
    return err;
}
