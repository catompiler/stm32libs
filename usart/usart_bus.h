/**
 * @file usart_bus.h
 * Библиотека для работы с шиной USART.
 */

#ifndef USART_BUS_H
#define	USART_BUS_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "errors/errors.h"
#include "defs/defs.h"



/**
 * Структура инициализации шины USART.
 */
typedef struct _UsartBusInit {
    USART_TypeDef* usart_device; //!< Устройство USART.
    DMA_Channel_TypeDef* dma_rx_channel; //!< Канал DMA для приёма.
    DMA_Channel_TypeDef* dma_tx_channel; //!< Канал DMA для передачи.
}usart_bus_init_t;

//! Состояние шины USART.
typedef enum _Usart_Status {
    USART_STATUS_IDLE = 0,//!< Бездействие.
    USART_STATUS_TRANSFERING,//!< Обмен данными.
    USART_STATUS_TRANSFERED,//!< Обмен данными завершён.
    USART_STATUS_ERROR//!< Ошибка.
} usart_status_t;

//! Ошибки шины USART.
typedef enum _Usart_Error {
    USART_ERROR_NONE = 0, //!< Нет ошибки.
    USART_ERROR_PARITY, //!< Ошибка чётности.
    USART_ERROR_NOISE, //!< Шум.
    USART_ERROR_OVERRUN, //!< Переполнение.
    USART_ERROR_FRAMING //!< Ошибка кадра.
} usart_error_t;

//! Каллбэк при приёме байта по шине USART.
typedef void (*usart_bus_callback_t)(uint8_t byte);

/**
 * Структура буферизации USART.
 */
typedef struct _UsartBus {
    USART_TypeDef* usart_device; //!< Устройство USART.
    DMA_Channel_TypeDef* dma_rx_channel; //!< Канал DMA для приёма.
    DMA_Channel_TypeDef* dma_tx_channel; //!< Канал DMA для передачи.
    usart_bus_callback_t callback; //!< Каллбэк при приёме байта.
    bool dma_rx_locked;//!< Заблокирован канал получения.
    bool dma_tx_locked;//!< Заблокирован канал передачи.
}usart_bus_t;

/**
 * Получает флаг разрешённости передатчика USART.
 * @param usart Устройство USART.
 * @return Флаг разрешённости передатчика USART.
 */
extern FunctionalState usart_bus_transmitter_state(USART_TypeDef* usart);

/**
 * Получает флаг разрешённости приёмника USART.
 * @param usart Устройство USART.
 * @return Флаг разрешённости приёмника USART.
 */
extern FunctionalState usart_bus_receiver_state(USART_TypeDef* usart);

/**
 * Инициализирует шину USART.
 * Разрешает необходимые прерывания USART.
 * @param usart Шина USART.
 * @param usart_bus_init Структура инициализации USART.
 * @return Код ошибки.
 */
extern err_t usart_bus_init(usart_bus_t* usart, usart_bus_init_t* usart_bus_is);

/**
 * Функция для обработки прерывания USART.
 * Должна вызываться из обработчика прерывания USART.
 * @param usart Шина USART.
 */
extern void usart_bus_irq_handler(usart_bus_t* usart);

/**
 * Получает каллбэк приёма байта шины USART.
 * @param usart Шина USART.
 * @return Каллбэк приёма байта.
 */
extern usart_bus_callback_t usart_bus_callback(usart_bus_t* usart);

/**
 * Устанавливает каллбэк приёма байта шины USART.
 * @param usart Шина USART.
 * @param callback Каллбэк приёма байта.
 */
extern void usart_bus_set_callback(usart_bus_t* usart, usart_bus_callback_t callback);

/**
 * Пропускает текущий поток данных
 * до метки IDLE.
 * @param usart Шина USART.
 */
extern void usart_bus_sleep(usart_bus_t* usart);

/**
 * Начинает принимать данные текущего потока
 * без ожидания метки IDLE.
 * @param usart Шина USART.
 */
extern void usart_bus_wake(usart_bus_t* usart);

/**
 * Копирует данные в буфер для асинхронной передачи по USART.
 * Если буфера нехватает, ждёт освобождения.
 * @param usart Шина USART.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
extern err_t usart_bus_write(usart_bus_t* usart, const void* data, size_t size);

/**
 * Получает асинхронно принятые по USART данные.
 * @param usart Шина USART.
 * @param data Буфер для данных.
 * @param size Размер буфера.
 * @return Код ошибки.
 */
extern err_t usart_bus_read(usart_bus_t* usart, void* data, size_t size);

#endif	/* USART_BUS_H */
