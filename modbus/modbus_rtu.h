/**
 * @file modbus_rtu.h Реализация протокола Modbus RTU.
 */

#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "errors/errors.h"
#include "usart/usart_bus.h"
#include "defs/defs.h"


//! Тип адреса протокола Modbus RTU.
typedef uint8_t modbus_rtu_address_t;

//! Тип кода функции протокола Modbus RTU.
typedef uint8_t modbus_rtu_func_t;

//! Тип ошибки протокола Modbus RTU.
typedef enum _Modbus_Rtu_Error {
    /**
     * Нет ошибки.
     */
    MODBUS_RTU_ERROR_NONE = 0,
    /**
     * Принятый код функции не может быть обработан.
     */
    MODBUS_RTU_ERROR_INVALID_FUNC = 0x1,
    /**
     * Адрес данных, указанный в запросе, недоступен.
     */
    MODBUS_RTU_ERROR_INVALID_ADDRESS = 0x2,
    /**
     * Значение, содержащееся в поле данных запроса,
     * является недопустимой величиной.
     */
    MODBUS_RTU_ERROR_INVALID_DATA = 0x3,
    /**
     * Невосстанавливаемая ошибка имела место, пока ведомое устройство
     * пыталось выполнить затребованное действие.
     */
    MODBUS_RTU_ERROR_NONRECOVERABLE = 0x4,
    /**
     * Ведомое устройство приняло запрос и обрабатывает его,
     * но это требует много времени.
     * Этот ответ предохраняет ведущее устройство
     * от генерации ошибки тайм-аута.
     */
    MODBUS_RTU_ERROR_OP_IN_PROGRESS = 0x5,
    /**
     * Ведомое устройство занято обработкой команды.
     * Ведущее устройство должно повторить сообщение позже,
     * когда ведомое освободится.
     */
    MODBUS_RTU_ERROR_BUSY = 0x6,
    /**
     * Ведомое устройство не может выполнить программную функцию,
     * заданную в запросе. Этот код возвращается для
     * неуспешного программного запроса,
     * использующего функции с номерами 13 или 14.
     * Ведущее устройство должно запросить диагностическую информацию
     * или информацию об ошибках от ведомого.
     */
    MODBUS_RTU_ERROR_FUNC = 0x7,
    /**
     * Ведомое устройство при чтении расширенной памяти
     * обнаружило ошибку паритета.
     * Ведущее устройство может повторить запрос,
     * но обычно в таких случаях требуется ремонт.
     */
    MODBUS_RTU_ERROR_PARITY = 0x8
} modbus_rtu_error_t;

//! Максимальный размер пакета протокола Modbus RTU.
#define MODBUS_RTU_PACKET_SIZE_MAX 256

//! Размер контрольной суммы протокола Modbus RTU.
#define MODBUS_RTU_CRC_SIZE 2

//! Размер полей сообщения протокола Modbus RTU.
#define MODBUS_RTU_FIELDS_SIZE (sizeof(modbus_rtu_address_t) + \
                                sizeof(modbus_rtu_func_t))

//! Размер полей и контрольной суммы сообщения протокола Modbus RTU.
#define MODBUS_RTU_FIELDS_CRC_SIZE (MODBUS_RTU_CRC_SIZE + MODBUS_RTU_FIELDS_SIZE)

//! Максимальный размер данных протокола Modbus RTU.
#define MODBUS_RTU_DATA_SIZE_MAX (MODBUS_RTU_PACKET_SIZE_MAX - MODBUS_RTU_FIELDS_CRC_SIZE)

//! Максимальный размер данных и контрольной суммы протокола Modbus RTU.
#define MODBUS_RTU_DATA_CRC_SIZE_MAX (MODBUS_RTU_DATA_SIZE_MAX + MODBUS_RTU_CRC_SIZE)

//! Тип ADU протокола Modbus RTU.
#pragma pack(push, 1)
typedef struct _Modbus_Rtu_Adu {
    modbus_rtu_address_t address; //!< Адрес устройства.
    modbus_rtu_func_t func; //!< Функция.
    uint8_t data_and_crc[MODBUS_RTU_DATA_CRC_SIZE_MAX]; //!< Данные и контрольная сумма.
} modbus_rtu_adu_t;
#pragma pack(pop)

//! Тип сообщения протокола Modbus RTU.
typedef struct _Modbus_Rtu_Message {
    modbus_rtu_adu_t adu; //!< ADU.
    size_t data_size; //!< Размер ADU.
} modbus_rtu_message_t;


//! Тип режима работы.
typedef enum _Modbus_Rtu_Mode {
    MODBUS_RTU_MODE_SLAVE = 0, //!< Ведомый.
    MODBUS_RTU_MODE_MASTER //!< Ведущий.
} modbus_rtu_mode_t;


//! Каллбэк приёма сообщения.
typedef void (*modbus_rtu_msg_recv_callback_t)(void);

//! Каллбэк передачи сообщения.
typedef void (*modbus_rtu_msg_sent_callback_t)(void);


//! Тип протокола Modbus RTU.
typedef struct _Modbus_Rtu {
    usart_bus_t* usart; //!< Шина USART.
    modbus_rtu_mode_t mode; //!< Режим работы.
    modbus_rtu_address_t address; //!< Адрес.
    modbus_rtu_message_t* rx_message; //!< Сообщение для приёма.
    modbus_rtu_message_t* tx_message; //!< Сообщение для передачи.
    modbus_rtu_msg_recv_callback_t recv_callback; //!< Каллбэк приёма сообщения.
    modbus_rtu_msg_sent_callback_t sent_callback; //!< Каллбэк передачи сообщения.
} modbus_rtu_t;


//! Тип структуры инициализации протокола Modbus RTU.
typedef struct _Modbus_Rtu_Init {
    usart_bus_t* usart; //!< Шина USART.
    modbus_rtu_mode_t mode; //!< Режим работы.
    modbus_rtu_address_t address; //!< Адрес.
    modbus_rtu_message_t* rx_message; //!< Сообщение для приёма.
    modbus_rtu_message_t* tx_message; //!< Сообщение для передачи.
} modbus_rtu_init_t;



/**
 * Инициализирует протокол Modbus RTU.
 * @param modbus Протокол Modbus RTU.
 * @param modbus_is Структура инициализации.
 * @return Код ошибки.
 */
extern err_t modbus_rtu_init(modbus_rtu_t* modbus, modbus_rtu_init_t* modbus_is);

/**
 * Отправляет сообщение протокола Modbus RTU.
 * Сообщение должно быть полностью сформировано
 * и вычислена его CRC.
 * @param modbus Протокол Modbus RTU.
 * @return Код ошибки.
 */
extern err_t modbus_rtu_send_message(modbus_rtu_t* modbus);

/**
 * Каллбэк получения байта по шине USART.
 * @param modbus Протокол Modbus RTU.
 * @param byte Полученный байт.
 * @return Флаг обработки получения байта.
 */
extern bool modbus_rtu_usart_rx_byte_callback(modbus_rtu_t* modbus, uint8_t byte);

/**
 * Каллбэк событий приёма данных по шине USART.
 * @param modbus Протокол Modbus RTU.
 * @return Флаг обработки события.
 */
extern bool modbus_rtu_usart_rx_callback(modbus_rtu_t* modbus);

/**
 * Каллбэк событий передачи данных по шине USART.
 * @param modbus Протокол Modbus RTU.
 * @return Флаг обработки события.
 */
extern bool modbus_rtu_usart_tx_callback(modbus_rtu_t* modbus);

/**
 * Получает каллбэк получения сообщения.
 * @param modbus Протокол Modbus RTU.
 * @return Каллбэк получения сообщения.
 */
extern modbus_rtu_msg_recv_callback_t modbus_rtu_msg_recv_callback(modbus_rtu_t* modbus);

/**
 * Устанавливает каллбэк получения сообщения.
 * @param modbus Протокол Modbus RTU.
 * @param callback Каллбэк получения сообщения.
 */
extern void modbus_rtu_set_msg_recv_callback(modbus_rtu_t* modbus, modbus_rtu_msg_recv_callback_t callback);

/**
 * Получает каллбэк передачи сообщения.
 * @param modbus Протокол Modbus RTU.
 * @return Каллбэк передачи сообщения.
 */
extern modbus_rtu_msg_sent_callback_t modbus_rtu_msg_sent_callback(modbus_rtu_t* modbus);

/**
 * Устанавливает каллбэк передачи сообщения.
 * @param modbus Протокол Modbus RTU.
 * @param callback Каллбэк передачи сообщения.
 */
extern void modbus_rtu_set_msg_sent_callback(modbus_rtu_t* modbus, modbus_rtu_msg_sent_callback_t callback);

/**
 * Получает ошибку приёма данных по шине USART.
 * @param modbus Протокол Modbus RTU.
 * @return Ошибка приёма данных по шине USART.
 */
ALWAYS_INLINE static usart_error_t modbus_rtu_rx_error(modbus_rtu_t* modbus)
{
    return usart_bus_rx_error(modbus->usart);
}

/**
 * Получает ошибку передачи данных по шине USART.
 * @param modbus Протокол Modbus RTU.
 * @return Ошибка передачи данных по шине USART.
 */
ALWAYS_INLINE static usart_error_t modbus_rtu_tx_error(modbus_rtu_t* modbus)
{
    return usart_bus_tx_error(modbus->usart);
}

/**
 * Получает сообщение для приёма протокола Modbus RTU.
 * @param modbus Протокол Modbus RTU.
 * @return Сообщение для приёма протокола Modbus RTU.
 */
extern modbus_rtu_message_t* modbus_rtu_rx_message(modbus_rtu_t* modbus);

/**
 * Получает сообщение для передачи протокола Modbus RTU.
 * @param modbus Протокол Modbus RTU.
 * @return Сообщение для передачи протокола Modbus RTU.
 */
extern modbus_rtu_message_t* modbus_rtu_tx_message(modbus_rtu_t* modbus);

/**
 * Формирует успешный ответ на сообщение.
 * @param message Формируемое сообщение.
 * @param from_message Сообщение для ответа.
 */
extern void modbus_rtu_message_answer_succ(modbus_rtu_message_t* message, const modbus_rtu_message_t* from_message);

/**
 * Формирует неудачный ответ на сообщение.
 * @param message Формируемое сообщение.
 * @param from_message Сообщение для ответа.
 */
extern void modbus_rtu_message_answer_fail(modbus_rtu_message_t* message, const modbus_rtu_message_t* from_message, modbus_rtu_error_t error);

/**
 * Сбрасывает сообщение протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 */
extern void modbus_rtu_message_reset(modbus_rtu_message_t* message);

/**
 * Получает адрес сообщения протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 * @return Адрес сообщения протокола Modbus RTU.
 */
extern modbus_rtu_address_t modbus_rtu_message_address(modbus_rtu_message_t* message);

/**
 * Устанавливает адрес сообщения протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 * @param address Адрес сообщения протокола Modbus RTU.
 */
extern void modbus_rtu_message_set_address(modbus_rtu_message_t* message, modbus_rtu_address_t address);

/**
 * Получает номер функции сообщения протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 * @return Номер функции сообщения протокола Modbus RTU.
 */
extern modbus_rtu_func_t modbus_rtu_message_func(modbus_rtu_message_t* message);

/**
 * Устанавливает номер функции сообщения протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 * @param func Номер функции сообщения протокола Modbus RTU.
 */
extern void modbus_rtu_message_set_func(modbus_rtu_message_t* message, modbus_rtu_func_t func);

/**
 * Получает размер данных сообщения протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 * @return Размер данных сообщения протокола Modbus RTU.
 */
extern size_t modbus_rtu_message_data_size(modbus_rtu_message_t* message);

/**
 * Устанавливает размер данных сообщения протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 * @param size Размер данных сообщения протокола Modbus RTU.
 * @return Код ошибки.
 */
extern err_t modbus_rtu_message_set_data_size(modbus_rtu_message_t* message, size_t size);

/**
 * Получает указатель на данные сообщения протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 * @return Указатель на данные сообщения протокола Modbus RTU.
 */
extern void* modbus_rtu_message_data(modbus_rtu_message_t* message);

/**
 * Копирует данные в сообщение протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 * @param data Данные для сообщения протокола Modbus RTU.
 * @param size Размер данных для сообщения протокола Modbus RTU.
 * @return Код ошибки.
 */
extern err_t modbus_rtu_message_copy_data(modbus_rtu_message_t* message, const void* data, size_t size);

/**
 * Получает значение контрольной суммы сообщения протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 * @return Значение контрольной суммы сообщения протокола Modbus RTU.
 */
extern uint16_t modbus_rtu_message_crc(modbus_rtu_message_t* message);

/**
 * Вычисляет значение контрольной суммы сообщения протокола Modbus RTU.
 * @param message Сообщение протокола Modbus RTU.
 * @return Значение контрольной суммы сообщения протокола Modbus RTU.
 */
extern uint16_t modbus_rtu_message_calc_crc(modbus_rtu_message_t* message);

#endif /* MODBUS_RTU_H */
