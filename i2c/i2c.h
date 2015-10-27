/**
 * @file i2c.h Библиотека по работе с I2C.
 */

#ifndef I2C_H
#define	I2C_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "errors/errors.h"
#include "defs/defs.h"

//! Ошибки I2C.
#define E_I2C (E_USER + 10)
//! Ошибка при некорректном сообщении.
#define E_I2C_INVALID_MESSAGE (E_I2C)

//! Тип адреса i2c-устройств.
typedef uint8_t i2c_address_t;

/**
 * Тип функции обратного вызова.
 * Вызывается при окончании приёма/передачи
 * даннх, или при возникновении ошибки.
 * @return true, если событие обработано, иначе false.
 */
typedef bool (*i2c_callback_t)(void);

//! Декларация структуры сообщения ниже.
struct _I2C_Message;
/**
 * Тип функции обратного вызова для сообщения.
 * Вызывается при успешном окончании передачи сообщения.
 */
typedef void (*i2c_message_callback_t)(struct _I2C_Message*);

/**
 * Тип идентификатора передачи.
 */
typedef uint8_t i2c_transfer_id_t;

//! Идентификатор передачи по умолчанию.
#define I2C_BUS_DEFAULT_TRANSFER_ID 0

//! Тип статуса шины i2c.
typedef enum _i2c_status {
    I2C_STATUS_IDLE = 0,//!< Бездействие.
    I2C_STATUS_TRANSFERING,//!< Обмен данными.
    I2C_STATUS_TRANSFERED,//!< Обмен данными завершён.
    I2C_STATUS_ERROR//!< Ошибка.
} i2c_status_t;

//! Тип ошибки i2c.
typedef enum _i2c_error {
    I2C_NO_ERROR = 0,//!< Нет ошибки.
    I2C_ERROR_TIMEOUT,//!< Тайм-аут.
    I2C_ERROR_OVERRUN,//!< Слишком медленная/быстрая пересылка данных.
    I2C_ERROR_NACK,//!< Получение Not Acknowledge от ведомого.
    I2C_ERROR_ARB_LOST,//!< Потеря приоритета на шине.
    I2C_ERROR_BUS_FAIL,//!< Ошибка шины.
    I2C_ERROR_DMA,//!< Ошибка DMA.
    I2C_ERROR_UNKNOWN//!< Прочие ошибки (SMBus).
} i2c_error_t;

//! Направление передачи i2c.
typedef enum _i2c_direction {
    I2C_READ = 0,//!< Чтение.
    I2C_WRITE//!< Запись.
} i2c_direction_t;

//! Тип сообщения i2c.
typedef struct _I2C_Message {
    i2c_address_t address;//!< Адрес устройства i2c.
    i2c_direction_t direction;//!< Режим передачи.
    void* data;//!< Данные для приёма/передачи.
    size_t data_size;//!< Размер данных.
    i2c_message_callback_t callback;//!< Функция обратного вызова.
    void* sender_data;//!< Данные отправителя сообщения.
} i2c_message_t;

//! Структуры шины i2c.
typedef struct _I2C {
    I2C_TypeDef* i2c_device;//!< Периферия i2c.
    DMA_Channel_TypeDef* dma_rx_channel;//!< Канал DMA для приёма.
    DMA_Channel_TypeDef* dma_tx_channel;//!< Канал DMA для передачи.
    i2c_status_t status;//!< Статус шины.
    i2c_error_t error;//!< Ошибка шины.
    i2c_callback_t callback;//!< Функция обратного вызова.
    i2c_transfer_id_t transfer_id;//!< Идентификатор передачи.
    i2c_message_t* messages;//!< Массив сообщений.
    size_t messages_count;//!< Число сообщений.
    size_t message_index;//!< Индекс текущего сообщения.
    uint8_t state;//!< Состояние передачи.
    bool dma_rx_locked;//!< Заблокирован канал получения.
    bool dma_tx_locked;//!< Заблокирован канал передачи.
} i2c_bus_t;

//! Структура инициализации шины i2c.
typedef struct _I2C_Bus_Init {
    I2C_TypeDef* i2c_device;//!< Периферия i2c.
    DMA_Channel_TypeDef* dma_rx_channel;//!< Канал DMA для приёма.
    DMA_Channel_TypeDef* dma_tx_channel;//!< Канал DMA для передачи.
}i2c_bus_init_t;

/**
 * Инициализирует шину i2c.
 * @param i2c Шина i2c.
 * @param device Периферия i2c.
 * @return Код ошибки.
 */
extern err_t i2c_bus_init(i2c_bus_t* i2c, i2c_bus_init_t* init);

/**
 * Обработчик прерывания события i2c.
 * @param i2c Шина i2c.
 */
extern void i2c_bus_event_irq_handler(i2c_bus_t* i2c);

/**
 * Обработчик прерывания ошибки i2c.
 * @param i2c Шина i2c.
 */
extern void i2c_bus_error_irq_handler(i2c_bus_t* i2c);

/**
 * Обработчик прерывания канала передачи DMA.
 * @param i2c Шина i2c.
 * @return true, если канал использовался шиной i2c, иначе false.
 */
extern bool i2c_bus_dma_rx_channel_irq_handler(i2c_bus_t* i2c);

/**
 * Обработчик прерывания канала приёма DMA.
 * @param i2c Шина i2c.
 * @return true, если канал использовался шиной i2c, иначе false.
 */
extern bool i2c_bus_dma_tx_channel_irq_handler(i2c_bus_t* i2c);

/**
 * Получает флаг занятости шины i2c.
 * @param i2c Шина i2c.
 * @return Флаг занятости шины i2c.
 */
extern bool i2c_bus_busy(i2c_bus_t* i2c);

/**
 * Ждёт завершения текущей операции i2c.
 */
extern void i2c_bus_wait(i2c_bus_t* i2c);

/**
 * Сбрасывает шину i2c.
 * @param i2c Шина i2c.
 */
extern void i2c_bus_reset(i2c_bus_t* i2c);

/**
 * Сбрасывает интерфейс.
 * @param i2c Шина i2c.
 */
extern void i2c_device_reset(I2C_TypeDef* device);

/**
 * Получает функцию обратного вызова.
 * @return Функция обратного вызова.
 */
extern i2c_callback_t i2c_bus_callback(i2c_bus_t* i2c);

/**
 * Устанавливает функцию обратного вызова.
 * @param callback Функция обратного вызова.
 * @return true, в случае успеха, иначе false (например, если шина занята).
 */
extern bool i2c_bus_set_callback(i2c_bus_t* i2c, i2c_callback_t callback);

/**
 * Получает идентификатор передачи.
 * @return Идентификатор передачи.
 */
extern i2c_transfer_id_t i2c_bus_transfer_id(i2c_bus_t* i2c);

/**
 * Устанавливает идентификатор передачи.
 * @param id Идентификатор передачи.
 * @return true, в случае успеха, иначе false (например, если шина занята).
 */
extern bool i2c_bus_set_transfer_id(i2c_bus_t* i2c, i2c_transfer_id_t id);

/**
 * Получает состояние шины i2c.
 * @param i2c Шина i2c.
 * @return Состояние шины i2c.
 */
extern i2c_status_t i2c_bus_status(i2c_bus_t* i2c);

/**
 * Получает ошибку шины i2c.
 * @param i2c Шина i2c.
 * @return Ошибку шины i2c.
 */
extern i2c_error_t i2c_bus_error(i2c_bus_t* i2c);

/**
 * Инициализирует сообщение.
 * @param message Сообщение.
 * @param address Адрес устройства.
 * @param direction Направление передачи.
 * @param data Данные для приёма/передачи.
 * @param data_size Размер буфера для приёма данных.
 * @return Код ошибки.
 */
extern err_t i2c_message_init(i2c_message_t* message, i2c_address_t address, i2c_direction_t direction, void* data, size_t data_size);

/**
 * Инициализирует сообщение.
 * Не проверяет ничего.
 * Для критичных к времени мест.
 * @param message Сообщение.
 * @param direction Направление передачи.
 * @param tx_data Данные для передачи.
 * @param rx_data Буфер для приёма данных.
 * @param data_size Размер буфера для приёма данных.
 */
ALWAYS_INLINE static void i2c_message_setup(i2c_message_t* message, i2c_address_t address, i2c_direction_t direction, void* data, size_t data_size)
{
    message->address = address;
    message->direction = direction;
    message->data = data;
    message->data_size = data_size;
    message->callback = NULL;
    message->sender_data = NULL;
}

/**
 * Получает функцию обратного вызова сообщения.
 * @param message Сообщение.
 */
ALWAYS_INLINE static i2c_message_callback_t i2c_message_callback(i2c_message_t* message)
{
    return message->callback;
}

/**
 * Устанавливает функцию обратного вызова сообщения.
 * @param message Сообщение.
 * @param callback Функция обратного вызова.
 */
ALWAYS_INLINE static void i2c_message_set_callback(i2c_message_t* message, i2c_message_callback_t callback)
{
    message->callback = callback;
}

/**
 * Получает данные отправителя.
 * @param message Сообщение.
 */
ALWAYS_INLINE static void* i2c_message_sender_data(i2c_message_t* message)
{
    return message->sender_data;
}

/**
 * Устанавливает данные отправителя.
 * @param message Сообщение.
 * @param sender_data Данные отправителя.
 */
ALWAYS_INLINE static void i2c_message_set_sender_data(i2c_message_t* message, void* sender_data)
{
    message->sender_data = sender_data;
}

/**
 * Обменивается сообщениями по шине I2C.
 * @param i2c Шина I2C.
 * @param messages Сообщения.
 * @param messages_count Число сообщений.
 * @return Код ошибки.
 */
extern err_t i2c_bus_transfer(i2c_bus_t* i2c, i2c_message_t* messages, size_t messages_count);

#endif	/* I2C_H */
