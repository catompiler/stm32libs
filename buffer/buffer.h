/**
 * @file buffer.h
 * Функции для работы с буфером.
 */

#ifndef BUFFER_H
#define	BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "buffer_defs.h"
#include "defs/defs.h"


/**
 * Структура буфера.
 */
typedef struct _Buffer {
    uint8_t* ptr;
    size_t size;
    volatile size_t pos;
}buffer_t;


/**
 * Инициализирует буфер.
 * @param buffer Буфер.
 * @param ptr Указатель на данные.
 * @param size Размер данных.
 * @param pos Позиция в данных.
 */
ALWAYS_INLINE static void buffer_init(buffer_t* buffer, uint8_t* ptr, size_t size)
{
    buffer->ptr = ptr;
    buffer->size = size;
    buffer->pos = 0;
}

/**
 * Сбрасывает позицию буфера.
 * @param buffer Буфер.
 */
ALWAYS_INLINE static void buffer_reset(buffer_t* buffer)
{
    buffer->pos = 0;
}

/**
 * Получает флаг валидности буфера.
 * @param buffer Буфер.
 */
ALWAYS_INLINE static bool buffer_valid(buffer_t* buffer)
{
    return buffer->ptr != NULL;
}

/**
 * Инкрементирует позицию буфера.
 * @param buffer Буфер.
 */
ALWAYS_INLINE static void buffer_next(buffer_t* buffer)
{
    buffer->pos ++;
}

/**
 * Получает оставшееся число байт данных до конца.
 * @param buffer Буфер.
 * @return Оставшееся число байт.
 */
ALWAYS_INLINE static uint8_t buffer_remain(buffer_t* buffer)
{
    return buffer->size - buffer->pos;
}

/**
 * Получает флаг нахождения указателя перед концом буфера.
 * @param buffer Буфер.
 * @return Флаг нахождения указателя перед концом буфера.
 */
ALWAYS_INLINE static bool buffer_at_last(buffer_t* buffer)
{
    return BUFFER_POS_AT_LAST(buffer->pos, buffer->size);
}

/**
 * Получает флаг нахождения указателя в конце буфера.
 * @param buffer Буфер.
 * @return Флаг нахождения указателя в конце буфера.
 */
ALWAYS_INLINE static bool buffer_at_end(buffer_t* buffer)
{
    return BUFFER_POS_AT_END(buffer->pos, buffer->size);
}

/**
 * Получает флаг наличия следующего байта в буфере.
 * @param buffer Буфер.
 * @return Флаг наличия следующего байта в буфере.
 */
ALWAYS_INLINE static bool buffer_has_next(buffer_t* buffer)
{
    return BUFFER_POS_HAS_NEXT(buffer->pos, buffer->size);
}

/**
 * Получает байт из буфера в текущей позиции.
 * @param buffer Буфер.
 * @return Байт из буфера в текущей позиции.
 */
ALWAYS_INLINE static uint8_t buffer_get(buffer_t* buffer)
{
    return BUFFER_POS_GET(buffer->ptr, buffer->pos);
}

/**
 * Получает байт из буфера в текущей позиции, инкрементируя указатель.
 * @param buffer Буфер.
 * @return Байт из буфера в текущей позиции.
 */
ALWAYS_INLINE static uint8_t buffer_get_next(buffer_t* buffer)
{
    return BUFFER_POS_GET(buffer->ptr, buffer->pos ++);
}

/**
 * Записывает байт в буфер в текущую позицию.
 * @param buffer Буфер.
 * @param byte Байт.
 */
ALWAYS_INLINE static void buffer_set(buffer_t* buffer, uint8_t byte)
{
    BUFFER_POS_SET(buffer->ptr, buffer->pos, byte);
}

/**
 * Записывает байт в буфер в текущую позицию, инкрементируя указатель.
 * @param buffer Буфер.
 * @param byte Байт.
 */
ALWAYS_INLINE static void buffer_set_next(buffer_t* buffer, uint8_t byte)
{
    BUFFER_POS_SET(buffer->ptr, buffer->pos ++, byte);
}


#endif	/* BUFFER_H */

