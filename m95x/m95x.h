/**
 * @file m95x.h Библиотека работы с памятью EEPROM M95x.
 */

#ifndef M95X_H
#define M95X_H

#include <stm32f10x.h>
#include <stddef.h>
#include <stdint.h>
#include "errors/errors.h"
#include "future/future.h"
#include "spi/spi.h"
#include "defs/defs.h"

//! Идентификатор передачи по-умолчанию.
#define M95X_DEFAULT_TRANSFER_ID 95

//! Размер буфера данных (WREN8 + WRITE8 + ADDRESS16).
#define M95X_BUFFER_SIZE 4
//! Число доступных сообщений SPI.
#define M95X_MESSAGES_COUNT 2

//! Тип структуры памяти eeprom.
typedef struct _M95X {
    //! Шина SPI.
    spi_bus_t* spi;
    //! Идентификатор передачи.
    spi_transfer_id_t transfer_id;
    //! Порт выбора ведомого.
    GPIO_TypeDef* ce_gpio;
    //! Пин выбора ведомого.
    uint16_t ce_pin;
    //! Будущее.
    future_t future;
    //! Размер страницы в байтах.
    uint16_t page_size;
    //! Буфер данных.
    uint8_t buffer[M95X_BUFFER_SIZE];
    //! Сообщения SPI.
    spi_message_t messages[M95X_MESSAGES_COUNT];
    //! Сообщение продолжения записи данных.
    spi_message_t* msg_cont_write;
    //! Размер оставшихся данных.
    size_t remain_cont_write;
} m95x_t;

//! Тип адреса памяти EEPROM.
typedef uint16_t m95x_address_t;

//! Размер страницы.
typedef enum _M95X_Page {
    M95X_PAGE_64 = 0, //!< 64 байта на страницу.
    M95X_PAGE_128     //!< 128 байт на страницу.
} m95x_page_t;

//! Размер страницы по-умолчанию.
#define M95X_PAGE_DEFAULT M95X_PAGE_64

/**
 * Тип структуры инициализации дисплея.
 */
typedef struct _M95X_init {
    spi_bus_t*          spi; //!< Шина SPI.
    spi_transfer_id_t   transfer_id; //!< Идентификатор передачи.
    GPIO_TypeDef*       ce_gpio; //!< Порт выбора ведомого.
    uint16_t            ce_pin; //!< Пин выбора ведомого.
    m95x_page_t         page; //!< Размер страницы.
} m95x_init_t;

//! Тип защиты памяти EEPROM.
typedef enum _M95X_Block_Protect {
    M95X_PROTECT_NONE = 0,          //!< Ничего не защищено.
    M95X_PROTECT_UPPER_QUARTER = 1, //!< Защищена верхняя четверть.
    M95X_PROTECT_UPPER_HALF = 2,    //!< Защищена верхняя половина.
    M95X_PROTECT_ALL = 3            //!< Защищено всё.
} m95x_block_protect_t;

//! Тип статуса EEPROM.
typedef struct _M95X_Status {
    bool write_in_progress; //! Запись в процессе.
    bool write_enable_latch; //! Запись разрешена.
    m95x_block_protect_t block_protect; //! Защита.
    bool status_reg_write_protect; //! Запрет запись в регистр состояния.
} m95x_status_t;


/**
 * Каллбэк SPI.
 * @return true, если событие обработано, иначе false.
 */
EXTERN bool m95x_spi_callback(m95x_t* eeprom);

/**
 * Инициализирует EEPROM.
 * @param eeprom EEPROM.
 * @param eeprom_init Структура инициализации.
 * @return Код ошибки.
 */
EXTERN err_t m95x_init(m95x_t* eeprom, m95x_init_t* eeprom_init);

/**
 * Получает флаг занятости EEPROM.
 * @param eeprom EEPROM.
 * @return Флаг занятости EEPROM.
 */
EXTERN bool m95x_busy(m95x_t* eeprom);

/**
 * Получает код ошибки асинхронно выполненой операции.
 * @param eeprom EEPROM.
 * @return Код ошибки асинхронно выполненой операции.
 */
EXTERN err_t m95x_error(m95x_t* eeprom);

/**
 * Ожидает завершения операции с EEPROM.
 * @param eeprom EEPROM.
 * @return Код ошибки операции.
 */
EXTERN err_t m95x_wait(m95x_t* eeprom);

/**
 * Получает размер страницы памяти в байтах.
 * @param eeprom EEPROM.
 * @return Размер страницы памяти в байтах.
 */
EXTERN size_t m95x_page_size(m95x_t* eeprom);

/**
 * Считывает регистр статуса.
 * Синхронная операция.
 * @param eeprom EEPROM.
 * @param status Статус.
 * @return Код ошибки.
 */
EXTERN err_t m95x_read_status(m95x_t* eeprom, m95x_status_t* status);

/**
 * Записывает регистр статуса.
 * Синхронная операция.
 * @param eeprom EEPROM.
 * @param status Статус.
 * @return Код ошибки.
 */
EXTERN err_t m95x_write_status(m95x_t* eeprom, m95x_status_t* status);

/**
 * Получает размер данных до конца страницы.
 * @param eeprom EEPROM.
 * @param address Адрес.
 * @return Размер данных до конца старницы.
 */
EXTERN size_t m95x_avail_data_size(m95x_t* eeprom, m95x_address_t address);

/**
 * Читает память EEPROM.
 * Асинхронная операция.
 * @param eeprom EEPROM.
 * @param address Адрес памяти.
 * @param data Буфер для чтения.
 * @param size Размер читаемых данных.
 * @return Код ошибки.
 */
err_t m95x_read(m95x_t* eeprom, m95x_address_t address, void* data, size_t size);

/**
 * Разрешает запись в EEPROM.
 * Синхронная операция.
 * @param eeprom EEPROM.
 * @return Код ошибки.
 */
err_t m95x_write_enable(m95x_t* eeprom);

/**
 * Запрещает запись в EEPROM.
 * Синхронная операция.
 * @param eeprom EEPROM.
 * @return Код ошибки.
 */
err_t m95x_write_enable(m95x_t* eeprom);

/**
 * Записывает память EEPROM.
 * Асинхронная операция.
 * Размер данных для записи не должен
 * выходить за границу страницы.
 * @param eeprom EEPROM.
 * @param address Адрес памяти.
 * @param data Буфер для чтения.
 * @param size Размер читаемых данных.
 * @return Код ошибки.
 */
err_t m95x_write(m95x_t* eeprom, m95x_address_t address, const void* data, size_t size);

#endif	// M95X_H

