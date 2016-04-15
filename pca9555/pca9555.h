/**
 * @file pca9555.h
 * Библиотека для работы с расширителем ввода-вывода pca9555.
 */

#ifndef PCA9555_H
#define	PCA9555_H

#include <stdbool.h>
#include <stdint.h>
#include "errors/errors.h"
#include "future/future.h"
#include "i2c/i2c.h"
#include "pca9555_mem.h"


//Адрес pca9555.
#define PCA9555_I2C_DEFAULT_ADDRESS 0x20

//Идентификатор передачи i2c для pca9555.
#define PCA9555_I2C_DEFAULT_TRANSFER_ID PCA9555_I2C_DEFAULT_ADDRESS


//! Число сообщений i2c.
#define PCA9555_I2C_MESSAGES_COUNT 2

//! Сруктура PCA9555.
typedef struct _Pca9555 {
    i2c_bus_t* i2c; //!< Шина i2c.
    i2c_transfer_id_t transfer_id; //!< Идентификатор передачи.
    future_t future; //!< Будущее.
    uint8_t rom_address; //!< Адрес в памяти часов.
    pca9555_mem_t memory; //!< Образ памяти часов.
    i2c_message_t i2c_messages[PCA9555_I2C_MESSAGES_COUNT]; //!< Сообщения i2c.
} pca9555_t;

/**
 * Пины расширителя.
 */
typedef enum _Pca9555_Pin {
	PCA9555_PIN_NONE = 0, //!< Нет пина.
	PCA9555_PIN_0 = 1, //!< Пин 0.
	PCA9555_PIN_1 = 2, //!< Пин 1.
	PCA9555_PIN_2 = 4, //!< Пин 2.
	PCA9555_PIN_3 = 8, //!< Пин 3.
	PCA9555_PIN_4 = 16, //!< Пин 4.
	PCA9555_PIN_5 = 32, //!< Пин 5.
	PCA9555_PIN_6 = 64, //!< Пин 6.
	PCA9555_PIN_7 = 128, //!< Пин 7.
	PCA9555_PIN_8 = 256, //!< Пин 8.
	PCA9555_PIN_9 = 512, //!< Пин 9.
	PCA9555_PIN_10 = 1024, //!< Пин 10.
	PCA9555_PIN_11 = 2048, //!< Пин 11.
	PCA9555_PIN_12 = 4096, //!< Пин 12.
	PCA9555_PIN_13 = 8192, //!< Пин 13.
	PCA9555_PIN_14 = 16384, //!< Пин 14.
	PCA9555_PIN_15 = 32768, //!< Пин 15.
	PCA9555_PIN_ALL = 65535 //!< Все пины.
} pca9555_pin_t;

//! Совокупность пинов расширителя.
typedef uint16_t pca9555_pins_t;


//! Состояние пина.
typedef enum _Pca9555_Pin_State {
	PCA9555_PIN_OFF = 0, //!< Выкл.
	PCA9555_PIN_ON = 1 //!< Вкл.
} pca9555_pin_state_t;


//! Инверсия входа.
typedef enum _Pca9555_Pin_Inversion {
	PCA9555_PIN_NORMAL = 0, //!< Не инвертирован.
	PCA9555_PIN_INVERTED = 1 //!< Инвертирован.
} pca9555_pin_inversion_t;


//! Направление пина.
typedef enum _Pca9555_Pin_Direction {
	PCA9555_PIN_OUTPUT = 0, //!< Выход.
	PCA9555_PIN_INPUT = 1 //!< Вход.
} pca9555_pin_direction_t;


/**
 * Инициализация Порт ввода-вывода.
 * @param ioport Порт ввода-вывода.
 * @param i2c Шина I2C.
 * @param address Адрес.
 * @return Код ошибки.
 */
extern err_t pca9555_init(pca9555_t* ioport, i2c_bus_t* i2c, i2c_address_t address);

/**
 * Получает идентификатор передачи i2c.
 * @param ioport Порт ввода-вывода.
 * @return Идентификатор передачи i2c.
 */
extern i2c_transfer_id_t pca9555_i2c_transfer_id(const pca9555_t* ioport);

/**
 * Устанавливает идентификатор передачи i2c.
 * @param ioport Порт ввода-вывода.
 * @param transfer_id Идентификатор передачи i2c.
 */
extern void pca9555_set_i2c_transfer_id(pca9555_t* ioport, i2c_transfer_id_t transfer_id);

/**
 * Получает идентификатор передачи i2c.
 * @param ioport Порт ввода-вывода.
 * @return Адрес i2c.
 */
extern i2c_address_t pca9555_i2c_address(const pca9555_t* ioport);

/**
 * Устанавливает идентификатор передачи i2c.
 * @param ioport Порт ввода-вывода.
 * @param address Адрес i2c.
 */
extern void pca9555_set_i2c_address(pca9555_t* ioport, i2c_address_t address);

/**
 * Получает шину i2c.
 * @param ioport Порт ввода-вывода.
 * @return Шина i2c.
 */
extern i2c_bus_t* pca9555_i2c_bus(const pca9555_t* ioport);

/**
 * Устанавливает шину i2c.
 * @param ioport Порт ввода-вывода.
 * @param i2c Шина i2c.
 */
extern void pca9555_set_i2c_bus(pca9555_t* ioport, i2c_bus_t* i2c);

/**
 * Каллбэк i2c.
 * @param ioport Порт ввода-вывода.
 * @return true, если событие обработано, иначе false.
 */
extern bool pca9555_i2c_callback(pca9555_t* ioport);

/**
 * Получает флаг процесса выполнения операции.
 * @param ioport Порт ввода-вывода.
 * @return Флаг процесса выполнения операции.
 */
extern bool pca9555_busy(pca9555_t* ioport);

/**
 * Получает флаг завершения текущей операции.
 * @param ioport Порт ввода-вывода.
 * @return Флаг завершения текущей операции.
 */
extern bool pca9555_done(pca9555_t* ioport);

/**
 * Получает код ошибки.
 * @param ioport Порт ввода-вывода.
 * @return Код ошибки.
 */
extern err_t pca9555_error(pca9555_t* ioport);

/**
 * Ждёт завершения текущей операции.
 * @param ioport Порт ввода-вывода.
 */
extern err_t pca9555_wait(pca9555_t* ioport);

/**
 * Читает полное состояние.
 * Асинхронная операция.
 * @param ioport Порт ввода-вывода.
 * @return Код ошибки.
 */
extern err_t pca9555_read_state(pca9555_t* ioport);

/**
 * Читает входное состояние пинов.
 * Асинхронная операция.
 * @param ioport Порт ввода-вывода.
 * @return Код ошибки.
 */
extern err_t pca9555_read_pins_state(pca9555_t* ioport);

/**
 * Записывает всю конфигурацию.
 * Асинхронная операция.
 * @param ioport Порт ввода-вывода.
 * @return Код ошибки.
 */
extern err_t pca9555_write_state(pca9555_t* ioport);

/**
 * Получает входное состояние пина.
 * Требует отдельного чтения значений из PCA9555.
 * @param ioport Порт ввода-вывода.
 * @param pin Пин.
 * @return Входное состояние пина.
 */
extern pca9555_pin_state_t pca9555_pin_state(pca9555_t* ioport, pca9555_pin_t pin);

/**
 * Получает выходное состояние пина.
 * @param ioport Порт ввода-вывода.
 * @param pin Пин.
 * @return Выходное состояние пина.
 */
extern pca9555_pin_state_t pca9555_pin_output_state(pca9555_t* ioport, pca9555_pin_t pin);

/**
 * Получает пины с заданным входным состоянием.
 * Требует отдельного чтения значений из PCA9555.
 * @param ioport Порт ввода-вывода.
 * @param state Состояние пинов.
 * @return Пины с заданным состоянием.
 */
extern pca9555_pins_t pca9555_pins_state(pca9555_t* ioport, pca9555_pin_state_t state);

/**
 * Получает пины с заданным выходным состоянием.
 * @param ioport Порт ввода-вывода.
 * @param state Состояние пинов.
 * @return Пины с заданным состоянием.
 */
extern pca9555_pins_t pca9555_pins_output_state(pca9555_t* ioport, pca9555_pin_state_t state);

/**
 * Устанавливает состояние заданных пинов.
 * Требует отдельной записи значений в PCA9555.
 * @param ioport Порт ввода-вывода.
 * @param pins Пины.
 * @param state Состояние пинов.
 */
extern void pca9555_set_pins_state(pca9555_t* ioport, pca9555_pins_t pins, pca9555_pin_state_t state);

/**
 * Записывает значения выходов.
 * Асинхронная операция.
 * @param ioport Порт ввода-вывода.
 * @return Код ошибки.
 */
extern err_t pca9555_write_pins_state(pca9555_t* ioport);

/**
 * Получает инвертирование входа пина.
 * Требует отдельного чтения значений из PCA9555.
 * @param ioport Порт ввода-вывода.
 * @param pin Пин.
 * @return Инвертирование пина.
 */
extern pca9555_pin_inversion_t pca9555_pin_inversion(pca9555_t* ioport, pca9555_pin_t pin);

/**
 * Получает пины с заданным инвертированием.
 * Требует отдельного чтения значений из PCA9555.
 * @param ioport Порт ввода-вывода.
 * @param inversion Инвертирование пинов.
 * @return Пины с заданным инвертированием.
 */
extern pca9555_pins_t pca9555_pins_inversion(pca9555_t* ioport, pca9555_pin_inversion_t inversion);

/**
 * Устанавливает инвертирование входа заданных пинов.
 * Требует отдельной записи значений в PCA9555.
 * @param ioport Порт ввода-вывода.
 * @param pins Пины.
 * @param inversion Инвертирование пинов.
 */
extern void pca9555_set_pins_inversion(pca9555_t* ioport, pca9555_pins_t pins, pca9555_pin_inversion_t inversion);

/**
 * Записывает значения инвертирования входов.
 * Асинхронная операция.
 * @param ioport Порт ввода-вывода.
 * @return Код ошибки.
 */
extern err_t pca9555_write_pins_inversion(pca9555_t* ioport);

/**
 * Получает направление пина.
 * Требует отдельного чтения значений из PCA9555.
 * @param ioport Порт ввода-вывода.
 * @param pin Пин.
 * @return Направление пина.
 */
extern pca9555_pin_direction_t pca9555_pin_direction(pca9555_t* ioport, pca9555_pin_t pin);

/**
 * Получает пины с заданным направлением.
 * Требует отдельного чтения значений из PCA9555.
 * @param ioport Порт ввода-вывода.
 * @param direction Направление пинов.
 * @return Пины с заданным направлением.
 */
extern pca9555_pins_t pca9555_pins_direction(pca9555_t* ioport, pca9555_pin_direction_t direction);

/**
 * Устанавливает направление заданных пинов.
 * Требует отдельной записи значений в PCA9555.
 * @param ioport Порт ввода-вывода.
 * @param pins Пины.
 * @param direction Направление пинов.
 */
extern void pca9555_set_pins_direction(pca9555_t* ioport, pca9555_pins_t pins, pca9555_pin_direction_t direction);

/**
 * Записывает направление выходов.
 * Асинхронная операция.
 * @param ioport Порт ввода-вывода.
 * @return Код ошибки.
 */
extern err_t pca9555_write_pins_direction(pca9555_t* ioport);

#endif	/* PCA9555_H */
