#include "circular_buffer.h"
#include <string.h>
#include "utils/utils.h"


void circular_buffer_init(circular_buffer_t* buffer, uint8_t* ptr, size_t size)
{
    buffer->ptr = ptr;
    buffer->put = 0;
    buffer->get = 0;
    buffer->size = size;
    buffer->count = 0;
}

void circular_buffer_reset(circular_buffer_t* buffer)
{
    buffer->put = 0;
    buffer->get = 0;
    buffer->count = 0;
}

bool circular_buffer_valid(circular_buffer_t* buffer)
{
    return /* buffer->size != 0 && */ buffer->ptr != NULL;
}

size_t circular_buffer_size(circular_buffer_t* buffer)
{
    return buffer->size;
}

size_t circular_buffer_free_size(circular_buffer_t* buffer)
{
    return buffer->size - buffer->count;
}

size_t circular_buffer_avail_size(circular_buffer_t* buffer)
{
    return buffer->count;
}

size_t circular_buffer_put(circular_buffer_t* buffer, uint8_t data)
{
    if(buffer->count == buffer->size) return 0;
    
    buffer->ptr[buffer->put] = data;
    
    CYCLIC_INC(buffer->put, 0, buffer->size);
    
    buffer->count ++;
    
    return 1;
}

size_t circular_buffer_get(circular_buffer_t* buffer, uint8_t* data)
{
    if(buffer->count == 0) return 0;
    
    *data = buffer->ptr[buffer->get];
    
    CYCLIC_INC(buffer->get, 0, buffer->size);
    
    buffer->count --;
    
    return 1;
}

size_t circular_buffer_peek(circular_buffer_t* buffer, uint8_t* data)
{
    if(buffer->count == 0) return 0;
    
    *data = buffer->ptr[buffer->get];
    
    return 1;
}

size_t circular_buffer_write(circular_buffer_t* buffer, const uint8_t* data, size_t size)
{
    // Если места недостаточно или размер данных равен 0 - возврат 0.
    if(buffer->size - buffer->count < size || size == 0) return 0;
    
    size_t n;
    for(n = 0; n < size; n ++) circular_buffer_put(buffer, *data ++);
    
    return size;
}

size_t circular_buffer_read(circular_buffer_t* buffer, uint8_t* data, size_t size)
{
    // Если в буфере нет данных, или размер данных равен 0 - возврат 0.
    if(buffer->count == 0 || size == 0) return 0;
    
    size_t n;
    for(n = 0; n < size; n ++) circular_buffer_get(buffer, data ++);
    
    return size;
}

