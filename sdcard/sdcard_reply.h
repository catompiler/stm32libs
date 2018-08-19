/**
 * @file sdcard_reply.h Декларация ответов SD-карты.
 */

#ifndef SDCARD_REPLY_H_
#define SDCARD_REPLY_H_

#include <stdint.h>
#include <assert.h>
#include "sdcard_reg.h"


//! Размер ответа R1 SD-карты.
#define SDCARD_REPLY_R1_SIZE 1
//! Размер данных ответа R1 SD-карты.
#define SDCARD_REPLY_R1_DATA_SIZE 1

// Поля ответа R1.
//! Ноль.
#define SDCARD_REPLY_R1_ZERO 0x80
//! Ошибка параметра.
#define SDCARD_REPLY_R1_PARAM_ERROR 0x40
//! Ошибка адреса.
#define SDCARD_REPLY_R1_ADDR_ERROR 0x20
//! Ошибка последовательности стирания.
#define SDCARD_REPLY_R1_ERASE_SEQ_ERROR 0x10
//! Ошибка CRC команды.
#define SDCARD_REPLY_R1_COM_CRC_ERROR 0x8
//! Недопустимая команда.
#define SDCARD_REPLY_R1_ILLEGAL_COMMAND 0x4
//! Сброс стирания.
#define SDCARD_REPLY_R1_ERASE_RESET 0x2
//! В состоянии IDLE.
#define SDCARD_REPLY_R1_IN_IDLE_STATE 0x1

//! Структура ответа R1 SD-карты.
#pragma pack(push, 1)
typedef struct _SD_Card_Reply_R1 {
    uint8_t data; //!< Данные.
} sdcard_reply_r1_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_reply_r1_t) == SDCARD_REPLY_R1_SIZE, "Invalid size of sdcard r1 reply!");


//! Размер ответа R2 SD-карты.
#define SDCARD_REPLY_R2_SIZE 2
//! Размер данных ответа R2 SD-карты.
#define SDCARD_REPLY_R2_DATA_SIZE 1

// Поля ответа R2.
//! Выход за пределы.
#define SDCARD_REPLY_R2_OUT_OF_RANGE 0x80
//! Передапись CSD.
#define SDCARD_REPLY_R2_CSD_OVERWRITE 0x80
//! Параметр стирания.
#define SDCARD_REPLY_R2_ERASE_PARAM 0x40
//! Запись в защищённый от записи блок.
#define SDCARD_REPLY_R2_WP_VIOLATION 0x20
//! Ошибка ECC.
#define SDCARD_REPLY_R2_CARD_ECC_FAILED 0x10
//! Ошибка контроллера карты.
#define SDCARD_REPLY_R2_CC_ERROR 0x8
//! Общая или неизвестная ошибка.
#define SDCARD_REPLY_R2_ERROR 0x4
//! Стирание защищённого от записи блока.
#define SDCARD_REPLY_R2_WP_ERASE_SKIP 0x2
//! Ошибка последовательности блокировки/разблокировки.
#define SDCARD_REPLY_R2_LOCK_UNLOCK_CMD_FAILED 0x2
//! Карта заблокирована.
#define SDCARD_REPLY_R2_CARD_IS_LOCKED 0x1

//! Структура ответа R2 SD-карты.
#pragma pack(push, 1)
typedef struct _SD_Card_Reply_R2 {
    sdcard_reply_r1_t r1; //!< Ответ R1.
    uint8_t data; //!< Данные.
} sdcard_reply_r2_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_reply_r2_t) == SDCARD_REPLY_R2_SIZE, "Invalid size of sdcard r2 reply!");


//! Размер ответа R3 SD-карты.
#define SDCARD_REPLY_R3_SIZE 5
//! Размер данных ответа R3 SD-карты.
#define SDCARD_REPLY_R3_DATA_SIZE 4

// Поля ответа R3.
//! Карта занята.
#define SDCARD_REPLY_R3_OCR_BUSY 0x80000000
//! Вместимость карты.
#define SDCARD_REPLY_R3_OCR_CCS 0x40000000
//! Статус карты UHS-II.
#define SDCARD_REPLY_R3_OCR_UHSII_STATUS 0x20000000
//! Принято переключения на 1.8V.
#define SDCARD_REPLY_R3_OCR_S18A 0x1000000
//! Напряжение 3.5-3.6V.
#define SDCARD_REPLY_R3_OCR_35_36V 0x800000
//! Напряжение 3.4-3.5V.
#define SDCARD_REPLY_R3_OCR_34_35V 0x400000
//! Напряжение 3.3-3.4V.
#define SDCARD_REPLY_R3_OCR_33_34V 0x200000
//! Напряжение 3.2-3.3V.
#define SDCARD_REPLY_R3_OCR_32_33V 0x100000
//! Напряжение 3.1-3.2V.
#define SDCARD_REPLY_R3_OCR_31_32V 0x80000
//! Напряжение 3.0-3.1V.
#define SDCARD_REPLY_R3_OCR_30_31V 0x40000
//! Напряжение 2.9-3.0V.
#define SDCARD_REPLY_R3_OCR_29_30V 0x20000
//! Напряжение 2.8-2.9V.
#define SDCARD_REPLY_R3_OCR_28_29V 0x10000
//! Напряжение 2.7-2.8V.
#define SDCARD_REPLY_R3_OCR_27_28V 0x8000

//! Маска напряжений.
#define SDCARD_REPLY_R3_OCR_VOLTAGE 0xff8000
//! Маска напряжения 3.3V.
#define SDCARD_REPLY_R3_OCR_VOLTAGE_33V 0x100000

//! Структура ответа R3 SD-карты.
#pragma pack(push, 1)
typedef struct _SD_Card_Reply_R3 {
    sdcard_reply_r1_t r1; //!< Ответ R1.
    union {
        uint32_t data; //!< Данные OCR.
        sdcard_ocr_t ocr; //!< OCR.
    };
} sdcard_reply_r3_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_reply_r3_t) == SDCARD_REPLY_R3_SIZE, "Invalid size of sdcard r3 reply!");


//! Размер ответа R7 SD-карты.
#define SDCARD_REPLY_R7_SIZE 5
//! Размер данных ответа R7 SD-карты.
#define SDCARD_REPLY_R7_DATA_SIZE 4

// Поля ответа R7.
//! Версия команды.
#define SDCARD_REPLY_R7_COMMAND_VERSION_MASK 0xf0000000
#define SDCARD_REPLY_R7_COMMAND_VERSION_OFFSET 27
//! Принятое напряжение.
#define SDCARD_REPLY_R7_VOLTAGE_ACCEPTED_MASK 0xf00
#define SDCARD_REPLY_R7_VOLTAGE_ACCEPTED_OFFSET 7
//! Маска проверки.
#define SDCARD_REPLY_R7_CHECK_PATTERN_MASK 0xff
#define SDCARD_REPLY_R7_CHECK_PATTERN_OFFSET 0

//! Структура ответа R7 SD-карты.
#pragma pack(push, 1)
typedef struct _SD_Card_Reply_R7 {
    sdcard_reply_r1_t r1; //!< Ответ R1.
    uint32_t data; //!< Данные.
} sdcard_reply_r7_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_reply_r7_t) == SDCARD_REPLY_R7_SIZE, "Invalid size of sdcard r7 reply!");

//! Перечисления типа ответа SD-карты.
typedef enum _SD_Card_Reply_Type {
    SDCARD_REPLY_R1 = 0, //!< Ответ R1.
    SDCARD_REPLY_R1b = 1, //!< Ответ R1 с ожиданием busy.
    SDCARD_REPLY_R2 = 2, //!< Ответ R2.
    SDCARD_REPLY_R3 = 3, //!< Ответ R3.
    SDCARD_REPLY_R7 = 4, //!< Ответ R7.
} sdcard_reply_type_t;

//! Общий ответ SD-карты.
typedef union _SD_Card_Reply {
    sdcard_reply_r1_t r1; //!< Ответ R1.
    sdcard_reply_r2_t r2; //!< Ответ R2.
    sdcard_reply_r3_t r3; //!< Ответ R3.
    sdcard_reply_r7_t r7; //!< Ответ R7.
} sdcard_reply_t;


//! Проверка отсутствия данных кроме r1 в ответе SD-карты.
#define SDCARD_REPLY_ONLY_R1(R1) (((R1) & SDCARD_REPLY_R1_ILLEGAL_COMMAND) ||\
                                  ((R1) & SDCARD_REPLY_R1_COM_CRC_ERROR))


#endif /* SDCARD_REPLY_H_ */
