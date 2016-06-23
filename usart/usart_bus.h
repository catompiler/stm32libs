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
    USART_ERROR_FRAMING, //!< Ошибка кадра.
    USART_ERROR_DMA//!< Ошибка DMA.
} usart_error_t;

//! Тип реакции на IDLE при приёме данных.
typedef enum _Usart_Idle_Mode {
    USART_IDLE_MODE_NONE = 0, //!< Ничего не делать.
    USART_IDLE_MODE_END_RX //!< Завершить приём данных.
} usart_idle_mode_t;

/**
 * Тип функции обратного вызова.
 * Вызывается при окончании приёма/передачи
 * даннх, или при возникновении ошибки.
 * @return true, если событие обработано, иначе false.
 */
typedef bool (*usart_bus_callback_t)(void);

/**
 * Тип функции обратного вызова приёма байта.
 * Вызывается при приёме байта.
 * @return true, если событие обработано, иначе false.
 */
typedef bool (*usart_bus_rx_byte_callback_t)(uint8_t byte);

/**
 * Тип идентификатора приёма/передачи.
 */
typedef uint8_t usart_transfer_id_t;

//! Идентификатор передачи по умолчанию.
#define USART_BUS_DEFAULT_TRANSFER_ID 0

/**
 * Структура буферизации USART.
 */
typedef struct _UsartBus {
    USART_TypeDef* usart_device; //!< Устройство USART.
    DMA_Channel_TypeDef* dma_rx_channel; //!< Канал DMA для приёма.
    DMA_Channel_TypeDef* dma_tx_channel; //!< Канал DMA для передачи.
    usart_bus_rx_byte_callback_t rx_byte_callback; //!< Каллбэк при приёме байта.
    usart_bus_callback_t rx_callback; //!< Каллбэк событий приёма данных шины USART.
    usart_bus_callback_t tx_callback; //!< Каллбэк событий передачи данных шины USART.
    bool dma_rx_locked;//!< Заблокирован канал получения.
    bool dma_tx_locked;//!< Заблокирован канал передачи.
    usart_transfer_id_t rx_transfer_id;//!< Идентификатор приёма.
    usart_transfer_id_t tx_transfer_id;//!< Идентификатор передачи.
    usart_status_t rx_status; //!< Состояние канала приёма.
    usart_status_t tx_status; //!< Состояние канала передачи.
    usart_error_t rx_error; //!< Ошибка канала приёма.
    usart_error_t tx_error; //!< Ошибка канала передачи.
    uint16_t rx_size; //!< Размер данных для приёма.
    uint16_t tx_size; //!< Размер данных для передачи.
    usart_idle_mode_t idle_mode; //!< Режим реакции на IDLE при приёме.
} usart_bus_t;

/**
 * Получает флаг разрешённости передатчика USART.
 * @param usart Устройство USART.
 * @return Флаг разрешённости передатчика USART.
 */
EXTERN FunctionalState usart_bus_transmitter_state(USART_TypeDef* usart);

/**
 * Получает флаг разрешённости приёмника USART.
 * @param usart Устройство USART.
 * @return Флаг разрешённости приёмника USART.
 */
EXTERN FunctionalState usart_bus_receiver_state(USART_TypeDef* usart);

/**
 * Получает флаг полудуплексного режима USART.
 * @param usart Устройство USART.
 * @return Флаг полудуплексного режима USART.
 */
EXTERN FunctionalState usart_bus_halfduplex_state(USART_TypeDef* usart);

/**
 * Инициализирует шину USART.
 * Разрешает необходимые прерывания USART.
 * @param usart Шина USART.
 * @param usart_bus_init Структура инициализации USART.
 * @return Код ошибки.
 */
EXTERN err_t usart_bus_init(usart_bus_t* usart, usart_bus_init_t* usart_bus_is);

/**
 * Функция для обработки прерывания USART.
 * Должна вызываться из обработчика прерывания USART.
 * @param usart Шина USART.
 */
EXTERN void usart_bus_irq_handler(usart_bus_t* usart);

/**
 * Обработчик прерывания канала передачи DMA.
 * @param usart Шина USART.
 * @return true, если канал использовался шиной usart, иначе false.
 */
EXTERN bool usart_bus_dma_rx_channel_irq_handler(usart_bus_t* usart);

/**
 * Обработчик прерывания канала приёма DMA.
 * @param usart Шина USART.
 * @return true, если канал использовался шиной usart, иначе false.
 */
EXTERN bool usart_bus_dma_tx_channel_irq_handler(usart_bus_t* usart);

/**
 * Получает флаг включения приёмника.
 * @param usart Шина USART.
 * @return true, если приёмник включен, иначе false.
 */
EXTERN bool usart_bus_receiver_enabled(usart_bus_t* usart);

/**
 * Включает приёмник.
 * @param usart Шина USART.
 */
EXTERN void usart_bus_receiver_enable(usart_bus_t* usart);

/**
 * Выключает приёмник.
 * @param usart Шина USART.
 */
EXTERN void usart_bus_receiver_disable(usart_bus_t* usart);

/**
 * Получает флаг включения передатчика.
 * @param usart Шина USART.
 * @return true, если передатчик включен, иначе false.
 */
EXTERN bool usart_bus_transmitter_enabled(usart_bus_t* usart);

/**
 * Включает передатчик.
 * @param usart Шина USART.
 */
EXTERN void usart_bus_transmitter_enable(usart_bus_t* usart);

/**
 * Выключает передатчик.
 * @param usart Шина USART.
 */
EXTERN void usart_bus_transmitter_disable(usart_bus_t* usart);

/**
 * Получает флаг занятости шины usart на приём.
 * @param usart Шина usart.
 * @return Флаг занятости шины usart.
 */
EXTERN bool usart_bus_rx_busy(usart_bus_t* usart);

/**
 * Получает флаг занятости шины usart на передачу.
 * @param usart Шина usart.
 * @return Флаг занятости шины usart.
 */
EXTERN bool usart_bus_tx_busy(usart_bus_t* usart);

/**
 * Ждёт завершения текущей операции usart на приём.
 * @param usart Шина USART.
 */
EXTERN void usart_bus_rx_wait(usart_bus_t* usart);

/**
 * Ждёт завершения текущей операции usart на передачу.
 * @param usart Шина USART.
 */
EXTERN void usart_bus_tx_wait(usart_bus_t* usart);

/**
 * Получает идентификатор приёма.
 * @param usart Шина USART.
 * @return Идентификатор передачи.
 */
EXTERN usart_transfer_id_t usart_bus_rx_transfer_id(usart_bus_t* usart);

/**
 * Устанавливает идентификатор приёма.
 * @param usart Шина USART.
 * @param id Идентификатор передачи.
 * @return true, если идентификатор передачи установлен, иначе false (шина занята).
 */
EXTERN bool usart_bus_set_rx_transfer_id(usart_bus_t* usart, usart_transfer_id_t id);

/**
 * Получает идентификатор передачи.
 * @param usart Шина USART.
 * @return Идентификатор передачи.
 */
EXTERN usart_transfer_id_t usart_bus_tx_transfer_id(usart_bus_t* usart);

/**
 * Устанавливает идентификатор передачи.
 * @param usart Шина USART.
 * @param id Идентификатор передачи.
 * @return true, если идентификатор передачи установлен, иначе false (шина занята).
 */
EXTERN bool usart_bus_set_tx_transfer_id(usart_bus_t* usart, usart_transfer_id_t id);

/**
 * Получает каллбэк приёма байта шины USART.
 * @param usart Шина USART.
 * @return Каллбэк приёма байта.
 */
EXTERN usart_bus_rx_byte_callback_t usart_bus_rx_byte_callback(usart_bus_t* usart);

/**
 * Устанавливает каллбэк приёма байта шины USART.
 * @param usart Шина USART.
 * @param callback Каллбэк приёма байта.
 */
EXTERN void usart_bus_set_rx_byte_callback(usart_bus_t* usart, usart_bus_rx_byte_callback_t callback);

/**
 * Получает каллбэк событий приёма данных шины USART.
 * @param usart Шина USART.
 * @return Каллбэк приёма байта.
 */
EXTERN usart_bus_callback_t usart_bus_rx_callback(usart_bus_t* usart);

/**
 * Устанавливает каллбэк событий приёма данных шины USART.
 * @param usart Шина USART.
 * @param callback Каллбэк приёма байта.
 */
EXTERN void usart_bus_set_rx_callback(usart_bus_t* usart, usart_bus_callback_t callback);

/**
 * Получает каллбэк событий передачи данных шины USART.
 * @param usart Шина USART.
 * @return Каллбэк.
 */
EXTERN usart_bus_callback_t usart_bus_tx_callback(usart_bus_t* usart);

/**
 * Устанавливает каллбэк событий передачи данных шины USART.
 * @param usart Шина USART.
 * @param callback Каллбэк.
 */
EXTERN void usart_bus_set_tx_callback(usart_bus_t* usart, usart_bus_callback_t callback);

/**
 * Получает состояние канала приёма шины USART.
 * @param usart Шина USART.
 * @return Состояние канала приёма.
 */
EXTERN usart_status_t usart_bus_rx_status(usart_bus_t* usart);

/**
 * Получает состояние канала передачи шины USART.
 * @param usart Шина USART.
 * @return Состояние канала передачи.
 */
EXTERN usart_status_t usart_bus_tx_status(usart_bus_t* usart);

/**
 * Получает ошибку канала приёма шины USART.
 * @param usart Шина USART.
 * @return Ошибка канала приёма.
 */
EXTERN usart_error_t usart_bus_rx_error(usart_bus_t* usart);

/**
 * Получает ошибку канала передачи шины USART.
 * @param usart Шина USART.
 * @return Ошибка канала передачи.
 */
EXTERN usart_error_t usart_bus_tx_error(usart_bus_t* usart);

/**
 * Получает число полученных
 * после завершения приёма байт данных.
 * @param usart Шина USART.
 * @return Число полученных байт данных.
 */
EXTERN size_t usart_bus_bytes_received(usart_bus_t* usart);

/**
 * Получает число переданных
 * после завершения передачи байт данных.
 * @param usart Шина USART.
 * @return Число переданных байт данных.
 */
EXTERN size_t usart_bus_bytes_transmitted(usart_bus_t* usart);

/**
 * Получает вид реакции на IDLE при приёме.
 * @param usart Шина USART.
 * @return Вид реакции на IDLE.
 */
EXTERN usart_idle_mode_t usart_bus_idle_mode(usart_bus_t* usart);

/**
 * Устанавливает вид реакции на IDLE при приёме.
 * @param usart Шина USART.
 * @param mode Вид реакции на IDLE.
 */
EXTERN void usart_bus_set_idle_mode(usart_bus_t* usart, usart_idle_mode_t mode);

/**
 * Пропускает текущий поток данных
 * до метки IDLE.
 * @param usart Шина USART.
 */
EXTERN void usart_bus_sleep(usart_bus_t* usart);

/**
 * Начинает принимать данные текущего потока
 * без ожидания метки IDLE.
 * @param usart Шина USART.
 */
EXTERN void usart_bus_wake(usart_bus_t* usart);

/**
 * Передаёт данные по шине USART.
 * Асинхронная операция.
 * @param usart Шина USART.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
EXTERN err_t usart_bus_send(usart_bus_t* usart, const void* data, size_t size);

/**
 * Направляет все последующие
 * полученные данные вплоть до
 * заданного размера в заданный буфер.
 * @param usart Шина USART.
 * @param data Буфер для данных.
 * @param size Размер буфера.
 * @return Код ошибки.
 */
EXTERN err_t usart_bus_recv(usart_bus_t* usart, void* data, size_t size);

#endif	/* USART_BUS_H */
