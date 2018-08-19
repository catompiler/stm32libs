/**
 * @file sdcard_token.h Декларация маркеров данных.
 */

#ifndef SDCARD_TOKEN_H_
#define SDCARD_TOKEN_H_

#include <stdint.h>


//! Размер контрольного маркера.
#define SDCARD_CONTROL_TOKEN_SIZE 1

//! Тип контрольного маркера.
typedef uint8_t sdcard_control_token_t;

// Маркер принятия данных картой при записи.
//! Маска полезных данных.
#define SDCARD_DATA_RESP_TOKEN_MASK 0x1f
//! Сброшенный бит.
#define SDCARD_DATA_RESP_TOKEN_BIT_ZERO 0x10
//! Состояние.
#define SDCARD_DATA_RESP_TOKEN_STATUS_MASK 0xe
#define SDCARD_DATA_RESP_TOKEN_STATUS_OFFSET 1
//! Установленный бит.
#define SDCARD_DATA_RESP_TOKEN_BIT_ONE 0x1
// Поле состояния.
//! Данные приняты.
#define SDCARD_DATA_RESP_TOKEN_STATUS_DATA_ACCEPTED 0x2
//! Данные отброшены из-за ошибки CRC.
#define SDCARD_DATA_RESP_TOKEN_STATUS_CRC_ERROR 0x5
//! Данные отброшены из-за ошибки записи.
#define SDCARD_DATA_RESP_TOKEN_STATUS_WRITE_ERROR 0x6

// Маркеры при одиночном чтении или записи
// и при множественном чтении.
//! Маркер начала блока данных.
#define SDCARD_START_BLOCK_TOKEN 0xfe

// Маркеры начала при множественной записи
// (Multiple Block Write).
//! Маркер начала блока данных.
#define SDCARD_MBW_START_BLOCK_TOKEN 0xfc
//! Маркер окончания передачи.
#define SDCARD_MBW_STOP_TRAN_TOKEN 0xfd

// Маркеры ошибки.
// Чтение.
//! Маска полезных данных.
#define SDCARD_DATA_ERROR_TOKEN_MASK 0xf
//! Сброшенные биты.
#define SDCARD_DATA_ERROR_TOKEN_ZERO_MASK 0xf0
#define SDCARD_DATA_ERROR_TOKEN_ZERO_OFFSET 4
//! Выход за пределы.
#define SDCARD_DATA_ERROR_TOKEN_OUT_OF_RANGE 0x8
//! Ошибка ECC.
#define SDCARD_DATA_ERROR_TOKEN_CARD_ECC_FAILED 0x4
//! Ошибка контроллера карты.
#define SDCARD_DATA_ERROR_TOKEN_CC_ERROR 0x2
//! Общая ошибка.
#define SDCARD_DATA_ERROR_TOKEN_ERROR 0x1


#endif /* SDCARD_TOKEN_H_ */
