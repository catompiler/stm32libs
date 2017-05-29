/**
 * @file lm75.h Библиотека для работы с датчиком температуры lm75.
 */

#ifndef LM75_H
#define	LM75_H

#include <stdbool.h>
#include <stdint.h>
#include "errors/errors.h"
#include "future/future.h"
#include "i2c/i2c.h"
#include "defs/defs.h"
#include "fixed/fixed16.h"


//! Адрес lm75.
#define LM75_I2C_DEFAULT_ADDRESS 0x48

//! Идентификатор передачи i2c для lm75.
#define LM75_I2C_DEFAULT_TRANSFER_ID LM75_I2C_DEFAULT_ADDRESS


//! Число сообщений i2c.
#define LM75_I2C_MESSAGES_COUNT 2

//! Сруктура LM75.
typedef struct _Lm75 {
    i2c_bus_t* i2c; //!< Шина i2c.
    i2c_transfer_id_t transfer_id; //!< Идентификатор передачи.
    future_t future; //!< Будущее.
    uint8_t rom_address; //!< Адрес в памяти.
    i2c_message_t i2c_messages[LM75_I2C_MESSAGES_COUNT]; //!< Сообщения i2c.
} lm75_t;

//! Перечисление полярности пина OS.
typedef enum _Lm75_Os_Polarity {
    LM75_OS_POLARITY_LOW  = 0, //!< Активное значение 0.
    LM75_OS_POLARITY_HIGH = 1 //!< Активное значение 1.
} lm75_os_polarity_t;

//! Перечисление режима пина OS.
typedef enum _Lm75_Os_Mode {
    LM75_OS_MODE_COMPARATOR = 0, //!< Режим компаратора.
    LM75_OS_MODE_INTERRUPT  = 1 //!< Режим прерывания.
} lm75_os_mode_t;

//! Структура конфигурации LM75.
typedef struct _Lm75_Conf {
    uint8_t fault_queue;
    lm75_os_polarity_t os_polarity;
    lm75_os_mode_t os_mode;
    bool shutdown;
} lm75_conf_t;


/**
 * Инициализация Датчик температуры.
 * @param sensor Датчик температуры.
 * @param i2c Шина I2C.
 * @param address Адрес.
 * @return Код ошибки.
 */
EXTERN err_t lm75_init(lm75_t* sensor, i2c_bus_t* i2c, i2c_address_t address);

/**
 * Получает идентификатор передачи i2c.
 * @param sensor Датчик температуры.
 * @return Идентификатор передачи i2c.
 */
EXTERN i2c_transfer_id_t lm75_i2c_transfer_id(const lm75_t* sensor);

/**
 * Устанавливает идентификатор передачи i2c.
 * @param sensor Датчик температуры.
 * @param transfer_id Идентификатор передачи i2c.
 */
EXTERN void lm75_set_i2c_transfer_id(lm75_t* sensor, i2c_transfer_id_t transfer_id);

/**
 * Получает адрес устройства на шине i2c.
 * @param sensor Датчик температуры.
 * @return Адрес i2c.
 */
EXTERN i2c_address_t lm75_i2c_address(const lm75_t* sensor);

/**
 * Устанавливает адрес устройства на шине i2c.
 * @param sensor Датчик температуры.
 * @param address Адрес i2c.
 */
EXTERN void lm75_set_i2c_address(lm75_t* sensor, i2c_address_t address);

/**
 * Получает шину i2c.
 * @param sensor Датчик температуры.
 * @return Шина i2c.
 */
EXTERN i2c_bus_t* lm75_i2c_bus(const lm75_t* sensor);

/**
 * Устанавливает шину i2c.
 * @param sensor Датчик температуры.
 * @param i2c Шина i2c.
 */
EXTERN void lm75_set_i2c_bus(lm75_t* sensor, i2c_bus_t* i2c);

/**
 * Каллбэк i2c.
 * @param sensor Датчик температуры.
 * @return true, если событие обработано, иначе false.
 */
EXTERN bool lm75_i2c_callback(lm75_t* sensor);

/**
 * Сбрасывает состояние.
 * @param sensor Датчик температуры.
 */
EXTERN void lm75_reset(lm75_t* sensor);

/**
 * Завершает операцию датчика температуры
 * с ошибкой тайм-аута.
 * @param sensor Датчик температуры.
 */
EXTERN void lm75_timeout(lm75_t* sensor);

/**
 * Получает флаг процесса выполнения операции.
 * @param sensor Датчик температуры.
 * @return Флаг процесса выполнения операции.
 */
EXTERN bool lm75_busy(lm75_t* sensor);

/**
 * Получает флаг завершения текущей операции.
 * @param sensor Датчик температуры.
 * @return Флаг завершения текущей операции.
 */
EXTERN bool lm75_done(lm75_t* sensor);

/**
 * Получает код ошибки.
 * @param sensor Датчик температуры.
 * @return Код ошибки.
 */
EXTERN err_t lm75_error(lm75_t* sensor);

/**
 * Ждёт завершения текущей операции.
 * @param sensor Датчик температуры.
 */
EXTERN err_t lm75_wait(lm75_t* sensor);

/**
 * Читает температуру.
 * @param sensor Датчик температуры.
 * @param temp Температура.
 * @return Код ошибки.
 */
EXTERN err_t lm75_read_temp(lm75_t* sensor, fixed16_t* temp);

/**
 * Читает конфигурацию.
 * @param sensor Датчик температуры.
 * @param conf Конфигурация.
 * @return Код ошибки.
 */
EXTERN err_t lm75_read_conf(lm75_t* sensor, lm75_conf_t* conf);

/**
 * Записывает конфигурацию.
 * @param sensor Датчик температуры.
 * @param conf Конфигурация.
 * @return Код ошибки.
 */
EXTERN err_t lm75_write_conf(lm75_t* sensor, const lm75_conf_t* conf);

/**
 * Читает нижний предел температуры (гистерезис).
 * @param sensor Датчик температуры.
 * @param temp Температура.
 * @return Код ошибки.
 */
EXTERN err_t lm75_read_temp_hyst(lm75_t* sensor, fixed16_t* temp);

/**
 * Записывает нижний предел температуры (гистерезис).
 * @param sensor Датчик температуры.
 * @param temp Температура.
 * @return Код ошибки.
 */
EXTERN err_t lm75_write_temp_hyst(lm75_t* sensor, fixed16_t temp);

/**
 * Читает верхний предел температуры.
 * @param sensor Датчик температуры.
 * @param temp Температура.
 * @return Код ошибки.
 */
EXTERN err_t lm75_read_temp_os(lm75_t* sensor, fixed16_t* temp);

/**
 * Записывает верхний предел температуры.
 * @param sensor Датчик температуры.
 * @param temp Температура.
 * @return Код ошибки.
 */
EXTERN err_t lm75_write_temp_os(lm75_t* sensor, fixed16_t temp);

#endif	/* LM75_H */
