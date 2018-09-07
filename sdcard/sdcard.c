#include "sdcard.h"
#include "utils/utils.h"
#include "crc/crc16_ccitt.h"
#include <string.h>


// Константы.

//! Число пустых байт в начале инициализации SD-карты.
#define SDCARD_INIT_DUMMY_BYTES 10
//! Число байт в ожидании ответа карты.
#define SDCARD_WAIT_REPLY_BYTES 8

//! Число байт до выбора SD-карты.
#define SDCARD_SELECT_BEFORE_BYTES 1
//! Число байт после выбора SD-карты.
#define SDCARD_SELECT_AFTER_BYTES 1
//! Число байт до сниятия выбора SD-карты.
#define SDCARD_DESELECT_BEFORE_BYTES 1
//! Число байт после сниятия выбора SD-карты.
#define SDCARD_DESELECT_AFTER_BYTES 1

//! Значение передаваемых данных по-умолчанию 16 бит.
#define SDCARD_DEFAULT_DATA16 0xffffU
//! Значение передаваемых данных по-умолчанию 8 бит.
#define SDCARD_DEFAULT_DATA8 0xffU
//! Значение получаемых данных в случае занятой SD-карты.
#define SDCARD_BUSY_DATA 0x0
//! Значение получаемых данных в случае отсутствия ответа SD-карты.
#define SDCARD_NOREPLY_DATA 0xff

//! Полином контрольной суммы CRC16.
#define SDCARD_CRC16_POLYNOMIAL 0x1021
#define SDCARD_CRC16_POLYNOMIAL_REVERSED 0x8408

//! Аргумент команды проверки напряжения (SEND_IF_COND).
#define SDCARD_SEND_IF_COND_CHECK 0x1aa


//! Получение границы блока из адреса.
#define SDCARD_ADDRESS_BLOCK(addr) ((addr) & ~((SDCARD_BLOCK_SIZE) - 1))



/*
 * CRC.
 */

/*
  Name  : CRC7 SDCARD
  Poly  : 0x89 x^7 + x^3 + 1
  Init  : 0x0
*/
static const uint8_t crc7_sdcard_table[256] = {
    0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
    0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
    0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
    0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
    0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
    0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
    0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
    0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
    0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
    0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
    0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
    0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
    0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
    0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
    0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
    0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
    0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
    0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
    0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
    0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
    0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
    0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
    0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
    0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
    0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
    0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
    0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
    0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
    0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
    0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
    0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
    0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79,
};

/**
 * Вычисляет контрольную сумму crc7 карты памяти.
 * @param data Данные.
 * @param size Размер данных.
 * @return Контрольная сумма.
 */
static uint8_t crc7_sdcard(const void* data, size_t size)
{
    uint8_t crc = 0x0;

    while (size --)
        crc = crc7_sdcard_table[(crc << 1) ^ *(uint8_t*)data ++];

    return crc;
}

/*
static uint8_t crc7_sdcard_first(void)
{
    return 0x0;
}

static uint8_t crc7_sdcard_next(uint8_t crc, const void* data)
{
    if(data == NULL) return crc;
    return crc7_sdcard_table[(crc << 1) ^ *(uint8_t*)data];
}
*/

/*
 * CRC16-CCITT
 * с начальным значением
 * 0x0000.
 */
ALWAYS_INLINE static uint16_t sdcard_crc16_ccitt(const void* data, size_t size)
{
    return crc16_ccitt_initial(data, size, 0x0000);
}


/*
 * Предопределённые команды SD-карты.
 */
static const sdcard_cmd_t sdcard_cmd0 = SDCARD_CMD_MAKE(SDCARD_CMD_GO_IDLE_STATE, 0, 0x4a);
static const sdcard_cmd_t sdcard_cmd1 = SDCARD_CMD_MAKE(SDCARD_CMD_SEND_OP_COND, 0, 0x7c);
static const sdcard_cmd_t sdcard_cmd8 = SDCARD_CMD_MAKE(SDCARD_CMD_SEND_IF_COND, SDCARD_SEND_IF_COND_CHECK, 0x43);
static const sdcard_cmd_t sdcard_cmd9 = SDCARD_CMD_MAKE(SDCARD_CMD_SEND_CSD, 0, 0x57);
static const sdcard_cmd_t sdcard_cmd10 = SDCARD_CMD_MAKE(SDCARD_CMD_SEND_CID, 0, 0xd);
static const sdcard_cmd_t sdcard_cmd12 = SDCARD_CMD_MAKE(SDCARD_CMD_STOP_TRANSMISSION, 0, 0x30);
static const sdcard_cmd_t sdcard_cmd13 = SDCARD_CMD_MAKE(SDCARD_CMD_SEND_STATUS, 0, 0x6);
static const sdcard_cmd_t sdcard_cmd38 = SDCARD_CMD_MAKE(SDCARD_CMD_ERASE, 0, 0x52);
static const sdcard_cmd_t sdcard_cmd55 = SDCARD_CMD_MAKE(SDCARD_CMD_APP_CMD, 0, 0x32);
static const sdcard_cmd_t sdcard_cmd58 = SDCARD_CMD_MAKE(SDCARD_CMD_READ_OCR, 0, 0x7e);
static const sdcard_cmd_t sdcard_acmd22 = SDCARD_CMD_MAKE(SDCARD_ACMD_SEND_NUM_WR_BLOCKS, 0, 0x21);
static const sdcard_cmd_t sdcard_acmd41_hc = SDCARD_CMD_MAKE(SDCARD_ACMD_SD_SEND_OP_COND, SDCARD_ACMD_SD_SEND_OP_COND_HCS, 0x3b);
static const sdcard_cmd_t sdcard_acmd41_sc = SDCARD_CMD_MAKE(SDCARD_ACMD_SD_SEND_OP_COND, 0, 0x72);



/*
 * SD card.
 */

/**
 * Устанавливает скорость шины SPI.
 * @param sdcard SD-карта.
 * @param speed Скорость SPI.
 * @return Флаг установки скорости (возможности продолжить работу).
 */
ALWAYS_INLINE static bool sdcard_set_speed(sdcard_t* sdcard, sdcard_spi_speed_t speed)
{
    if(sdcard->set_spi_speed) return sdcard->set_spi_speed(speed);
    return true;
}

ALWAYS_INLINE static void sdcard_timeout_begin(sdcard_t* sdcard, sdcard_timeout_t timeout)
{
    sdcard->timeout = false;
    if(sdcard->timeout_begin) sdcard->timeout_begin(timeout);
}

ALWAYS_INLINE static void sdcard_timeout_end(sdcard_t* sdcard)
{
    if(sdcard->timeout_end) sdcard->timeout_end();
    sdcard->timeout = false;
}

/**
 * Ждёт завершения текущей операции.
 * @param sdcard SD-карта.
 * @return true, если шина spi занята нами и мы дождались, иначе false.
 */
static bool sdcard_wait_current_op(sdcard_t* sdcard)
{
    // Если шина занята, да ещё и не нами - возврат ошибки занятости.
    if(spi_bus_busy(sdcard->spi) && spi_bus_transfer_id(sdcard->spi) != sdcard->transfer_id) return false;

    future_wait(&sdcard->future);
    spi_bus_wait(sdcard->spi);

    return true;
}
/**
 * Начинает обмен данными с SD-картой.
 * @param sdcard SD-карта.
 */
static void sdcard_begin(sdcard_t* sdcard)
{
    future_set_result(&sdcard->future, int_to_pvoid(E_NO_ERROR));
    future_start(&sdcard->future);
}

/**
 * Завершает обмен данными с SD-картой.
 * @param sdcard SD-карта.
 * @param err Код ошибки.
 */
static void sdcard_end(sdcard_t* sdcard, err_t err)
{
    future_finish(&sdcard->future, int_to_pvoid(err));
}

/**
 * Передаёт сообщения SPI.
 * @param sdcard SD-карта.
 * @param messages_count Число сообщений.
 * @return Код ошибки.
 */
static err_t sdcard_transfer(sdcard_t* sdcard, size_t messages_count)
{
    if(!spi_bus_set_transfer_id(sdcard->spi, sdcard->transfer_id)) return E_BUSY;

    spi_bus_set_tx_default_value(sdcard->spi, SDCARD_DEFAULT_DATA16);

    sdcard_begin(sdcard);

    err_t err = spi_bus_transfer(sdcard->spi, sdcard->messages, messages_count);

    if(err != E_NO_ERROR){
        sdcard_end(sdcard, err);
        return err;
    }

    return err;
}

/**
 * Ожидает завершения текущей операции.
 * @param sdcard SD-карта.
 * @return Код ошибки.
 */
static err_t sdcard_wait(sdcard_t* sdcard)
{
    future_wait(&sdcard->future);
    return pvoid_to_int(err_t, future_result(&sdcard->future));
}

err_t sdcard_init(sdcard_t* sdcard, sdcard_init_t* init)
{
    if(init->spi == NULL) return E_NULL_POINTER;
    if(init->gpio_cs == NULL) return E_NULL_POINTER;
    if(init->pin_cs == 0) return E_INVALID_VALUE;

    memset(sdcard, 0x0, sizeof(sdcard_t));

    sdcard->spi = init->spi;
    sdcard->transfer_id = init->transfer_id;
    sdcard->gpio_cs = init->gpio_cs;
    sdcard->pin_cs = init->pin_cs;
    sdcard->set_spi_speed = init->set_spi_speed;
    sdcard->timeout_begin = init->timeout_begin;
    sdcard->timeout_end = init->timeout_end;

    future_init(&sdcard->future);

    gpio_set(sdcard->gpio_cs, sdcard->pin_cs);

    return E_NO_ERROR;
}

bool sdcard_spi_callback(sdcard_t* sdcard)
{
    if(spi_bus_transfer_id(sdcard->spi) != sdcard->transfer_id) return false;
    if(!future_running(&sdcard->future)) return false;

    err_t err = E_NO_ERROR;

    spi_errors_t spi_errs = spi_bus_errors(sdcard->spi);

    if(spi_errs != SPI_NO_ERROR){
        if(spi_errs & SPI_ERROR_CRC){
            err = E_CRC;
        }else{
            err = E_IO_ERROR;
        }
    }

    sdcard_end(sdcard, err);

    return true;
}

void sdcard_timeout(sdcard_t* sdcard)
{
    sdcard->timeout = true;
}

bool sdcard_identified(sdcard_t* sdcard)
{
    return sdcard->card_type != SDCARD_TYPE_UNKNOWN;
}

bool sdcard_initialized(sdcard_t* sdcard)
{
    return sdcard->initialized;
}

sdcard_type_t sdcard_card_type(sdcard_t* sdcard)
{
    return sdcard->card_type;
}

uint64_t sdcard_capacity(sdcard_t* sdcard)
{
    uint64_t cap = 0;

    switch(sdcard->card_type){
    case SDCARD_TYPE_UNKNOWN:
        break;
    case SDCARD_TYPE_MMC:
        cap = sdcard->csd.mmc_csd.c_size + 1;
        cap <<= (sdcard->csd.mmc_csd.c_size_mult + 2 + sdcard->csd.mmc_csd.read_bl_len);
        break;
    case SDCARD_TYPE_SDSCv1:
    case SDCARD_TYPE_SDSCv2:
        cap = sdcard->csd.sd_csd1.c_size + 1;
        cap <<= (sdcard->csd.sd_csd1.c_size_mult + 2 + sdcard->csd.sd_csd1.read_bl_len);
        break;
    case SDCARD_TYPE_SDHC_SDXC:
        cap = sdcard->csd.sd_csd2.c_size + 1;
        cap <<= 19;
        break;
    }

    return cap;
}

uint16_t sdcard_sector_size(sdcard_t* sdcard)
{
    uint16_t size = 0;

    switch(sdcard->card_type){
    case SDCARD_TYPE_UNKNOWN:
        break;
    case SDCARD_TYPE_MMC:
    case SDCARD_TYPE_SDSCv1:
    case SDCARD_TYPE_SDSCv2:
    case SDCARD_TYPE_SDHC_SDXC:
        size = SDCARD_BLOCK_SIZE;
        break;
    }

    return size;
}

uint32_t sdcard_sectors_count(sdcard_t* sdcard)
{
    uint32_t count = 0;

    switch(sdcard->card_type){
    case SDCARD_TYPE_UNKNOWN:
        break;
    case SDCARD_TYPE_MMC:
        count = sdcard->csd.mmc_csd.c_size + 1;
        count <<= (sdcard->csd.mmc_csd.c_size_mult + 2 + sdcard->csd.mmc_csd.read_bl_len - 9);
        break;
    case SDCARD_TYPE_SDSCv1:
    case SDCARD_TYPE_SDSCv2:
        count = sdcard->csd.sd_csd1.c_size + 1;
        count <<= (sdcard->csd.sd_csd1.c_size_mult + 2 + sdcard->csd.sd_csd1.read_bl_len - 9);
        break;
    case SDCARD_TYPE_SDHC_SDXC:
        count = sdcard->csd.sd_csd2.c_size + 1;
        count <<= (19 - 9);
        break;
    }

    return count;
}

uint16_t sdcard_block_len(sdcard_t* sdcard)
{
    return sdcard->rw_block_size;
}

uint16_t sdcard_erase_block_len(sdcard_t* sdcard)
{
    return sdcard->er_block_size;
}

bool sdcard_partial_block_read(sdcard_t* sdcard)
{
    switch(sdcard->card_type){
    case SDCARD_TYPE_UNKNOWN:
        break;
    case SDCARD_TYPE_MMC:
        return sdcard->csd.mmc_csd.read_bl_partial;
    case SDCARD_TYPE_SDSCv1:
    case SDCARD_TYPE_SDSCv2:
        return sdcard->csd.sd_csd1.read_bl_partial;
    case SDCARD_TYPE_SDHC_SDXC:
        return sdcard->csd.sd_csd2.read_bl_partial;
    }

    return false;
}

bool sdcard_partial_block_write(sdcard_t* sdcard)
{
    switch(sdcard->card_type){
    case SDCARD_TYPE_UNKNOWN:
        break;
    case SDCARD_TYPE_MMC:
        return sdcard->csd.mmc_csd.write_bl_partial;
    case SDCARD_TYPE_SDSCv1:
    case SDCARD_TYPE_SDSCv2:
        return sdcard->csd.sd_csd1.write_bl_partial;
    case SDCARD_TYPE_SDHC_SDXC:
        return sdcard->csd.sd_csd2.write_bl_partial;
    }

    return false;
}

bool sdcard_misalign_block_read(sdcard_t* sdcard)
{
    switch(sdcard->card_type){
    case SDCARD_TYPE_UNKNOWN:
        break;
    case SDCARD_TYPE_MMC:
        return sdcard->csd.mmc_csd.read_blk_misalign;
    case SDCARD_TYPE_SDSCv1:
    case SDCARD_TYPE_SDSCv2:
        return sdcard->csd.sd_csd1.read_blk_misalign;
    case SDCARD_TYPE_SDHC_SDXC:
        return sdcard->csd.sd_csd2.read_blk_misalign;
    }

    return false;
}

bool sdcard_misalign_block_write(sdcard_t* sdcard)
{
    switch(sdcard->card_type){
    case SDCARD_TYPE_UNKNOWN:
        break;
    case SDCARD_TYPE_MMC:
        return sdcard->csd.mmc_csd.write_blk_misalign;
    case SDCARD_TYPE_SDSCv1:
    case SDCARD_TYPE_SDSCv2:
        return sdcard->csd.sd_csd1.write_blk_misalign;
    case SDCARD_TYPE_SDHC_SDXC:
        return sdcard->csd.sd_csd2.write_blk_misalign;
    }

    return false;
}

void sdcard_cmd_setup(sdcard_cmd_t* cmd, uint8_t index, uint32_t argument)
{
    cmd->index_sb_tb = (0x40 | (index & 0x3f));
    cmd->argument = __REV(argument);

    uint8_t crc = crc7_sdcard(cmd, SDCARD_CMD_SIZE_CRC);

    cmd->crc_eb = (crc << 1) | 0x1;
}

/**
 * Тактирует шину SPI одним байтом
 * данных по-умолчанию.
 * @param sdcard SD-карта.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t sdcard_nop(sdcard_t* sdcard)
{
    return spi_bus_transmit(sdcard->spi, SDCARD_DEFAULT_DATA8, NULL);
}

/**
 * Получает от SD-карты данные.
 * @param sdcard SD-карта.
 * @param data Данные.
 * @return Код ошибки.
 */
static err_t sdcard_recv_data(sdcard_t* sdcard, uint8_t* data)
{
    if(data == NULL) return E_NULL_POINTER;

    err_t err = E_NO_ERROR;
    uint16_t rx_data = SDCARD_DEFAULT_DATA8;

    err = spi_bus_transmit(sdcard->spi, SDCARD_DEFAULT_DATA8, &rx_data);
    if(err != E_NO_ERROR) return err;

    *data = rx_data;

    return err;
}

/**
 * Получает от SD-карты валидные (не 0xff) данные.
 * Для таймаута используется Ncr число тактов
 * (SDCARD_WAIT_REPLY_BYTES).
 * @param sdcard SD-карта.
 * @param data Данные.
 * @return Код ошибки.
 */
static err_t sdcard_recv_valid_data(sdcard_t* sdcard, uint8_t* data)
{
    if(data == NULL) return E_NULL_POINTER;

    err_t err = E_NO_ERROR;
    uint16_t rx_data = SDCARD_NOREPLY_DATA;

    size_t wait_bytes = 0;

    for(;;) {
        err = spi_bus_transmit(sdcard->spi, SDCARD_DEFAULT_DATA8, &rx_data);
        if(err != E_NO_ERROR) return err;

        if(rx_data != SDCARD_NOREPLY_DATA) break;

        if(++ wait_bytes > SDCARD_WAIT_REPLY_BYTES) return E_TIME_OUT;
    }

    *data = rx_data;

    return E_NO_ERROR;
}

/**
 * Ожидает окончания чтения данных SD-карты
 * и принимает маркер данных.
 * @param sdcard SD-карта.
 * @param token Маркер данных.
 * @return Код ошибки.
 */
static err_t sdcard_recv_data_token(sdcard_t* sdcard, sdcard_control_token_t* token)
{
    err_t err = E_NO_ERROR;
    uint16_t rx_data = SDCARD_NOREPLY_DATA;

    sdcard_timeout_begin(sdcard, SDCARD_TIMEOUT_READ);
    for(;;) {
        err = spi_bus_transmit(sdcard->spi, SDCARD_DEFAULT_DATA8, &rx_data);
        if(err != E_NO_ERROR){
            sdcard_timeout_end(sdcard);
            return err;
        }

        if(rx_data != SDCARD_NOREPLY_DATA) break;

        if(sdcard->timeout){
            //sdcard_timeout_end(sdcard);
            return E_TIME_OUT;
        }
    }
    sdcard_timeout_end(sdcard);

    *token = rx_data;

    return E_NO_ERROR;
}

/**
 * Ожидает окончания занятости SD-карты.
 * @param sdcard SD-карта.
 * @return Код ошибки.
 */
/*static*/ err_t sdcard_wait_busy(sdcard_t* sdcard)
{
    err_t err = E_NO_ERROR;
    uint16_t rx_data = SDCARD_DEFAULT_DATA8;

    sdcard_timeout_begin(sdcard, SDCARD_TIMEOUT_BUSY);
    for(;;) {
        err = spi_bus_transmit(sdcard->spi, SDCARD_DEFAULT_DATA8, &rx_data);
        if(err != E_NO_ERROR){
            sdcard_timeout_end(sdcard);
            return err;
        }

        if(rx_data != SDCARD_BUSY_DATA) break;

        if(sdcard->timeout){
            //sdcard_timeout_end(sdcard);
            return E_TIME_OUT;
        }
    }
    sdcard_timeout_end(sdcard);

    return E_NO_ERROR;
}

/**
 * Реверсирует порядок байт в блоке данных.
 * @param data Данные.
 * @param size Размер данных.
 */
static void sdcard_data_reverse(uint8_t* data, size_t size)
{
    if(size <= 1) return;
    // 16 бит - __REV16.
    if(size == 2){
        *((uint16_t*)data) = __REV16(*((uint16_t*)data));
    } // 32 бит - __REV.
    else if(size == 4){
        *((uint32_t*)data) = __REV(*((uint32_t*)data));
    }else{
        uint8_t tmp;
        size_t i, j;
        size_t half_size = size >> 1;
        for(i = 0, j = size - 1; i < half_size; i ++, j --){
            tmp = data[i];
            data[i] = data[j];
            data[j] = tmp;
        }
    }
}

/**
 * Получает ответ заданного типа от SD-карты.
 * @param sdcard SD-карта.
 * @param reply_type Тип ответа.
 * @param reply Ответ.
 * @return Код ошибки.
 */
static err_t sdcard_recv_reply(sdcard_t* sdcard, sdcard_reply_type_t reply_type, sdcard_reply_t* reply)
{
    if(reply == NULL) return E_NULL_POINTER;

    // Always receive R1 reply.
    err_t err = E_NO_ERROR;

    uint8_t r1_data = SDCARD_DEFAULT_DATA8;

    err = sdcard_recv_valid_data(sdcard, &r1_data);
    if(err != E_NO_ERROR) return err;

    reply->r1.data = r1_data;

    if(SDCARD_REPLY_ONLY_R1(r1_data)) return err;

    switch(reply_type){
    default:
    case SDCARD_REPLY_R1:
        break;

    case SDCARD_REPLY_R1b:
        err = sdcard_wait_busy(sdcard);
        break;

    case SDCARD_REPLY_R2:
        err = sdcard_recv_data(sdcard, &r1_data);
        if(err != E_NO_ERROR) break;

        reply->r2.data = r1_data;
        break;

    case SDCARD_REPLY_R3:
        spi_message_setup(&sdcard->messages[0], SPI_READ, NULL, &reply->r3.data, sizeof(reply->r3.data));

        err = sdcard_transfer(sdcard, 1);
        if(err != E_NO_ERROR) break;

        err = sdcard_wait(sdcard);
        if(err != E_NO_ERROR) break;

        reply->r3.data = __REV(reply->r3.data);

        break;

    case SDCARD_REPLY_R7:
        spi_message_setup(&sdcard->messages[0], SPI_READ, NULL, &reply->r7.data, sizeof(reply->r7.data));

        err = sdcard_transfer(sdcard, 1);
        if(err != E_NO_ERROR) break;

        err = sdcard_wait(sdcard);
        if(err != E_NO_ERROR) break;

        reply->r7.data = __REV(reply->r7.data);

        break;
    }

    return err;
}

/**
 * Выполняет базовую проверку ответа SD-карты.
 * @param reply Ответ.
 * @return Код ошибки.
 */
static err_t sdcard_reply_base_check(sdcard_reply_t* reply)
{
    // Ошибка CRC.
    if(reply->r1.data & SDCARD_REPLY_R1_COM_CRC_ERROR) return E_SDCARD_COM_CRC_ERROR;
    // Неподдерживаемая команда.
    if(reply->r1.data & SDCARD_REPLY_R1_ILLEGAL_COMMAND) return E_SDCARD_ILLEGAL_COMMAND;

    return E_NO_ERROR;
}

/**
 * Выполняет полную проверку ответа SD-карты.
 * @param reply Ответ.
 * @return Код ошибки.
 */
static err_t sdcard_reply_check(sdcard_reply_t* reply)
{
    // Ошибка параметра.
    if(reply->r1.data & SDCARD_REPLY_R1_PARAM_ERROR) return E_SDCARD_PARAM_ERROR;
    // Ошибка адреса.
    if(reply->r1.data & SDCARD_REPLY_R1_ADDR_ERROR) return E_SDCARD_ADDR_ERROR;
    // Ошибка последовательности стирания.
    if(reply->r1.data & SDCARD_REPLY_R1_ERASE_SEQ_ERROR) return E_SDCARD_ERASE_SEQ_ERROR;
    // Ошибка CRC.
    if(reply->r1.data & SDCARD_REPLY_R1_COM_CRC_ERROR) return E_SDCARD_COM_CRC_ERROR;
    // Неподдерживаемая команда.
    if(reply->r1.data & SDCARD_REPLY_R1_ILLEGAL_COMMAND) return E_SDCARD_ILLEGAL_COMMAND;
    // Сброс процесса стирания.
    if(reply->r1.data & SDCARD_REPLY_R1_ERASE_RESET) return E_SDCARD_ERASE_RESET;
    // Карта в состоянии IDLE.
    if(reply->r1.data & SDCARD_REPLY_R1_IN_IDLE_STATE) return E_SDCARD_IN_IDLE_STATE;

    return E_NO_ERROR;
}

/**
 * Выполняет проверку состояния SD-карты.
 * @param status Ответ.
 * @return Код ошибки.
 */
static err_t sdcard_status_check(sdcard_status_t* status)
{
    // Поля ответа R1.
    if(status->parameter_error) return E_SDCARD_PARAM_ERROR;
    if(status->address_error) return E_SDCARD_ADDR_ERROR;
    if(status->erase_seq_error) return E_SDCARD_ERASE_SEQ_ERROR;
    if(status->com_crc_error) return E_SDCARD_COM_CRC_ERROR;
    if(status->illegal_command) return E_SDCARD_ILLEGAL_COMMAND;
    if(status->erase_reset) return E_SDCARD_ERASE_RESET;
    if(status->in_idle_state) return E_SDCARD_IN_IDLE_STATE;
    // Поля ответа R2.
    if(status->out_of_rng_csd_ovrwr) return E_SDCARD_OUT_OF_RANGE;
    if(status->erase_param) return E_SDCARD_ERASE_PARAM;
    if(status->wp_violation) return E_SDCARD_WP_VIOLATION;
    if(status->card_ecc_failed) return E_SDCARD_CARD_ECC_FAILED;
    if(status->cc_error) return E_SDCARD_CC_ERROR;
    if(status->error) return E_SDCARD_ERROR;
    if(status->wp_erase_lock_fail) return E_SDCARD_WP_ERASE_SKIP;
    if(status->card_locked) return E_SDCARD_CARD_IS_LOCKED;

    return E_NO_ERROR;
}

/**
 * Выполняет проверку маркера чтения данных.
 * @param token Маркер
 * @return Код ошибки.
 */
static err_t sdcard_read_data_token_check(sdcard_control_token_t token)
{
    // Проверка на соответствие.
    if(token == SDCARD_START_BLOCK_TOKEN) return E_NO_ERROR;

    if((token & SDCARD_DATA_ERROR_TOKEN_ZERO_MASK) != 0) return E_INVALID_VALUE;

    if(token & SDCARD_DATA_ERROR_TOKEN_OUT_OF_RANGE) return E_SDCARD_OUT_OF_RANGE;
    if(token & SDCARD_DATA_ERROR_TOKEN_CARD_ECC_FAILED) return E_SDCARD_CARD_ECC_FAILED;
    if(token & SDCARD_DATA_ERROR_TOKEN_CC_ERROR) return E_SDCARD_CC_ERROR;
    if(token & SDCARD_DATA_ERROR_TOKEN_ERROR)return E_SDCARD_ERROR;

    return E_IO_ERROR;
}

/**
 * Выполняет проверку маркера записи данных.
 * @param token Маркер
 * @return Код ошибки.
 */
static err_t sdcard_write_data_token_check(sdcard_control_token_t token)
{
    // Проверка на соответствие.
    if((token & SDCARD_DATA_RESP_TOKEN_BIT_ZERO) != 0) return E_INVALID_VALUE;

    uint8_t status = (token & SDCARD_DATA_RESP_TOKEN_STATUS_MASK)
                            >> SDCARD_DATA_RESP_TOKEN_STATUS_OFFSET;

    switch(status){
    default:
        return E_INVALID_VALUE;
    case SDCARD_DATA_RESP_TOKEN_STATUS_CRC_ERROR:
        return E_CRC;
    case SDCARD_DATA_RESP_TOKEN_STATUS_WRITE_ERROR:
        return E_IO_ERROR;
    case SDCARD_DATA_RESP_TOKEN_STATUS_DATA_ACCEPTED:
        break;
    }

    return E_NO_ERROR;
}

/**
 * Читает и проверяет состояние записи данных.
 * @param sdcard SD-карта.
 * @return Код ошибки.
 */
static err_t sdcard_check_write_status(sdcard_t* sdcard)
{
    err_t err = E_NO_ERROR;

    sdcard_status_t status;

    // Проверка состояния.
    err = sdcard_read_status(sdcard, &status);
    if(err != E_NO_ERROR) return err;

    return sdcard_status_check(&status);
}

/**
 * Отправляет команду в SD-карту.
 * Ждёт готовности карты.
 * @param sdcard SD-карта.
 * @param cmd Команда.
 * @param reply_type Тип ответа.
 * @param reply Ответ.
 * @return Код ошибки.
 */
static err_t sdcard_cmd(sdcard_t* sdcard, const sdcard_cmd_t* cmd,
                 sdcard_reply_type_t reply_type, sdcard_reply_t* reply)
{
    err_t err = E_NO_ERROR;

    err = sdcard_wait_busy(sdcard);
    if(err != E_NO_ERROR) return err;

    spi_message_setup(&sdcard->messages[0], SPI_WRITE, cmd, NULL, SDCARD_CMD_SIZE);

    err = sdcard_transfer(sdcard, 1);
    if(err != E_NO_ERROR) return err;

    err = sdcard_wait(sdcard);
    if(err != E_NO_ERROR) return err;

    err = sdcard_recv_reply(sdcard, reply_type, reply);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

//    /**
//     * Отправляет команду в SD-карту.
//     * Не ждёт готовности карты.
//     * @param sdcard SD-карта.
//     * @param cmd Команда.
//     * @param reply_type Тип ответа.
//     * @param reply Ответ.
//     * @return Код ошибки.
//     */
//    static err_t sdcard_cmd_inst(sdcard_t* sdcard, const sdcard_cmd_t* cmd,
//                          sdcard_reply_type_t reply_type, sdcard_reply_t* reply)
//    {
//        err_t err = E_NO_ERROR;
//
//        spi_message_setup(&sdcard->messages[0], SPI_WRITE, cmd, NULL, SDCARD_CMD_SIZE);
//
//        err = sdcard_transfer(sdcard, 1);
//        if(err != E_NO_ERROR) return err;
//
//        err = sdcard_wait(sdcard);
//        if(err != E_NO_ERROR) return err;
//
//        err = sdcard_recv_reply(sdcard, reply_type, reply);
//        if(err != E_NO_ERROR) return err;
//
//        return E_NO_ERROR;
//    }

/**
 * Отправляет команду приложения в SD-карту.
 * @param sdcard SD-карта.
 * @param cmd Команда.
 * @param reply_type Тип ответа.
 * @param reply Ответ.
 * @return Код ошибки.
 */
static err_t sdcard_acmd(sdcard_t* sdcard, const sdcard_cmd_t* cmd,
                  sdcard_reply_type_t reply_type, sdcard_reply_t* reply)
{
    err_t err = E_NO_ERROR;

    err = sdcard_cmd(sdcard, &sdcard_cmd55, SDCARD_REPLY_R1, reply);
    if(err != E_NO_ERROR) return err;

    err = sdcard_reply_base_check(reply);
    if(err != E_NO_ERROR) return err;

    err = sdcard_cmd(sdcard, cmd, reply_type, reply);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

/**
 * Отправляет команду окончания
 * передачи в SD-карту.
 * Не ждёт готовности карты.
 * Пропускает 1 байт (мусорный)
 * после отправки команды.
 * @param sdcard SD-карта.
 * @param reply Ответ.
 * @return Код ошибки.
 */
err_t sdcard_cmd_stop_transmission(sdcard_t* sdcard, sdcard_reply_t* reply)
{
    err_t err = E_NO_ERROR;

    spi_message_setup(&sdcard->messages[0], SPI_WRITE, &sdcard_cmd12, NULL, SDCARD_CMD_SIZE);

    err = sdcard_transfer(sdcard, 1);
    if(err != E_NO_ERROR) return err;

    err = sdcard_wait(sdcard);
    if(err != E_NO_ERROR) return err;

    // Отбросим мусорный байт.
    err = sdcard_nop(sdcard);
    if(err != E_NO_ERROR) return err;

    err = sdcard_recv_reply(sdcard, SDCARD_REPLY_R1b, reply);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

/**
 * Принимает блок данных от SD-карты.
 * @param sdcard SD-карта.
 * @param data Данные.
 * @param data_size Размер данных.
 * @return Код ошибки.
 */
static err_t sdcard_recv_data_block(sdcard_t* sdcard, void* data, size_t data_size)
{
    err_t err = E_NO_ERROR;

    // Принимаем заданное число байт данных
    // и два байта контрольной суммы.

    spi_message_setup(&sdcard->messages[0], SPI_READ, NULL, data, data_size);

    err = sdcard_transfer(sdcard, 1);
    if(err != E_NO_ERROR) return err;

    err = sdcard_wait(sdcard);
    if(err != E_NO_ERROR) return err;

    uint16_t crc_hi, crc_lo;

    err = spi_bus_transmit(sdcard->spi, SDCARD_DEFAULT_DATA8, &crc_hi);
    if(err != E_NO_ERROR) return err;
    err = spi_bus_transmit(sdcard->spi, SDCARD_DEFAULT_DATA8, &crc_lo);
    if(err != E_NO_ERROR) return err;

    // Если разрешён crc - вычислим контрольную сумму.
    if(sdcard->crc_enabled){
        uint16_t crc = sdcard_crc16_ccitt(data, data_size);
        uint16_t crc_val = (crc_hi << 8) | crc_lo;

        if(crc != crc_val) return E_CRC;
    }

    return E_NO_ERROR;
}

/**
 * Отправляет блок данных в SD-карту.
 * @param sdcard SD-карта.
 * @param data Данные.
 * @param data_size Размер данных.
 * @return Код ошибки.
 */
static err_t sdcard_send_data_block(sdcard_t* sdcard, const void* data, size_t data_size)
{
    err_t err = E_NO_ERROR;

    // Отправляем заданное число байт данных
    // и два байта контрольной суммы.

    spi_message_setup(&sdcard->messages[0], SPI_WRITE, data, NULL, data_size);

    err = sdcard_transfer(sdcard, 1);
    if(err != E_NO_ERROR) return err;

    // В процессе передачи подсчитаем
    // контрольную сумму, если она разрешена.
    uint16_t crc_hi = SDCARD_DEFAULT_DATA8;
    uint16_t crc_lo = SDCARD_DEFAULT_DATA8;

    // Если разрешён crc - вычислим контрольную сумму.
    if(sdcard->crc_enabled){
        uint16_t crc = sdcard_crc16_ccitt(data, data_size);
        crc_hi = crc >> 8; crc_lo = crc & 0xff;
    }

    err = sdcard_wait(sdcard);
    if(err != E_NO_ERROR) return err;

    err = spi_bus_transmit(sdcard->spi, crc_hi, NULL);
    if(err != E_NO_ERROR) return err;
    err = spi_bus_transmit(sdcard->spi, crc_lo, NULL);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

/*
 * Чтение и запись данных.
 */

/**
 * Читает данные команды из SD-карты.
 * @param sdcard SD-карта.
 * @param data Данные.
 * @param data_size Размер данных.
 * @return Код ошибки.
 */
static err_t sdcard_read_cmd_data(sdcard_t* sdcard, void* data, size_t data_size)
{
    err_t err = E_NO_ERROR;
    sdcard_control_token_t token;

    // Получим маркер начала блока данных.
    err = sdcard_recv_valid_data(sdcard, &token);
    if(err != E_NO_ERROR) return err;

    // Проверим маркер данных.
    err = sdcard_read_data_token_check(token);
    if(err != E_NO_ERROR) return err;

    // Прочитаем данные.
    err = sdcard_recv_data_block(sdcard, data, data_size);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

/**
 * Читает данные из SD-карты.
 * @param sdcard SD-карта.
 * @param data Данные.
 * @param data_size Размер данных.
 * @return Код ошибки.
 */
static err_t sdcard_read_data(sdcard_t* sdcard, void* data, size_t data_size)
{
    err_t err = E_NO_ERROR;

    sdcard_control_token_t token;

    // Получим маркер начала блока данных.
    err = sdcard_recv_data_token(sdcard, &token);
    if(err != E_NO_ERROR) return err;

    // Проверим маркер данных.
    err = sdcard_read_data_token_check(token);
    if(err != E_NO_ERROR) return err;

    // Прочитаем данные.
    err = sdcard_recv_data_block(sdcard, data, data_size);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

/**
 * Читает одиночный блок данных из SD-карты.
 * @param sdcard SD-карта.
 * @param address Адрес.
 * @param data Данные.
 * @param data_size Размер данных.
 * @param readed Число прочитанных байт, может быть NULL.
 * @return Код ошибки.
 */
static err_t sdcard_read_single_data_block(sdcard_t* sdcard, uint32_t address,
                                           void* data, size_t data_size, size_t* readed)
{
    err_t err = E_NO_ERROR;

    sdcard_cmd_t cmd;
    sdcard_reply_t reply;

    if(readed) *readed = 0;

    sdcard_cmd_setup(&cmd, SDCARD_CMD_READ_SINGLE_BLOCK, address);

    // Отправим команду.
    err = sdcard_cmd(sdcard, &cmd, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    // Получим данные.
    err = sdcard_read_data(sdcard, data, data_size);
    if(err != E_NO_ERROR) return err;

    if(readed) *readed = data_size;

    return E_NO_ERROR;
}

/**
 * Читает несколько блоков данных из SD-карты.
 * @param sdcard SD-карта.
 * @param address Адрес.
 * @param data Данные.
 * @param data_size Размер данных.
 * @param readed Число прочитанных байт, может быть NULL.
 * @return Код ошибки.
 */
static err_t sdcard_read_multiple_data_block(sdcard_t* sdcard, uint32_t address,
                                             void* data, size_t data_size, size_t* readed)
{
    err_t err = E_NO_ERROR;
    err_t read_err = E_NO_ERROR;

    sdcard_cmd_t cmd;
    sdcard_reply_t reply;

    size_t rx_count = 0;

    if(readed) *readed = 0;

    sdcard_cmd_setup(&cmd, SDCARD_CMD_READ_MULTIPLE_BLOCK, address);

    // Отправим команду.
    err = sdcard_cmd(sdcard, &cmd, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    while(data_size >= sdcard->rw_block_size){
        // Получим данные.
        read_err = sdcard_read_data(sdcard, data, sdcard->rw_block_size);
        // При ошибке.
        if(read_err != E_NO_ERROR) break;

        data += sdcard->rw_block_size;
        data_size -= sdcard->rw_block_size;
        rx_count += sdcard->rw_block_size;
    }

    if(readed) *readed = rx_count;

    // Отправим команду завершения передачи.
    err = sdcard_cmd_stop_transmission(sdcard, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR){
        // SD-карта может отправить ошибку адреса,
        // если чтение производилось на границе,
        // поэтому игнорируем её.
        if(err != E_OUT_OF_RANGE || (uint32_t)data != sdcard_sectors_count(sdcard)){
            return err;
        }
    }

    return read_err;
}

/**
 * Записывает данные в SD-карту.
 * @param sdcard SD-карта.
 * @param token Маркер начала блока.
 * @param data Данные.
 * @param data_size Размер данных.
 * @return Код ошибки.
 */
static err_t sdcard_write_data(sdcard_t* sdcard, sdcard_control_token_t token,
                               const void* data, size_t data_size)
{
    err_t err = E_NO_ERROR;

    sdcard_control_token_t resp_token;

    // Один пустой цикл.
    err = sdcard_nop(sdcard);
    if(err != E_NO_ERROR) return err;

    // Отправим маркер начала данных.
    err = spi_bus_transmit(sdcard->spi, token, NULL);
    if(err != E_NO_ERROR) return err;

    // Отправим данные.
    err = sdcard_send_data_block(sdcard, data, data_size);
    if(err != E_NO_ERROR) return err;

    // Получим маркер принятия данных SD-картой.
    err = sdcard_recv_valid_data(sdcard, &resp_token);
    if(err != E_NO_ERROR) return err;

    // Проверим маркер данных.
    err = sdcard_write_data_token_check(resp_token);
    if(err != E_NO_ERROR) return err;

    // Ожидание окончания записи.
    err = sdcard_wait_busy(sdcard);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

/**
 * Записывает одиночиный блок данных в SD-карту.
 * @param sdcard SD-карта.
 * @param address Адрес.
 * @param data Данные.
 * @param data_size Размер данных.
 * @param written Число записаных байт, может быть NULL.
 * @return Код ошибки.
 */
static err_t sdcard_write_single_data_block(sdcard_t* sdcard, uint32_t address,
                                            const void* data, size_t data_size, size_t* written)
{
    err_t err = E_NO_ERROR;

    sdcard_cmd_t cmd;
    sdcard_reply_t reply;

    if(written) *written = 0;

    sdcard_cmd_setup(&cmd, SDCARD_CMD_WRITE_SINGLE_BLOCK, address);

    // Отправим команду.
    err = sdcard_cmd(sdcard, &cmd, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    // Отправим данные.
    err = sdcard_write_data(sdcard, SDCARD_START_BLOCK_TOKEN, data, data_size);
    if(err != E_NO_ERROR) return err;

    // Проверим состояние записи.
    err = sdcard_check_write_status(sdcard);
    if(err != E_NO_ERROR) return err;

    if(written) *written = data_size;

    return E_NO_ERROR;
}

/**
 * Записывает несколько блоков данных в SD-карту.
 * @param sdcard SD-карта.
 * @param address Адрес.
 * @param data Данные.
 * @param data_size Размер данных.
 * @param written Число записаных байт, может быть NULL.
 * @return Код ошибки.
 */
static err_t sdcard_write_multiple_data_block(sdcard_t* sdcard, uint32_t address,
                                              const void* data, size_t data_size, size_t* written)
{
    err_t err = E_NO_ERROR;
    err_t write_err = E_NO_ERROR;

    sdcard_cmd_t cmd;
    sdcard_reply_t reply;

    size_t tx_count = 0;

    if(written) *written = 0;

    sdcard_cmd_setup(&cmd, SDCARD_CMD_WRITE_MULTIPLE_BLOCK, address);

    // Отправим число записываемых блоков.
    // Карты MMC (по спецификации Samsung) не поддерживают
    // CMD55 (APP_CMD).
    if(sdcard->card_type != SDCARD_TYPE_MMC){

        sdcard_cmd_t cmd_count;
        size_t blocks_count;

        if(sdcard->rw_block_size == SDCARD_BLOCK_SIZE){
            blocks_count = data_size >> 9;
        }else{
            blocks_count = data_size / sdcard->rw_block_size;
        }

        sdcard_cmd_setup(&cmd_count, SDCARD_ACMD_SET_WR_BLK_ERASE_COUNT,
                         blocks_count & SDCARD_ACMD_SET_WR_BLK_ERASE_COUNT_NUMBER);

        // Отправим команду количества блоков.
        err = sdcard_acmd(sdcard, &cmd_count, SDCARD_REPLY_R1, &reply);
        if(err != E_NO_ERROR) return err;

        // Проверим ответ.
        err = sdcard_reply_check(&reply);
        if(err != E_NO_ERROR) return err;
    }

    // Отправим команду записи.
    err = sdcard_cmd(sdcard, &cmd, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    while(data_size >= sdcard->rw_block_size){
        // Отправим данные.
        write_err = sdcard_write_data(sdcard, SDCARD_MBW_START_BLOCK_TOKEN, data, sdcard->rw_block_size);
        // При ошибке.
        if(write_err != E_NO_ERROR) break;

        data += sdcard->rw_block_size;
        data_size -= sdcard->rw_block_size;
        tx_count += sdcard->rw_block_size;
    }

    if(written) *written = tx_count;

    // Один пустой цикл.
    err = sdcard_nop(sdcard);
    if(err != E_NO_ERROR) return err;

    // Отправим маркер начала данных.
    err = spi_bus_transmit(sdcard->spi, SDCARD_MBW_STOP_TRAN_TOKEN, NULL);
    if(err != E_NO_ERROR) return err;

    // Один пустой цикл.
    err = sdcard_nop(sdcard);
    if(err != E_NO_ERROR) return err;

    // Ожидание окончания записи.
    err = sdcard_wait_busy(sdcard);
    if(err != E_NO_ERROR) return err;

    // Проверим состояние записи.
    err = sdcard_check_write_status(sdcard);
    if(err != E_NO_ERROR) return err;

    return write_err;
}

err_t sdcard_select(sdcard_t* sdcard)
{
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

#if (SDCARD_SELECT_BEFORE_BYTES != 0) || (SDCARD_SELECT_AFTER_BYTES != 0)
    err_t err = E_NO_ERROR;
    size_t bytes_count;
#endif

#if SDCARD_SELECT_BEFORE_BYTES != 0
    for(bytes_count = 0; bytes_count < SDCARD_SELECT_BEFORE_BYTES; bytes_count ++){
        err = sdcard_nop(sdcard);
        if(err != E_NO_ERROR) break;
    }
#endif

    gpio_reset(sdcard->gpio_cs, sdcard->pin_cs);

#if SDCARD_SELECT_AFTER_BYTES != 0
    for(bytes_count = 0; bytes_count < SDCARD_SELECT_AFTER_BYTES; bytes_count ++){
        err = sdcard_nop(sdcard);
        if(err != E_NO_ERROR) break;
    }
#endif

    return E_NO_ERROR;
}

void sdcard_deselect(sdcard_t* sdcard)
{
#if (SDCARD_DESELECT_BEFORE_BYTES != 0) || (SDCARD_DESELECT_AFTER_BYTES != 0)
    err_t err = E_NO_ERROR;
    size_t bytes_count;
#endif

#if SDCARD_DESELECT_BEFORE_BYTES != 0
    for(bytes_count = 0; bytes_count < SDCARD_DESELECT_BEFORE_BYTES; bytes_count ++){
        err = sdcard_nop(sdcard);
        if(err != E_NO_ERROR) break;
    }
#endif

    gpio_set(sdcard->gpio_cs, sdcard->pin_cs);

#if SDCARD_DESELECT_AFTER_BYTES != 0
    for(bytes_count = 0; bytes_count < SDCARD_DESELECT_AFTER_BYTES; bytes_count ++){
        err = sdcard_nop(sdcard);
        if(err != E_NO_ERROR) break;
    }
#endif
}

/**
 * Выполняет конфигурацию шины SPI для инициализации.
 * @param sdcard SD-карта.
 * @return Флаг конфигурации шины SPI.
 */
static bool sdcard_init_begin_setup_spi(sdcard_t* sdcard)
{
    if(!spi_bus_set_enabled(sdcard->spi, false)) return false;
    if(!sdcard_set_speed(sdcard, SDCARD_SPI_SPEED_LOW)) return false;
    if(!spi_bus_set_enabled(sdcard->spi, true)) return false;

    return true;
}

/**
 * Выполняет конфигурацию шины SPI по завершению инициализации.
 * @param sdcard SD-карта.
 * @return Флаг конфигурации шины SPI.
 */
static bool sdcard_init_end_setup_spi(sdcard_t* sdcard)
{
    if(!spi_bus_set_enabled(sdcard->spi, false)) return false;
    if(!sdcard_set_speed(sdcard, SDCARD_SPI_SPEED_HIGH)) return false;
    if(!spi_bus_set_enabled(sdcard->spi, true)) return false;

    return true;
}

/**
 * Обновляет конфигурацию карты.
 * Тип SD-карты должен быть определён.
 * @param sdcard SD-карта.
 * @return Код ошибки.
 */
static err_t sdcard_init_update_conf(sdcard_t* sdcard)
{
    if(sdcard->card_type == SDCARD_TYPE_UNKNOWN) return E_STATE;

    err_t err = E_NO_ERROR;

    // Прочитаем CID - правда непонятно на кой,
    // но это было в каком-то даташите.
    err = sdcard_read_cid(sdcard, &sdcard->cid);
    if(err != E_NO_ERROR) return err;

    // Прочитаем CSD.
    err = sdcard_read_csd(sdcard, &sdcard->csd);
    if(err != E_NO_ERROR) return err;

    // Если тип карты не SDHC/SDXC.
    if(sdcard->card_type != SDCARD_TYPE_SDHC_SDXC){
        // Установим размер блока в 512 байт.
        err = sdcard_set_block_len(sdcard, SDCARD_BLOCK_SIZE);
        if(err != E_NO_ERROR) return err;
        // Новый размер блока.
        sdcard->rw_block_size = SDCARD_BLOCK_SIZE;

        // Вычислим размер стираемого блока.
        // SDSC.
        if(sdcard->card_type == SDCARD_TYPE_SDSCv1 || sdcard->card_type == SDCARD_TYPE_SDSCv2){
            // Если стирание производится блоками.
            if(sdcard->csd.sd_csd1.erase_blk_en){
                sdcard->er_block_size = SDCARD_BLOCK_SIZE;
            }else{
                sdcard->er_block_size = SDCARD_BLOCK_SIZE *
                                        (sdcard->csd.sd_csd1.sector_size + 1);
            }
        }else{ //SDCARD_TYPE_MMC
            sdcard->er_block_size = SDCARD_BLOCK_SIZE *
                                    (sdcard->csd.mmc_csd.erase_grp_mult + 1) *
                                    (sdcard->csd.mmc_csd.erase_grp_size + 1);
        }
    }else{
        sdcard->rw_block_size = SDCARD_BLOCK_SIZE;
        sdcard->er_block_size = SDCARD_BLOCK_SIZE;
    }

    if(sdcard->crc_enabled){
        err = sdcard_set_crc_enabled(sdcard, true);
        if(err != E_NO_ERROR) return err;
    }

    return E_NO_ERROR;
}

/**
 * Инициализирует карту MMC.
 * @param sdcard SD-карта.
 * @param reply Ответ SD-карты.
 * @return Код ошибки.
 */
static err_t sdcard_init_card_mmc(sdcard_t* sdcard, sdcard_reply_t* reply)
{
    err_t err = E_NO_ERROR;

    sdcard_timeout_begin(sdcard, SDCARD_TIMEOUT_INIT);
    for(;;){
        err = sdcard_cmd(sdcard, &sdcard_cmd1, SDCARD_REPLY_R1, reply);
        if(err != E_NO_ERROR){
            sdcard_timeout_end(sdcard);
            return err;
        }

        err = sdcard_reply_base_check(reply);
        if(err != E_NO_ERROR){
            sdcard_timeout_end(sdcard);
            return err;
        }

        if(!(reply->r1.data & SDCARD_REPLY_R1_IN_IDLE_STATE)) break;

        if(sdcard->timeout){
            //sdcard_timeout_end(sdcard);
            return E_TIME_OUT;
        }
    }
    sdcard_timeout_end(sdcard);

    sdcard->card_type = SDCARD_TYPE_MMC;

    // Чтение конфигурации SD-карты.
    err = sdcard_init_update_conf(sdcard);
    if(err != E_NO_ERROR) return err;

    sdcard->initialized = true;

    return E_NO_ERROR;
}

/**
 * Проверяет поддерживаемые напряжения в регистре OCR.
 * @param sdcard SD-карта.
 * @param reply Ответ.
 * @return Код ошибки.
 */
static err_t sdcard_init_check_ocr_voltage(sdcard_t* sdcard, sdcard_reply_t* reply)
{
    err_t err = E_NO_ERROR;

    // Прочитаем OCR.
    err = sdcard_cmd(sdcard, &sdcard_cmd58, SDCARD_REPLY_R3, reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_base_check(reply);
    if(err != E_NO_ERROR) return err;

    // Если не поддерживается диапазон 2.7-3.6V,
    // значит неподдерживаемая карта.
    if((reply->r3.data & SDCARD_REPLY_R3_OCR_VOLTAGE_33V) == 0){
        return E_SDCARD_INVALID;
    }

    return E_NO_ERROR;
}

/**
 * Инициализирует карту и ожидает
 * её выхода из состояния IDLE.
 * @param sdcard SD-карта.
 * @param cmd Команда ACMD41.
 * @param reply Ответ.
 * @return Код ошибки.
 */
static err_t sdacrd_init_card_wait_idle(sdcard_t* sdcard, const sdcard_cmd_t* cmd, sdcard_reply_t* reply)
{
    err_t err = E_NO_ERROR;

    sdcard_timeout_begin(sdcard, SDCARD_TIMEOUT_INIT);
    for(;;){
        err = sdcard_acmd(sdcard, cmd, SDCARD_REPLY_R1, reply);
        if(err != E_NO_ERROR){
            sdcard_timeout_end(sdcard);
            return err;
        }

        err = sdcard_reply_base_check(reply);
        if(err != E_NO_ERROR){
            sdcard_timeout_end(sdcard);
            return err;
        }

        if(!(reply->r1.data & SDCARD_REPLY_R1_IN_IDLE_STATE)) break;

        if(sdcard->timeout){
            //sdcard_timeout_end(sdcard);
            return E_TIME_OUT;
        }
    }
    sdcard_timeout_end(sdcard);

    return E_NO_ERROR;
}

/**
 * Инициализирует SD-карту версии 1.
 * @param sdcard SD-карта.
 * @param reply Ответ SD-карты.
 * @return Код ошибки.
 */
static err_t sdcard_init_card_v1(sdcard_t* sdcard, sdcard_reply_t* reply)
{
    err_t err = E_NO_ERROR;

    // Проверим поддерживаемые напряжения.
    err = sdcard_init_check_ocr_voltage(sdcard, reply);
    if(err != E_NO_ERROR) return err;

    // Проверим на предмет MMC карты.
    err = sdcard_acmd(sdcard, &sdcard_acmd41_sc, SDCARD_REPLY_R1, reply);
    if(err != E_NO_ERROR){
        // Если команда ACMD41 не поддерживается.
        if(reply->r1.data & SDCARD_REPLY_R1_ILLEGAL_COMMAND){
            // Проинициализируем MMC карту.
            return sdcard_init_card_mmc(sdcard, reply);
        }
        return err;
    }

    // Продолжим инициализировать SD-карту версии 1.

    // Если карта в состоянии IDLE.
    if(reply->r1.data & SDCARD_REPLY_R1_IN_IDLE_STATE){
        // Подождём выхода из состояния IDLE.
        err = sdacrd_init_card_wait_idle(sdcard, &sdcard_acmd41_sc, reply);
        if(err != E_NO_ERROR) return err;
    }

    sdcard->card_type = SDCARD_TYPE_SDSCv1;

    // Чтение конфигурации SD-карты.
    err = sdcard_init_update_conf(sdcard);
    if(err != E_NO_ERROR) return err;

    sdcard->initialized = true;

    return E_NO_ERROR;
}

/**
 * Инициализирует SD-карту версии 2.
 * @param sdcard SD-карта.
 * @param reply Ответ SD-карты.
 * @return Код ошибки.
 */
static err_t sdcard_init_card_v2(sdcard_t* sdcard, sdcard_reply_t* reply)
{
    err_t err = E_NO_ERROR;

    // Проверим поддерживаемые напряжения.
    err = sdcard_init_check_ocr_voltage(sdcard, reply);
    if(err != E_NO_ERROR) return err;

    // Ожидаем инициализации и выход из состояния IDLE.
    err = sdacrd_init_card_wait_idle(sdcard, &sdcard_acmd41_hc, reply);
    if(err != E_NO_ERROR) return err;

    // Прочитаем регистр OCR
    // и определим тип карты.
    sdcard_ocr_t ocr;
    err = sdcard_read_ocr(sdcard, &ocr);
    if(err != E_NO_ERROR) return err;

    if(ocr.ccs) sdcard->card_type = SDCARD_TYPE_SDHC_SDXC;
    else sdcard->card_type = SDCARD_TYPE_SDSCv2;

    // Чтение конфигурации SD-карты.
    err = sdcard_init_update_conf(sdcard);
    if(err != E_NO_ERROR) return err;

    sdcard->initialized = true;

    return E_NO_ERROR;
}

/*static err_t __attribute__((noinline)) debug_err(err_t err)
{
    for(;;){
        __NOP();
    }

    return err;
}*/

err_t sdcard_init_card(sdcard_t* sdcard)
{
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    // Сброс данных от предыдущей sd-карты.
    // Сброс флага инициализации.
    sdcard->initialized = false;
    // Сброс типа SD-карты.
    sdcard->card_type = SDCARD_TYPE_UNKNOWN;
    // Сброс настроек CRC.
    //sdcard->crc_enabled = false;
    // Сброс размеров блоков.
    sdcard->rw_block_size = 0;
    sdcard->er_block_size = 0;

    size_t i;
    err_t err = E_NO_ERROR;

    sdcard_reply_t reply;

    sdcard_deselect(sdcard);

    if(!sdcard_init_begin_setup_spi(sdcard)) return E_BUSY;

    // Отправим 80 циклов тактирования.
    for(i = 0; i < SDCARD_INIT_DUMMY_BYTES; i ++){
        err = sdcard_nop(sdcard);
        if(err != E_NO_ERROR){
            sdcard_init_end_setup_spi(sdcard);
            return err;
        }
    }

    err = sdcard_select(sdcard);
    if(err != E_NO_ERROR){
        sdcard_deselect(sdcard);
        sdcard_init_end_setup_spi(sdcard);
        return err;
    }

    // Отправим команду сброса - GO_IDLE_STATE (CMD0).
    err = sdcard_cmd(sdcard, &sdcard_cmd0, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR){
        sdcard_deselect(sdcard);
        sdcard_init_end_setup_spi(sdcard);
        return err;
    }

    // Проверим ответ.
    err = sdcard_reply_base_check(&reply);
    if(err != E_NO_ERROR) return err;

    // Успешно - получили ответ SD-карты.
    // Отправим команду проверки поддержки нужного напряжения - SEND_IF_COND (CMD8).
    err = sdcard_cmd(sdcard, &sdcard_cmd8, SDCARD_REPLY_R7, &reply);
    if(err != E_NO_ERROR){
        sdcard_deselect(sdcard);
        sdcard_init_end_setup_spi(sdcard);
        return err;
    }

    // Если ошибка CRC - возвратим её.
    if(reply.r1.data & SDCARD_REPLY_R1_COM_CRC_ERROR){
        sdcard_deselect(sdcard);
        sdcard_init_end_setup_spi(sdcard);

        return E_CRC;
    }

    // Если SEND_IF_COND - недопустимая команда,
    // значит имеем дело с картой SDv1.
    if(reply.r1.data & SDCARD_REPLY_R1_ILLEGAL_COMMAND){
        // Попытаемся проинициализировать карту SDv1.
        err = sdcard_init_card_v1(sdcard, &reply);

        sdcard_deselect(sdcard);
        sdcard_init_end_setup_spi(sdcard);

        return err;
    }

    // Если получен ошибочный ответ - карта непригодна.
    if(reply.r7.data != SDCARD_SEND_IF_COND_CHECK){
        sdcard_deselect(sdcard);
        sdcard_init_end_setup_spi(sdcard);
        return E_SDCARD_INVALID;
    }

    // Попытаемся проинициализировать карту SDv2.
    err = sdcard_init_card_v2(sdcard, &reply);

    sdcard_deselect(sdcard);
    sdcard_init_end_setup_spi(sdcard);

    return err;
}

err_t sdcard_send_cmd(sdcard_t* sdcard, const sdcard_cmd_t* cmd,
                      sdcard_reply_type_t reply_type, sdcard_reply_t* reply)
{
    if(cmd == NULL) return E_NULL_POINTER;
    if(reply == NULL) return E_NULL_POINTER;
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    return sdcard_cmd(sdcard, cmd, reply_type, reply);
}

err_t sdcard_send_acmd(sdcard_t* sdcard, const sdcard_cmd_t* cmd,
                       sdcard_reply_type_t reply_type, sdcard_reply_t* reply)
{
    if(cmd == NULL) return E_NULL_POINTER;
    if(reply == NULL) return E_NULL_POINTER;
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    return sdcard_acmd(sdcard, cmd, reply_type, reply);
}

err_t sdcard_read_status(sdcard_t* sdcard, sdcard_status_t* status)
{
    if(status == NULL) return E_NULL_POINTER;
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    sdcard_reply_t reply;
    err_t err = E_NO_ERROR;

    // Прочитаем состояние.
    err = sdcard_cmd(sdcard, &sdcard_cmd13, SDCARD_REPLY_R2, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_base_check(&reply);
    if(err != E_NO_ERROR) return err;

    // Получен верный ответ.
    status->data[0] = reply.r2.data;
    status->data[1] = reply.r1.data;

    return E_NO_ERROR;
}


err_t sdcard_read_ocr(sdcard_t* sdcard, sdcard_ocr_t* ocr)
{
    if(ocr == NULL) return E_NULL_POINTER;
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    sdcard_reply_t reply;
    err_t err = E_NO_ERROR;

    // Прочитаем OCR.
    err = sdcard_cmd(sdcard, &sdcard_cmd58, SDCARD_REPLY_R3, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_base_check(&reply);
    if(err != E_NO_ERROR) return err;

    // Получен верный ответ.
    memcpy(ocr, &reply.r3.data, SDCARD_OCR_SIZE);

    return E_NO_ERROR;
}

err_t sdcard_read_csd(sdcard_t* sdcard, sdcard_csd_t* csd)
{
    if(csd == NULL) return E_NULL_POINTER;
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    err_t err = E_NO_ERROR;

    sdcard_reply_t reply;

    // Отправим команду.
    err = sdcard_cmd(sdcard, &sdcard_cmd9, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    // Прочитаем данные команды.
    err = sdcard_read_cmd_data(sdcard, csd, SDCARD_CSD_SIZE);
    if(err != E_NO_ERROR) return err;

    uint8_t crc = crc7_sdcard(csd, SDCARD_CSD_SIZE - 1);
    uint8_t crc_val = csd->data[SDCARD_CSD_SIZE - 1] >> 1;

    if(crc != crc_val) return E_CRC;

    sdcard_data_reverse((uint8_t*)csd, SDCARD_CSD_SIZE);

    return E_NO_ERROR;
}

err_t sdcard_read_cid(sdcard_t* sdcard, sdcard_cid_t* cid)
{
    if(cid == NULL) return E_NULL_POINTER;
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    err_t err = E_NO_ERROR;

    sdcard_reply_t reply;

    // Отправим команду.
    err = sdcard_cmd(sdcard, &sdcard_cmd10, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    // Прочитаем данные команды.
    err = sdcard_read_cmd_data(sdcard, cid, SDCARD_CID_SIZE);
    if(err != E_NO_ERROR) return err;

    uint8_t crc = crc7_sdcard(cid, SDCARD_CID_SIZE - 1);
    uint8_t crc_val = cid->data[SDCARD_CID_SIZE - 1] >> 1;

    if(crc != crc_val) return E_CRC;

    sdcard_data_reverse((uint8_t*)cid, SDCARD_CID_SIZE);

    return E_NO_ERROR;
}

err_t sdcard_stop_transmission(sdcard_t* sdcard)
{
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    sdcard_reply_t reply;
    err_t err = E_NO_ERROR;

    // Остановим передачу данных.
    err = sdcard_cmd_stop_transmission(sdcard, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t sdcard_set_crc_enabled(sdcard_t* sdcard, bool enabled)
{
    // Если карта не идентифицирована -
    // Вызов произошёл до инициализации.
    if(sdcard->card_type == SDCARD_TYPE_UNKNOWN){
        // Просто сохраним разрешение CRC
        // для установки при инциализации.
        sdcard->crc_enabled = enabled;
        return E_NO_ERROR;
    }

    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    sdcard_reply_t reply;
    err_t err = E_NO_ERROR;

    sdcard_cmd_t cmd;
    uint32_t arg;

    arg = enabled ? SDCARD_CMD_CRC_ON_OFF_CRC_OPTION : 0;

    sdcard_cmd_setup(&cmd, SDCARD_CMD_CRC_ON_OFF, arg);

    // Отправим команду.
    err = sdcard_cmd(sdcard, &cmd, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    sdcard->crc_enabled = enabled;

    return E_NO_ERROR;
}

err_t sdcard_set_block_len(sdcard_t* sdcard, uint32_t blocklen)
{
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;
    if(blocklen > SDCARD_BLOCK_SIZE) return E_INVALID_VALUE;

    sdcard_reply_t reply;
    err_t err = E_NO_ERROR;

    sdcard_cmd_t cmd;
    uint32_t arg;

    arg = blocklen;

    sdcard_cmd_setup(&cmd, SDCARD_CMD_SET_BLOCKLEN, arg);

    // Отправим команду.
    err = sdcard_cmd(sdcard, &cmd, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    if(sdcard->card_type != SDCARD_TYPE_SDHC_SDXC){
        sdcard->rw_block_size = blocklen;
    }

    return E_NO_ERROR;
}

err_t sdcard_blocks_written(sdcard_t* sdcard, uint32_t* blocks)
{
    if(blocks == NULL) return E_NULL_POINTER;
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    sdcard_reply_t reply;
    err_t err = E_NO_ERROR;

    // Остановим передачу данных.
    err = sdcard_acmd(sdcard, &sdcard_acmd22, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    // Прочитаем данные команды.
    err = sdcard_read_cmd_data(sdcard, blocks, sizeof(uint32_t));
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t sdcard_set_card_detect(sdcard_t* sdcard, bool enable)
{
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    sdcard_reply_t reply;
    err_t err = E_NO_ERROR;

    sdcard_cmd_t cmd;
    uint32_t arg;

    arg = (uint32_t) enable;

    sdcard_cmd_setup(&cmd, SDCARD_ACMD_SET_CLR_CARD_DETECT, arg);

    // Отправим команду.
    err = sdcard_acmd(sdcard, &cmd, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;

    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t sdcard_read(sdcard_t* sdcard, uint32_t address, void* data, size_t size, size_t* readed)
{
    if(data == NULL) return E_NULL_POINTER;
    if(size == 0) return E_INVALID_VALUE;
    if(!sdcard->initialized) return E_STATE;

    // Если запрещено чтение данных меньше блока -
    // размер блока должен быть 512.
    if(!sdcard_partial_block_read(sdcard) &&
            (sdcard->rw_block_size != SDCARD_BLOCK_SIZE)){
        return E_STATE;
    }

    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    err_t err = E_NO_ERROR;

    // Одиночное чтение.
    if(size <= sdcard->rw_block_size){
        err = sdcard_read_single_data_block(sdcard, address, data, size, readed);
    }
    // Множественное чтение.
    else{
        err = sdcard_read_multiple_data_block(sdcard, address, data, size, readed);
    }
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t sdcard_write(sdcard_t* sdcard, uint32_t address, const void* data, size_t size, size_t* written)
{
    if(data == NULL) return E_NULL_POINTER;
    if(size == 0) return E_INVALID_VALUE;
    if(!sdcard->initialized) return E_STATE;

    // Если запрещено чтение данных меньше блока -
    // размер блока должен быть 512.
    if(!sdcard_partial_block_read(sdcard) &&
            (sdcard->rw_block_size != SDCARD_BLOCK_SIZE)){
        return E_STATE;
    }

    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    err_t err = E_NO_ERROR;

    // Одиночная запись.
    if(size <= sdcard->rw_block_size){
        err = sdcard_write_single_data_block(sdcard, address, data, size, written);
    }
    // Множественная запись.
    else{
        err = sdcard_write_multiple_data_block(sdcard, address, data, size, written);
    }
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t sdcard_erase(sdcard_t* sdcard, uint32_t address_start, uint32_t address_end)
{
    if(!sdcard->initialized) return E_STATE;
    if(!sdcard_wait_current_op(sdcard)) return E_BUSY;

    err_t err = E_NO_ERROR;

    sdcard_cmd_t cmd_start_addr;
    sdcard_cmd_t cmd_end_addr;
    sdcard_reply_t reply;

    // Индекс команды установки начального адреса стирания.
    uint8_t cmd_start_addr_index;
    // Индекс команды установки конечного адреса стирания.
    uint8_t cmd_end_addr_index;

    // Определим нужные индексы
    // для команд начального и
    // конечного адреса стирания.
    // SD-карта.
    if(sdcard->card_type != SDCARD_TYPE_MMC){
        cmd_start_addr_index = SDCARD_CMD_ERASE_WR_BLK_START_ADDR;
        cmd_end_addr_index = SDCARD_CMD_ERASE_WR_BLK_END_ADDR;
    }// MMC-карта.
    else{
        cmd_start_addr_index = SDCARD_CMD_MMC_TAG_ERASE_GROUP_START;
        cmd_end_addr_index = SDCARD_CMD_MMC_TAG_ERASE_GROUP_END;
    }

    // Подготовим команды для задания адресов.
    sdcard_cmd_setup(&cmd_start_addr, cmd_start_addr_index, address_start);
    sdcard_cmd_setup(&cmd_end_addr, cmd_end_addr_index, address_end);

    // Установим начальный адрес стирания.
    err = sdcard_cmd(sdcard, &cmd_start_addr, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;
    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    // Установим конечный адрес стирания.
    err = sdcard_cmd(sdcard, &cmd_end_addr, SDCARD_REPLY_R1, &reply);
    if(err != E_NO_ERROR) return err;
    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    // Выполним стирание.
    err = sdcard_cmd(sdcard, &sdcard_cmd38, SDCARD_REPLY_R1b, &reply);
    if(err != E_NO_ERROR) return err;
    // Проверим ответ.
    err = sdcard_reply_check(&reply);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t sdcard_read_sector(sdcard_t* sdcard, uint32_t number, size_t count, void* data, size_t* readed)
{
    if(sdcard->rw_block_size != SDCARD_BLOCK_SIZE) return E_STATE;
    if(count > SDCARD_MAX_RW_SECTORS_COUNT) return E_OUT_OF_RANGE;
    if(count + number > sdcard_sectors_count(sdcard)) return E_OUT_OF_RANGE;

    size_t address = number;
    size_t size = count << 9;

    // Карты MMC/SDSC имеют побайтовую адресацию -
    // для них нужно умножить номер сектора на размер блока.
    if(sdcard->card_type != SDCARD_TYPE_SDHC_SDXC){
        if(number > SDCARD_SDSC_MMC_MAX_RW_SECTOR_NUMBER) return E_OUT_OF_RANGE;
        address <<= 9;
    }

    return sdcard_read(sdcard, address, data, size, readed);
}

err_t sdcard_write_sector(sdcard_t* sdcard, uint32_t number, size_t count, const void* data, size_t* written)
{
    if(sdcard->rw_block_size != SDCARD_BLOCK_SIZE) return E_STATE;
    if(count > SDCARD_MAX_RW_SECTORS_COUNT) return E_OUT_OF_RANGE;
    if(count + number > sdcard_sectors_count(sdcard)) return E_OUT_OF_RANGE;

    size_t address = number;
    size_t size = count << 9;

    // Карты MMC/SDSC имеют побайтовую адресацию -
    // для них нужно умножить номер сектора на размер блока.
    if(sdcard->card_type != SDCARD_TYPE_SDHC_SDXC){
        if(number > SDCARD_SDSC_MMC_MAX_RW_SECTOR_NUMBER) return E_OUT_OF_RANGE;
        address <<= 9;
    }

    return sdcard_write(sdcard, address, data, size, written);
}

err_t sdcard_erase_sector(sdcard_t* sdcard, uint32_t number, size_t count)
{
    if(count == 0) return E_INVALID_VALUE;
    if(sdcard->rw_block_size != SDCARD_BLOCK_SIZE) return E_STATE;
    if(count > SDCARD_MAX_RW_SECTORS_COUNT) return E_OUT_OF_RANGE;
    if(count + number > sdcard_sectors_count(sdcard)) return E_OUT_OF_RANGE;

    size_t address_start = number;
    size_t address_end = number + (count - 1);

    // Карты MMC/SDSC имеют побайтовую адресацию -
    // для них нужно умножить номер сектора на размер блока.
    if(sdcard->card_type != SDCARD_TYPE_SDHC_SDXC){
        if(number > SDCARD_SDSC_MMC_MAX_RW_SECTOR_NUMBER) return E_OUT_OF_RANGE;
        address_start <<= 9;
        address_end <<= 9;
    }

    return sdcard_erase(sdcard, address_start, address_end);
}
