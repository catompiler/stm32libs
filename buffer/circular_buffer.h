/**
 * @file circular_buffer.h
 * Функции для работы с кольцевым буфером.
 */

#ifndef CIRCULAR_BUFFER_H
#define	CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "defs/defs.h"


/**
 * Структура кольцевого буфера.
 */
typedef struct _CircularBuffer {
    uint8_t* ptr;
    size_t size;
    volatile size_t put;
    volatile size_t get;
    volatile size_t count;
}circular_buffer_t;


/**
 * Инициализирует кольцевой буфер.
 * @param buffer Кольцевой буфер.
 * @param ptr Указатель на память для буфера.
 * @param size Размер буфера.
 */
EXTERN void circular_buffer_init(circular_buffer_t* buffer, uint8_t* ptr, size_t size);

/**
 * Сбрасывает кольцевой буфер.
 * @param buffer Кольцевой буфер.
 */
EXTERN void circular_buffer_reset(circular_buffer_t* buffer);

/**
 * Получает флаг валидности кольцевого буфера.
 * @param buffer Кольцевой буфер.
 * @return Флаг валидности кольцевого буфера.
 */
EXTERN bool circular_buffer_valid(circular_buffer_t* buffer);

/**
 * Получает размер кольцевого буфера.
 * @param buffer Кольцевой буфер.
 * @return Размер кольцевого буфера.
 */
EXTERN size_t circular_buffer_size(circular_buffer_t* buffer);

/**
 * Получает свободное место в кольцевом буфере.
 * @param buffer Кольцевой буфер.
 * @return Свободное место в кольцевом буфере.
 */
EXTERN size_t circular_buffer_free_size(circular_buffer_t* buffer);

/**
 * Получает размер данных в кольцевом буфере.
 * @param buffer Кольцевой буфер.
 * @return Размер данных в кольцевом буфере.
 */
EXTERN size_t circular_buffer_avail_size(circular_buffer_t* buffer);

/**
 * Помещает байт данных в кольцевой буфер.
 * @param buffer Кольцевой буфер.
 * @param data Байт данных.
 * @return Число помещённых данных в кольцевой буфер (ноль если места недостаточно).
 */
EXTERN size_t circular_buffer_put(circular_buffer_t* buffer, uint8_t data);

/**
 * Получает байт данных из кольцевого буфера.
 * @param buffer Кольцевой буфер.
 * @param data Указатель на байт данных.
 * @return  Число полученных данных из кольцевого буфера (ноль если нет данных).
 */
EXTERN size_t circular_buffer_get(circular_buffer_t* buffer, uint8_t* data);

/**
 * Получает байт данных из кольцевого буфера без извлечения из буфера.
 * @param buffer Кольцевой буфер.
 * @param data Указатель на байт данных.
 * @return  Число полученных данных из кольцевого буфера (ноль если нет данных).
 */
EXTERN size_t circular_buffer_peek(circular_buffer_t* buffer, uint8_t* data);

/**
 * Помещает данные в кольцевой буфер.
 * @param buffer Кольцевой буфер.
 * @param data Данные.
 * @param size Размер данных.
 * @return Число помещённых данных в кольцевой буфер (ноль если места недостаточно).
 */
EXTERN size_t circular_buffer_write(circular_buffer_t* buffer, const uint8_t* data, size_t size);

/**
 * Получает данные из кольцевого буфера.
 * @param buffer Кольцевой буфер.
 * @param data Указатель на данные.
 * @param size Размер данных.
 * @return  Число полученных данных из кольцевого буфера (ноль если нет данных).
 */
EXTERN size_t circular_buffer_read(circular_buffer_t* buffer, uint8_t* data, size_t size);

#endif	/* CIRCULAR_BUFFER_H */

