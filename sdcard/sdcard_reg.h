/**
 * @file sdcard_reg.h Декларация регистров SD-карты.
 */

#ifndef SDCARD_REG_H_
#define SDCARD_REG_H_

#include <stdint.h>
#include <assert.h>


// Состояние карты.
// Ответ на команду SEND_STATUS (R2).
//! Размер состояния SD-карты.
#define SDCARD_STATUS_SIZE 2

//! Структура состояния.
#pragma pack(push, 1)
typedef union _SD_Card_Status {
    struct {
        unsigned card_locked:1; //!< Карта заблокирована.
        unsigned wp_erase_lock_fail:1; //!< Стирания защищённого участка, ошибка lock/unlock.
        unsigned error:1; //!< Общая ошибка.
        unsigned cc_error:1; //!< Ошибка контроллера карты.
        unsigned card_ecc_failed:1; //!< Ошибка ECC карты.
        unsigned wp_violation:1; //!< Запись в защищённый участок.
        unsigned erase_param:1; //!< Ошибка параметров стирания.
        unsigned out_of_rng_csd_ovrwr:1; //!< Выход из диапазона, перезапись CSD.
        unsigned in_idle_state:1; //!< В состоянии простоя.
        unsigned erase_reset:1; //!< Сброс стирания.
        unsigned illegal_command:1; //!< Недопустимая команда.
        unsigned com_crc_error:1; //!< Ошибка CRC команды.
        unsigned erase_seq_error:1; //!< Ошибка последовательности стирания.
        unsigned address_error:1; //!< Ошибка адреса.
        unsigned parameter_error:1; //!< Ошибка параметра.
        unsigned zero:1; //!< Всегда 0.
    };
    uint8_t data[SDCARD_STATUS_SIZE];
} sdcard_status_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_status_t) == SDCARD_STATUS_SIZE, "Invalid size of sdcard status!");


// Регистр OCR.

//! Размер регистра OCR.
#define SDCARD_OCR_SIZE 4

//! Структура регистра OCR.
#pragma pack(push, 1)
typedef union _SD_Card_OCR {
    struct {
        unsigned reserved0_14:15;
        unsigned voltage27_28:1;
        unsigned voltage28_29:1;
        unsigned voltage29_30:1;
        unsigned voltage30_31:1;
        unsigned voltage31_32:1;
        unsigned voltage32_33:1;
        unsigned voltage33_34:1;
        unsigned voltage34_35:1;
        unsigned voltage35_36:1;
        unsigned s18a:1; //!< Switching to 1.8V accepted.
        unsigned reserved25_28:4;
        unsigned uhs2cs:1; //!< UHS-II Card Status.
        unsigned ccs:1; //!< Card Capacity Status.
        unsigned busy:1; //!< Set to low if card is busy.
    };
    uint8_t data[SDCARD_OCR_SIZE];
} sdcard_ocr_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_ocr_t) == SDCARD_OCR_SIZE, "Invalid size of sdcard OCR!");


// Регистр CID.

//! Размер регистра CID.
#define SDCARD_CID_SIZE 16

//! Структура регистра CID SD-карты.
#pragma pack(push, 1)
typedef struct _SD_Card_SD_CID {
    unsigned one:1; //!< Не используется, всегда 1.
    unsigned crc:7; //!< CRC7.
    unsigned mdt_m:4; //!< Месяц выпуска, 1 - январь.
    unsigned mdt_y:8; //!< Год выпуска начиная с 2000.
    unsigned reserved20_23:4;
    uint8_t psn[4]; //!< Product serial number.
    uint8_t prv; //!< Product revision.
    uint8_t pnm[5]; //!< Product name.
    uint8_t oid[2]; //!< OEM/Application ID.
    uint8_t mid; //!< Manufacturer ID.
} sdcard_sd_cid_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_sd_cid_t) == SDCARD_CID_SIZE, "Invalid size of sdcard CID!");

//! Структура регистра CID MMC-карты.
#pragma pack(push, 1)
typedef struct _SD_Card_MMC_CID {
    unsigned one:1; //!< Не используется, всегда 1.
    unsigned crc:7; //!< CRC7.
    uint8_t mdt; //!< Дата выпуска.
    uint8_t psn[4]; //!< Product serial number.
    uint8_t prv; //!< Product revision.
    uint8_t pnm[6]; //!< Product name.
    uint8_t oid[2]; //!< OEM/Application ID.
    uint8_t mid; //!< Manufacturer ID.
} sdcard_mmc_cid_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_mmc_cid_t) == SDCARD_CID_SIZE, "Invalid size of sdcard MMC CID!");

// Общий регистр.

//! Структура регистра CID.
#pragma pack(push, 1)
typedef union _SD_Card_CID {
    sdcard_sd_cid_t sd_cid; //!< Регистр CID SD-карты.
    sdcard_mmc_cid_t mmc_cid; //!< Регистр CID MMC-карты.
    uint8_t data[SDCARD_CID_SIZE];
} sdcard_cid_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_cid_t) == SDCARD_CID_SIZE, "Invalid size of sdcard CID!");


// Регистр CSD.

//! Размер регистра CSD.
#define SDCARD_CSD_SIZE 16

// Регистр CSD SD-карты версии 1.

//! Структура регистра CSD SD-карты версии 1.
#pragma pack(push, 1)
typedef struct _SD_Card_SD_CSD1 {
    unsigned one:1; //!< Не используется, всегда 1.
    unsigned crc:7; //!< CRC7.
    unsigned reserved8_9:2;
    unsigned file_format:2; //!< File format.
    unsigned tmp_write_protect:1; //!< Temporary write protect.
    unsigned perm_write_protect:1; //!< Permanent write protect.
    unsigned copy:1; //!< Copy flag.
    unsigned file_format_grp:1; //!< File format group.
    unsigned reserved16_20:5;
    unsigned write_bl_partial:1; //!< Partial blocks for write allowed.
    unsigned write_bl_len:4; //!< Max write data block length.
    unsigned r2w_factor:3; //!< Write speed factor.
    unsigned reserved29_30:2;
    unsigned wp_grp_enable:1; //!< Write protect group enable.
    unsigned wp_grp_size:7; //!< Write protect group size.
    unsigned sector_size:7; //!< Erase sector size.
    unsigned erase_blk_en:1; //!< Erase single block enable.
    unsigned c_size_mult:3; //!< Device size multiplier.
    unsigned vdd_w_curr_max:3; //!< Max write current @ Vdd max.
    unsigned vdd_w_curr_min:3; //!< Max write current @ Vdd min.
    unsigned vdd_r_curr_max:3; //!< Max read current @ Vdd max.
    unsigned vdd_r_curr_min:3; //!< Max read current @ Vdd min.
    unsigned c_size:12; //!< Device size.
    unsigned reserved74_75:2;
    unsigned dsr_imp:1; //!< DSR implemented.
    unsigned read_blk_misalign:1; //!< Read block misalignment.
    unsigned write_blk_misalign:1; //!< Write block misalignment.
    unsigned read_bl_partial:1; //!< Partial blocks for read allowed.
    unsigned read_bl_len:4; //!< Max read data block length.
    unsigned ccc:12; //!< Card command classes.
    uint8_t tran_speed; //!< Max data transfer rate.
    uint8_t nsac; //!< Data read access-time-2 in clk cycles.
    uint8_t taac; //!< Data read access-time-1.
    unsigned reserved120_125:6;
    unsigned csd_structure:2; //!< CSD structure.
} sdcard_sd_csd1_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_sd_csd1_t) == SDCARD_CSD_SIZE, "Invalid size of sdcard SD CSDv1!");

// Регистр CSD SD-карты версии 2.

//! Структура регистра CSD SD-карты версии 2.
#pragma pack(push, 1)
typedef struct _SD_Card_SD_CSD2 {
    unsigned one:1; //!< Не используется, всегда 1.
    unsigned crc:7; //!< CRC7.
    unsigned reserved8_9:2;
    unsigned file_format:2; //!< File format.
    unsigned tmp_write_protect:1; //!< Temporary write protect.
    unsigned perm_write_protect:1; //!< Permanent write protect.
    unsigned copy:1; //!< Copy flag.
    unsigned file_format_grp:1; //!< File format group.
    unsigned reserved16_20:5;
    unsigned write_bl_partial:1; //!< Partial blocks for write allowed.
    unsigned write_bl_len:4; //!< Max write data block length.
    unsigned r2w_factor:3; //!< Write speed factor.
    unsigned reserved29_30:2;
    unsigned wp_grp_enable:1; //!< Write protect group enable.
    unsigned wp_grp_size:7; //!< Write protect group size.
    unsigned sector_size:7; //!< Erase sector size.
    unsigned erase_blk_en:1; //!< Erase single block enable.
    unsigned reserved47:1;
    unsigned c_size:22; //!< Device size.
    unsigned reserved70_76:2;
    unsigned dsr_imp:1; //!< DSR implemented.
    unsigned read_blk_misalign:1; //!< Read block misalignment.
    unsigned write_blk_misalign:1; //!< Write block misalignment.
    unsigned read_bl_partial:1; //!< Partial blocks for read allowed.
    unsigned read_bl_len:4; //!< Max read data block length.
    unsigned ccc:12; //!< Card command classes.
    uint8_t tran_speed; //!< Max data transfer rate.
    uint8_t nsac; //!< Data read access-time-2 in clk cycles.
    uint8_t taac; //!< Data read access-time-1.
    unsigned reserved120_125:6;
    unsigned csd_structure:2; //!< CSD structure.
} sdcard_sd_csd2_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_sd_csd2_t) == SDCARD_CSD_SIZE, "Invalid size of sdcard SD CSDv2!");

// Регистр CSD MMC-карты.

//! Структура регистра CSD MMC-карты.
#pragma pack(push, 1)
typedef struct _SD_Card_MMC_CSD {
    unsigned one:1; //!< Не используется, всегда 1.
    unsigned crc:7; //!< CRC7.
    unsigned ecc:2; //!< ECC code.
    unsigned file_format:2; //!< File format.
    unsigned tmp_write_protect:1; //!< Temporary write protect.
    unsigned perm_write_protect:1; //!< Permanent write protect.
    unsigned copy:1; //!< Copy flag.
    unsigned file_format_grp:1; //!< File format group.
    unsigned reserved16_20:5;
    unsigned write_bl_partial:1; //!< Partial blocks for write allowed.
    unsigned write_bl_len:4; //!< Max write data block length.
    unsigned r2w_factor:3; //!< Write speed factor.
    unsigned default_ecc:2; //!< Manufacturer default ECC.
    unsigned wp_grp_enable:1; //!< Write protect group enable.
    unsigned wp_grp_size:5; //!< Write protect group size.
    unsigned erase_grp_mult:5; //!< Erase group size multiplier.
    unsigned erase_grp_size:5; //!< Erase group size.
    unsigned c_size_mult:3; //!< Device size multiplier.
    unsigned vdd_w_curr_max:3; //!< Max write current @ Vdd max.
    unsigned vdd_w_curr_min:3; //!< Max write current @ Vdd min.
    unsigned vdd_r_curr_max:3; //!< Max read current @ Vdd max.
    unsigned vdd_r_curr_min:3; //!< Max read current @ Vdd min.
    unsigned c_size:12; //!< Device size.
    unsigned reserved74_75:2;
    unsigned dsr_imp:1; //!< DSR implemented.
    unsigned read_blk_misalign:1; //!< Read block misalignment.
    unsigned write_blk_misalign:1; //!< Write block misalignment.
    unsigned read_bl_partial:1; //!< Partial blocks for read allowed.
    unsigned read_bl_len:4; //!< Max read data block length.
    unsigned ccc:12; //!< Card command classes.
    uint8_t tran_speed; //!< Max data transfer rate.
    uint8_t nsac; //!< Data read access-time-2 in clk cycles.
    uint8_t taac; //!< Data read access-time-1.
    unsigned reserved120_121:2;
    unsigned spec_vers:4; //!< Spec version.
    unsigned csd_structure:2; //!< CSD structure.
} sdcard_mmc_csd_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_mmc_csd_t) == SDCARD_CSD_SIZE, "Invalid size of sdcard MMC CSD!");

// Общий регистр.

// Полу CSD_STRUCTURE.
//! SD CSDv1.
#define SDCARD_CSD_STRUCTURE_SD_CSD_V1 0
//! SD CSDv2.
#define SDCARD_CSD_STRUCTURE_SD_CSD_V2 1
//! MMC CSD.
#define SDCARD_CSD_STRUCTURE_MMC_CSD 2

//! Структура регистра CSD.
#pragma pack(push, 1)
typedef union _SD_Card_CSD {
    sdcard_sd_csd1_t sd_csd1; //!< Регистр CSD SD-карты версии 1.
    sdcard_sd_csd2_t sd_csd2; //!< Регистр CSD SD-карты версии 2.
    sdcard_mmc_csd_t mmc_csd; //!< Регистр CSD MMC-карты.
    uint8_t data[SDCARD_CSD_SIZE];
} sdcard_csd_t;
#pragma pack(pop)

static_assert(sizeof(sdcard_csd_t) == SDCARD_CSD_SIZE, "Invalid size of sdcard CSD!");


#endif /* SDCARD_REG_H_ */
