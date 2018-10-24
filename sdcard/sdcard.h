/**
 * @file sdcard.h Библиотека для работы с SD-картами по SPI.
 */

#ifndef SDCARD_H_
#define SDCARD_H_

#include <stm32f10x.h>
#include "defs/defs.h"
#include "spi/spi.h"
#include "gpio/gpio.h"
#include "errors/errors.h"
#include "future/future.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sdcard_cmd.h"
#include "sdcard_reply.h"
#include "sdcard_token.h"
#include "sdcard_reg.h"


/*
 * Размер блока и ограничения SD-карты.
 */

//! Размер блока SD-карты.
#define SDCARD_BLOCK_SIZE 512

//! Максимальный номер читаемого/записываемого сектора для карт MMC/SDSC.
//! Ограничен из-за трансляции секторной адресации в байтовую.
#define SDCARD_SDSC_MMC_MAX_RW_SECTOR_NUMBER (UINT32_MAX >> 9) // UINT_MAX / 512.

//! Максимальное число читаемых/записываемых секторов.
//! Ограничено из-за трансляции числа секторов в размер читаемых данных.
//! Равно 4 Гб - 512 б данных, иначе 32 битная переменная
//! размера считываемых/записываемых данных переполнится.
#define SDCARD_MAX_RW_SECTORS_COUNT (UINT32_MAX >> 9) // UINT_MAX / 512.


/*
 * Ошибки SD-карты.
 */
//! Начальный код ошибки SD-карты.
#define E_SDCARD (E_USER + 10)
//! Неподдерживаемая карта.
#define E_SDCARD_INVALID (E_SDCARD + 0)
//! Принят некорректный токен данных.
#define E_SDCARD_INVALID_TOKEN (E_SDCARD + 1)
//! Ошибка записи данных картой.
#define E_SDCARD_WRITE_ERROR (E_SDCARD + 2)
//! Ошибки в статусе карты.
#define E_SDCARD_STATUS (E_SDCARD + 3)
//! Карта заблокирована.
#define E_SDCARD_CARD_IS_LOCKED (E_SDCARD_STATUS + 0)
//! Ошибка последовательности блокировки/разблокировки.
#define E_SDCARD_LOCK_UNLOCK_CMD_FAILED (E_SDCARD_STATUS + 1)
//! Стирание защищённого от записи блока.
#define E_SDCARD_WP_ERASE_SKIP (E_SDCARD_STATUS + 1)
//! Общая или неизвестная ошибка.
#define E_SDCARD_ERROR (E_SDCARD_STATUS + 2)
//! Ошибка контроллера карты.
#define E_SDCARD_CC_ERROR (E_SDCARD_STATUS + 3)
//! Ошибка ECC.
#define E_SDCARD_CARD_ECC_FAILED (E_SDCARD_STATUS + 4)
//! Запись в защищённый от записи блок.
#define E_SDCARD_WP_VIOLATION (E_SDCARD_STATUS + 5)
//! Параметр стирания.
#define E_SDCARD_ERASE_PARAM (E_SDCARD_STATUS + 6)
//! Передапись CSD.
#define E_SDCARD_CSD_OVERWRITE (E_SDCARD_STATUS + 7)
//! Выход за пределы.
#define E_SDCARD_OUT_OF_RANGE (E_SDCARD_STATUS + 7)
//! В состоянии IDLE.
#define E_SDCARD_IN_IDLE_STATE (E_SDCARD_STATUS + 8)
//! Сброс стирания.
#define E_SDCARD_ERASE_RESET (E_SDCARD_STATUS + 9)
//! Недопустимая команда.
#define E_SDCARD_ILLEGAL_COMMAND (E_SDCARD_STATUS + 10)
//! Ошибка CRC команды.
#define E_SDCARD_COM_CRC_ERROR (E_SDCARD_STATUS + 11)
//! Ошибка последовательности стирания.
#define E_SDCARD_ERASE_SEQ_ERROR (E_SDCARD_STATUS + 12)
//! Ошибка адреса.
#define E_SDCARD_ADDR_ERROR (E_SDCARD_STATUS + 13)
//! Ошибка параметра.
#define E_SDCARD_PARAM_ERROR (E_SDCARD_STATUS + 14)


//! Идентификатор передачи SPI.
#define SDCARD_DEFAULT_TRANSFER_ID 0xCD //mmC sD CarD.


//! Перечисление скоростей SPI.
typedef enum _SD_Card_Spi_Speed {
    SDCARD_SPI_SPEED_LOW = 0, //!< Низкая скорость.
    SDCARD_SPI_SPEED_HIGH = 1 //!< Высокая скорость.
} sdcard_spi_speed_t;


//! Каллбэк установки скорости SPI.
//! Возвращает флаг установки скорости.
typedef bool (*sdcard_set_spi_speed_t)(sdcard_spi_speed_t speed);


//! Тип тайм-аута для работы с SD-картой.
typedef enum _SD_Card_Time_Out {
    SDCARD_TIMEOUT_INIT = 0, //!< Время ожидания инициализации карты.
    SDCARD_TIMEOUT_BUSY = 1, //!< Время ожидания занятости карты.
    SDCARD_TIMEOUT_READ = 2 //!< Время ожидания чтения данных картой.
} sdcard_timeout_t;

//! Каллбэк начала тайм-аута ожидания SD-карты.
typedef void (*sdcard_timeout_begin_t)(sdcard_timeout_t timeout);
//! Каллбэк окончания тайм-аута ожидания SD-карты.
typedef void (*sdcard_timeout_end_t)(void);


//! Структура инициализации SD-карты.
typedef struct _SD_Card_Init {
    spi_bus_t* spi; //!< Шина SPI.
    GPIO_TypeDef* gpio_cs; //!< Порт CS.
    gpio_pin_t pin_cs; //!< Пин CS.
    spi_transfer_id_t transfer_id; //!< Идентификатор передачи SPI.
    sdcard_set_spi_speed_t set_spi_speed; //!< Функция установки скорости SPI.
    sdcard_timeout_begin_t timeout_begin; //!< Функция начала таймаута.
    sdcard_timeout_end_t timeout_end; //!< Функция завершения таймаута.
} sdcard_init_t;


//! Перечисление типа SD-карты.
typedef enum _SD_Card_Type {
    SDCARD_TYPE_UNKNOWN = 0, //!< Неизвестный тип карты.
    SDCARD_TYPE_MMC = 1, //!< MMC.
    SDCARD_TYPE_SDSCv1 = 2, //!< SDSC версии 1.
    SDCARD_TYPE_SDSCv2 = 3, //!< SDSC версии 2.
    SDCARD_TYPE_SDHC_SDXC = 4 //!< SDHC или SDXC.
} sdcard_type_t;


//! Число сообщений SPI.
#define SDCARD_SPI_MESSAGES_COUNT 1


//! Структура SD-карты.
typedef struct _SD_Card {
    spi_bus_t* spi; //!< Шина SPI.
    GPIO_TypeDef* gpio_cs; //!< Порт CS.
    gpio_pin_t pin_cs; //!< Пин CS.
    spi_transfer_id_t transfer_id; //!< Идентификатор передачи SPI.
    sdcard_type_t card_type; //!< Тип SD-карты.
    sdcard_set_spi_speed_t set_spi_speed; //!< Функция установки скорости SPI.
    sdcard_timeout_begin_t timeout_begin; //!< Функция начала таймаута.
    sdcard_timeout_end_t timeout_end; //!< Функция завершения таймаута.
    future_t future; //!< Будущее.
    spi_message_t messages[SDCARD_SPI_MESSAGES_COUNT]; //!< Сообщения SPI.
    bool timeout; //!< Флаг таймаута SD-карты.
    bool crc_enabled; //!< Флаг включения CRC.
    bool initialized; //!< Флаг инициализации SD-карты.
    sdcard_cid_t cid; //!< Регистр идентификации карты.
    sdcard_csd_t csd; //!< Регистр со специфичными для карты данными.
    uint16_t rw_block_size; //!< Размер читаемого/записываемого блока.
    uint16_t er_block_size; //!< Размер стираемого блока.
} sdcard_t;


/**
 * Инициализирует экземпляр структуры SD-памяти.
 * @param sdcard SD-карта.
 * @param init Данные для инициализации.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_init(sdcard_t* sdcard, sdcard_init_t* init);

/**
 * Функция обратного вызова шины SPI.
 * @param sdcard SD-карта.
 * @return Флаг обработки.
 */
EXTERN bool sdcard_spi_callback(sdcard_t* sdcard);

/**
 * Функция обработки таймаута SD-карты.
 * @param sdcard SD-карта.
 */
EXTERN void sdcard_timeout(sdcard_t* sdcard);

/**
 * Получает флаг идентификации SD-карты.
 * @param sdcard SD-карта.
 * @return Флаг идентификации SD-карты.
 */
EXTERN bool sdcard_identified(sdcard_t* sdcard);

/**
 * Получает флаг инициализации SD-карты.
 * @param sdcard SD-карта.
 * @return Флаг инициализации SD-карты.
 */
EXTERN bool sdcard_initialized(sdcard_t* sdcard);

/**
 * Получает тип карты памяти.
 * @param sdcard SD-карта.
 * @return Тип карты памяти.
 */
EXTERN sdcard_type_t sdcard_card_type(sdcard_t* sdcard);

/**
 * Получает размер SD-карты в байтах.
 * @param sdcard SD-карта.
 * @return Размер.
 */
EXTERN uint64_t sdcard_capacity(sdcard_t* sdcard);

/**
 * Получает размер сектора SD-карты.
 * @param sdcard SD-карта.
 * @return Размер сектора.
 */
EXTERN uint16_t sdcard_sector_size(sdcard_t* sdcard);

/**
 * Получает число секторов SD-карты.
 * @param sdcard SD-карта.
 * @return Число секторов.
 */
EXTERN uint32_t sdcard_sectors_count(sdcard_t* sdcard);

/**
 * Получает размер блока чтения.
 * @param sdcard SD-карта.
 * @return Размер блока.
 */
EXTERN uint16_t sdcard_block_len(sdcard_t* sdcard);

/**
 * Получает размер блока стирания.
 * @param sdcard SD-карта.
 * @return Размер блока.
 */
EXTERN uint16_t sdcard_erase_block_len(sdcard_t* sdcard);

/**
 * Получает флаг поддержки чтения части блока данных.
 * @param sdcard SD-карта.
 * @return Флаг поддержки чтения части блока данных.
 */
EXTERN bool sdcard_partial_block_read(sdcard_t* sdcard);

/**
 * Получает флаг поддержки записи части блока данных.
 * @param sdcard SD-карта.
 * @return Флаг поддержки записи части блока данных.
 */
EXTERN bool sdcard_partial_block_write(sdcard_t* sdcard);

/**
 * Получает флаг поддержки невыровненного чтения блока данных.
 * @param sdcard SD-карта.
 * @return Флаг поддержки невыровненного чтения блока данных.
 */
EXTERN bool sdcard_misalign_block_read(sdcard_t* sdcard);

/**
 * Получает флаг поддержки невыровненной записи блока данных.
 * @param sdcard SD-карта.
 * @return Флаг поддержки невыровненной записи блока данных.
 */
EXTERN bool sdcard_misalign_block_write(sdcard_t* sdcard);

/**
 * Инициализирует команду SD-карты.
 * Считает CRC.
 * @param cmd Команда.
 * @param index Индекс.
 * @param argument Аргумент.
 */
EXTERN void sdcard_cmd_setup(sdcard_cmd_t* cmd, uint8_t index, uint32_t argument);

/**
 * Выбирает SD-карту.
 * Проверяет шину SPI на занятость.
 * Опускает пин CS.
 * По возможности генерирует определённое
 * количество циклов SCK до и после
 * отпускания пина.
 * @param sdcard SD-карта.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_select(sdcard_t* sdcard);

/**
 * Отменяет выбор SD-карты.
 * Поднимает пин CS.
 * По возможности генерирует определённое
 * количество циклов SCK до и после
 * поднятия пина.
 * @param sdcard SD-карта.
 */
EXTERN void sdcard_deselect(sdcard_t* sdcard);

/**
 * Инициализирует карту памяти.
 * @param sdcard SD-карта.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_init_card(sdcard_t* sdcard);

/**
 * Ожидает окончания занятости карты.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_wait_busy(sdcard_t* sdcard);

/**
 * Передаёт команду SD-карте.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param cmd Команда.
 * @param reply_type Тип ответа.
 * @param reply Ответ.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_send_cmd(sdcard_t* sdcard, const sdcard_cmd_t* cmd,
                             sdcard_reply_type_t reply_type, sdcard_reply_t* reply);

/**
 * Передаёт команду приложения SD-карте
 * (Передаёт сперва CMD55).
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param cmd Команда.
 * @param reply_type Тип ответа.
 * @param reply Ответ.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_send_acmd(sdcard_t* sdcard, const sdcard_cmd_t* cmd,
                              sdcard_reply_type_t reply_type, sdcard_reply_t* reply);

/**
 * Читает состояние (R2) SD-карты.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param status Состояние.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_read_status(sdcard_t* sdcard, sdcard_status_t* status);

/**
 * Читает регистр OCR SD-карты.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param ocr Регистр.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_read_ocr(sdcard_t* sdcard, sdcard_ocr_t* ocr);

/**
 * Читает регистр CSD SD-карты.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param csd Регистр.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_read_csd(sdcard_t* sdcard, sdcard_csd_t* csd);

/**
 * Читает регистр CID SD-карты.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param cid Регистр.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_read_cid(sdcard_t* sdcard, sdcard_cid_t* cid);

/**
 * Прекращает передачу данных.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_stop_transmission(sdcard_t* sdcard);

/**
 * Устанавливает флаг проверки контрольной суммы.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param enabled Флаг проверки контрольной суммы.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_set_crc_enabled(sdcard_t* sdcard, bool enabled);

/**
 * Устанавливает размер блока чтения/записи/блокировки.
 * Размер блока чтения/записи устанавливается
 * только для карт SDSCv1.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param blocklen Размер блока.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_set_block_len(sdcard_t* sdcard, uint32_t blocklen);

/**
 * Получает число успешно записанных блоков данных.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param blocks Число блоков.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_blocks_written(sdcard_t* sdcard, uint32_t* blocks);

/**
 * Управление подтяжкой 50 кОм
 * на пине CardDetect SD-карты.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param enable Включение подтяжки.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_set_card_detect(sdcard_t* sdcard, bool enable);

/**
 * Читает данные из SD-карты.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param address Адрес блока.
 * @param data Данные.
 * @param size Размер данных.
 * @param readed Размер фактически прочитанных данных, может быть NULL.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_read(sdcard_t* sdcard, uint32_t address, void* data, size_t size, size_t* readed);

/**
 * Записывает данные в SD-карту.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param address Адрес блока.
 * @param data Данные.
 * @param size Размер данных.
 * @param written Размер фактически записанных данных, может быть NULL.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_write(sdcard_t* sdcard, uint32_t address, const void* data, size_t size, size_t* written);

/**
 * Стирает кластеры на SD-карте.
 * Стирание производится кратно erase_size.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param address_start Начальный адрес.
 * @param address_end Конечный адрес.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_erase(sdcard_t* sdcard, uint32_t address_start, uint32_t address_end);

/**
 * Читает секторы из SD-карты.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param number Номер сектора.
 * @param count Число секторов.
 * @param data Данные.
 * @param readed Размер фактически прочитанных данных, может быть NULL.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_read_sector(sdcard_t* sdcard, uint32_t number, size_t count, void* data, size_t* readed);


/**
 * Записывает секторы в SD-карту.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param number Номер сектора.
 * @param count Число секторов.
 * @param data Данные.
 * @param written Размер фактически записанных данных, может быть NULL.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_write_sector(sdcard_t* sdcard, uint32_t number, size_t count, const void* data, size_t* written);
/**
 * Стирает кластеры на SD-карте.
 * Стирание производится кратно erase_size.
 * Должна быть обрамлена вызовами
 * sdcard_select/sdcard_deselect.
 * @param sdcard SD-карта.
 * @param number Номер сектора.
 * @param count Число секторов.
 * @return Код ошибки.
 */
EXTERN err_t sdcard_erase_sector(sdcard_t* sdcard, uint32_t number, size_t count);


/*
 * FatFS diskio.
 */
#ifdef USE_SDCARD_FATFS_DISKIO

#include "sdcard_diskio.h"

static size_t sdcard_disks_count = 0;
static sdcard_t* sdcard_disks = NULL;

/**
 * Устанавливает для diskio массив SD-карт.
 * @param sdcards Массив SD-карт.
 * @param count Число SD-карт.
 */
static void sdcard_setup_diskio(sdcard_t* sdcards, size_t count)
{
    sdcard_disks = sdcards;
    sdcard_disks_count = count;
}

/**
 * Получает SD-карту по номеру диска.
 * @param pdrv Номер диска.
 * @return SD-карта.
 */
static sdcard_t* sdcard_diskio_get_sdcard(BYTE pdrv)
{
    if(pdrv >= sdcard_disks_count) return NULL;

    return &sdcard_disks[pdrv];
}

DSTATUS disk_initialize(BYTE pdrv)
{
    sdcard_t* sdcard = sdcard_diskio_get_sdcard(pdrv);
    if(sdcard == NULL) return STA_NOINIT;

    return sdcard_disk_initialize(sdcard);
}


DSTATUS disk_status(BYTE pdrv)
{
    sdcard_t* sdcard = sdcard_diskio_get_sdcard(pdrv);
    if(sdcard == NULL) return STA_NOINIT;

    return sdcard_disk_status(sdcard);
}


DRESULT disk_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
    sdcard_t* sdcard = sdcard_diskio_get_sdcard(pdrv);
    if(sdcard == NULL) return RES_PARERR;

    return sdcard_disk_read(sdcard, buff, sector, count);
}

#if _USE_WRITE
DRESULT disk_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
    sdcard_t* sdcard = sdcard_diskio_get_sdcard(pdrv);
    if(sdcard == NULL) return RES_PARERR;

    return sdcard_disk_write(sdcard, buff, sector, count);
}

#endif // _USE_WRITE

#if _USE_IOCTL
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
    sdcard_t* sdcard = sdcard_diskio_get_sdcard(pdrv);
    if(sdcard == NULL) return RES_PARERR;

    return sdcard_disk_ioctl(sdcard, cmd, buff);
}
#endif // _USE_IOCTL

#endif // USE_SDCARD_FATFS_DISKIO

#endif /* SDCARD_H_ */
