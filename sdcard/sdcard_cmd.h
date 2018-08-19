/**
 * @file sdcard_cmd.h Декларация типа команды SD-карты.
 */

#ifndef SDCARD_CMD_H_
#define SDCARD_CMD_H_

#include <stdint.h>
#include <assert.h>


//! Размер команды SD-карты.
#define SDCARD_CMD_SIZE 6
//! Размер команды SD-карты без поля контрольной суммы.
#define SDCARD_CMD_SIZE_CRC 5

// Поля команды.
//! Стартовый бит команды.
#define SDCARD_CMD_START_BIT 0x80
//! Бит передачи команды.
#define SDCARD_CMD_TRANSMISSION_BIT 0x40
//! Индекс.
#define SDCARD_CMD_INDEX_MASK 0x3f
#define SDCARD_CMD_INDEX_OFFSET 0
//! Контрольная сумма.
#define SDCARD_CMD_CRC7_MASK 0xfe
#define SDCARD_CMD_CRC7_OFFSET 1
//! Стоповый бит.
#define SDCARD_CMD_STOP_BIT 0x1

//! Структура команды SD-карты.
#pragma pack(push, 1)
typedef struct _SD_Card_Cmd {
    uint8_t index_sb_tb; //!< Стартовый бит, бит передачи, индекс команды.
    uint32_t argument; //!< Аргумент команды.
    uint8_t crc_eb; //!< Контрольная сумма, стоповый бит.
} sdcard_cmd_t;
#pragma pack(pop)

//! Инициализация команды SD-карты по месту объявления.
#define SDCARD_CMD_MAKE(index, arg, crc)\
            {0x40 | ((index) & 0x3f),\
            ((((arg) >> 24) & 0xff) <<  0)|\
            ((((arg) >> 16) & 0xff) <<  8)|\
            ((((arg) >>  8) & 0xff) << 16)|\
            ((((arg) >>  0) & 0xff) << 24),\
            (((crc) & 0x7f) << 1) | 0x1}

static_assert(sizeof(sdcard_cmd_t) == SDCARD_CMD_SIZE, "Invalid size of sdcard cmd!");


/*
 * Команды карты памяти.
 */
//! Программный сброс.
#define SDCARD_CMD_GO_IDLE_STATE 0

//! Инициализация карты MMC.
#define SDCARD_CMD_SEND_OP_COND 1

//! Проверка поддержки напряжения.
#define SDCARD_CMD_SEND_IF_COND 8

//! Отправка регистра CSD.
#define SDCARD_CMD_SEND_CSD 9

//! Отправка регистра CID.
#define SDCARD_CMD_SEND_CID 10

//! Остановка передачи.
#define SDCARD_CMD_STOP_TRANSMISSION 12

//! Отправка состояния.
#define SDCARD_CMD_SEND_STATUS 13

//! Установка размера блока.
#define SDCARD_CMD_SET_BLOCKLEN 16

//! Чтение одного блока.
#define SDCARD_CMD_READ_SINGLE_BLOCK 17
//! Аргумент - адрес блока.
#define SDCARD_CMD_READ_SINGLE_BLOCK_ADDRESS 0xffffffff

//! Чтение нескольких блоков.
#define SDCARD_CMD_READ_MULTIPLE_BLOCK 18
//! Аргумент - адрес блока.
#define SDCARD_CMD_READ_MULTIPLE_BLOCK_ADDRESS 0xffffffff

//! Запись одного блока.
#define SDCARD_CMD_WRITE_SINGLE_BLOCK 24
//! Аргумент - адрес блока.
#define SDCARD_CMD_WRITE_SINGLE_BLOCK_ADDRESS 0xffffffff

//! Запись нескольких блоков.
#define SDCARD_CMD_WRITE_MULTIPLE_BLOCK 25
//! Аргумент - адрес блока.
#define SDCARD_CMD_WRITE_MULTIPLE_BLOCK_ADDRESS 0xffffffff

//! Установка начального адреса стирания блоков карты SD.
#define SDCARD_CMD_ERASE_WR_BLK_START_ADDR 32
//! Аргумент - адрес блока.
#define SDCARD_CMD_ERASE_WR_BLK_START_ADDR_ADDRESS 0xffffffff

//! Установка конечного адреса стирания блоков карты SD.
#define SDCARD_CMD_ERASE_WR_BLK_END_ADDR 33
//! Аргумент - адрес блока.
#define SDCARD_CMD_ERASE_WR_BLK_END_ADDR_ADDRESS 0xffffffff

//! Установка начального адреса стирания групп блоков карты MMC.
#define SDCARD_CMD_MMC_TAG_ERASE_GROUP_START 35
//! Аргумент - адрес блока.
#define SDCARD_CMD_MMC_TAG_ERASE_GROUP_START_ADDRESS 0xffffffff

//! Установка конечного адреса стирания групп блоков карты MMC.
#define SDCARD_CMD_MMC_TAG_ERASE_GROUP_END 36
//! Аргумент - адрес блока.
#define SDCARD_CMD_MMC_TAG_ERASE_GROUP_END_ADDRESS 0xffffffff

//! Стирания блоков.
#define SDCARD_CMD_ERASE 38

//! Установка следующей команды приложения.
#define SDCARD_CMD_APP_CMD 55

//! Чтение регистра OCR.
#define SDCARD_CMD_READ_OCR 58

//! Включение/выключение проверки CRC.
#define SDCARD_CMD_CRC_ON_OFF 59
//! Аргумент - бит CRC option.
#define SDCARD_CMD_CRC_ON_OFF_CRC_OPTION 0x1

//! Получения числа успешно записанных блоков.
//! Данные ответа - 32 бит число.
#define SDCARD_ACMD_SEND_NUM_WR_BLOCKS 22

//! Установка число записываемых блоков для стирания.
#define SDCARD_ACMD_SET_WR_BLK_ERASE_COUNT 23
//! Аргумент - 23 битное число записываемых блоков.
#define SDCARD_ACMD_SET_WR_BLK_ERASE_COUNT_NUMBER 0x7FFFFF

//! Инициализация карты.
#define SDCARD_ACMD_SD_SEND_OP_COND 41
//! Аргумент - бит HCS.
#define SDCARD_ACMD_SD_SEND_OP_COND_HCS 0x40000000

//! Управление 50 кОм подтяжкой на пину CardDetect карты.
#define SDCARD_ACMD_SET_CLR_CARD_DETECT 42
//! Аргумент - подключение подтяжки.
#define SDCARD_ACMD_SET_CLR_CARD_DETECT_SET_CD 0x1

#endif /* SDCARD_CMD_H_ */
