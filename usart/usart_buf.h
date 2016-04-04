/**
 * @file usart_buf.h
 * Библиотека для работы с буферами USART.
 */

#ifndef USART_BUF_H
#define	USART_BUF_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "errors/errors.h"
#include "buffer/circular_buffer.h"

/**
 * Структура буферизации USART.
 */
typedef struct _UsartBuf {
    //! Устройство USART.
    USART_TypeDef* usart;
    //! Буфер записи.
    circular_buffer_t write_buffer;
    //! Буфер чтения.
    circular_buffer_t read_buffer;
    //! Флаг переполнения буфера чтения.
    bool data_overrun;
}usart_buf_t;

/**
 * Структура инициализации буферизации USART.
 */
typedef struct _UsartBufInit {
    //! Устройство USART.
    USART_TypeDef* usart;
    //! Буфер записи в USART.
    void* write_buffer;
    //! Размер буфера записи.
    size_t write_buffer_size;
    //! Буфер чтения из USART.
    void* read_buffer;
    //! Размер буфера чтения.
    size_t read_buffer_size;
}usart_buf_init_t;


/**
 * Инициализирует буферизацию USART.
 * Разрешает необходимые прерывания USART.
 * @param usart_buf Буферизация USART.
 * @param usart_buf_init Структура инициализации USART.
 * @return Код ошибки.
 */
extern err_t usart_buf_init(usart_buf_t* usart_buf, usart_buf_init_t* usart_buf_init);

/**
 * Функция для обработки прерывания USART.
 * Должна вызываться из обработчика прерывания USART.
 * @param usart_buf Буферизация USART.
 */
extern void usart_buf_irq_handler(usart_buf_t* usart_buf);

/**
 * Получает флаг переполнения буфера чтения.
 * @param usart_buf Буферизация USART.
 * @return Флаг переполнения буфера чтения.
 */
extern bool usart_buf_data_overrun(usart_buf_t* usart_buf);

/**
 * Ждёт окончания передачи данных по USART.
 * @param usart_buf Буферизация USART.
 */
extern void usart_buf_flush(usart_buf_t* usart_buf);

/**
 * Получает количество принятых и не считанных байт данных.
 * @param usart_buf Буферизация USART.
 * @return Количество принятых и не считанных байт данных.
 */
extern size_t usart_buf_data_avail(usart_buf_t* usart_buf);

/**
 * Копирует байт данных для передачи по USART.
 * При необходимости ждёт освобождения буфера.
 * @param usart_buf Буферизация USART.
 * @param data Байт данных.
 * @return Размер скопированных данных.
 */
extern size_t usart_buf_put(usart_buf_t* usart_buf, uint8_t data);

/**
 * Получает асинхронно принятый по USART байт данных.
 * @param usart_buf Буферизация USART.
 * @param data Байт данных.
 * @return Размер скопированных данных.
 */
extern size_t usart_buf_get(usart_buf_t* usart_buf, uint8_t* data);

/**
 * Копирует данные в буфер для асинхронной передачи по USART.
 * Если буфера нехватает, ждёт освобождения.
 * @param usart_buf Буферизация USART.
 * @param data Данные.
 * @param size Размер данных.
 * @return Размер скопированных для передачи байт.
 */
extern size_t usart_buf_write(usart_buf_t* usart_buf, const void* data, size_t size);

/**
 * Получает асинхронно принятые по USART данные.
 * @param usart_buf Буферизация USART.
 * @param data Буфер для данных.
 * @param size Размер буфера.
 * @return Размер скопированных в буфер данных.
 */
extern size_t usart_buf_read(usart_buf_t* usart_buf, void* data, size_t size);

#ifdef USART_STDIO

#include <unistd.h>
#include <errno.h>
#include <stdio.h>

static usart_buf_t* _usart_stdio = NULL;

/**
 * Устанавливает буфер USART для использования в STDIO.
 * @param stdio_buf Буфер USART.
 */
static void usart_setup_stdio(usart_buf_t* stdio_buf)
{
    _usart_stdio = stdio_buf;
    
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

int _read(int file, char *ptr, int len)
{
    switch (file)
    {
    case STDIN_FILENO: /* stdin */
        len = (len == 1) ? (int)usart_buf_get(_usart_stdio, (uint8_t*)ptr):
                           (int)usart_buf_read(_usart_stdio, ptr, len);
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}

int _write(int file, char *ptr, int len)
{
    switch (file)
    {
    case STDOUT_FILENO: /* stdout */
    case STDERR_FILENO: /* stderr */
        len = (len == 1) ? (int)usart_buf_put(_usart_stdio, *ptr):
                           (int)usart_buf_write(_usart_stdio, ptr, len);
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}
#endif

#endif	/* USART_H */

