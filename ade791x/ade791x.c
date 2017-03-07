#include "ade791x.h"
#include <string.h>
#include "utils/utils.h"
#include "bits/bits.h"
#include "utils/delay.h"
#include "crc/crc16_ccitt.h"

// Debug.
#define ADE791X_GET_MEM_DEBUG

// Регистры и биты в них.
// IWV
#define ADE791X_IWV_ADDR 0x0
#define ADE791X_IWV_SIZE 3

// V1WV
#define ADE791X_V1WV_ADDR 0x1
#define ADE791X_V1WV_SIZE 3

// V2WV
#define ADE791X_V2WV_ADDR 0x2
#define ADE791X_V2WV_SIZE 3

// ADC_CRC
#define ADE791X_ADC_CRC_ADDR 0x4
#define ADE791X_ADC_CRC_SIZE 2

// CTRL_CRC
#define ADE791X_CTRL_CRC_ADDR 0x5
#define ADE791X_CTRL_CRC_SIZE 2

// CNT_SNAPSHOT
#define ADE791X_CNT_SNAPSHOT_ADDR 0x7
#define ADE791X_CNT_SNAPSHOT_SIZE 2
// Counter
#define ADE791X_CNT_SHAPSHOT_COUNTER_BIT 0
#define ADE791X_CNT_SHAPSHOT_COUNTER_SIZE 12
#define ADE791X_CNT_SHAPSHOT_COUNTER_MASK 0xfff

// CONFIG
#define ADE791X_CONFIG_ADDR 0x8
#define ADE791X_CONFIG_SIZE 1
// CLKOUT_EN
#define ADE791X_CONFIG_CLKOUT_EN_OFFSET 0
#define ADE791X_CONFIG_CLKOUT_EN_SIZE 1
#define ADE791X_CONFIG_CLKOUT_EN_MASK 0x1
// PWRDWN_EN
#define ADE791X_CONFIG_PWRDWN_EN_OFFSET 2
#define ADE791X_CONFIG_PWRDWN_EN_SIZE 1
#define ADE791X_CONFIG_PWRDWN_EN_MASK 0x1
// TEMP_EN
#define ADE791X_CONFIG_TEMP_EN_OFFSET 3
#define ADE791X_CONFIG_TEMP_EN_SIZE 1
#define ADE791X_CONFIG_TEMP_EN_MASK 0x1
// ADC_FREQ
#define ADE791X_CONFIG_ADC_FREQ_OFFSET 4
#define ADE791X_CONFIG_ADC_FREQ_SIZE 2
#define ADE791X_CONFIG_ADC_FREQ_MASK 0x3
// SWRST
#define ADE791X_CONFIG_SWRST_OFFSET 6
#define ADE791X_CONFIG_SWRST_SIZE 1
#define ADE791X_CONFIG_SWRST_MASK 0x1
// BW
#define ADE791X_CONFIG_BW_OFFSET 7
#define ADE791X_CONFIG_BW_SIZE 1
#define ADE791X_CONFIG_BW_MASK 0x1

// STATUS0
#define ADE791X_STATUS0_ADDR 0x9
#define ADE791X_STATUS0_SIZE 1
// RESET_ON
#define ADE791X_STATUS0_RESET_ON_OFFSET 0
#define ADE791X_STATUS0_RESET_ON_SIZE 1
#define ADE791X_STATUS0_RESET_ON_MASK 0x1
// CRC_STAT
#define ADE791X_STATUS0_CRC_STAT_OFFSET 1
#define ADE791X_STATUS0_CRC_STAT_SIZE 1
#define ADE791X_STATUS0_CRC_STAT_MASK 0x1
// IC_PROT
#define ADE791X_STATUS0_IC_PROT_OFFSET 2
#define ADE791X_STATUS0_IC_PROT_SIZE 1
#define ADE791X_STATUS0_IC_PROT_MASK 0x1

// LOCK
#define ADE791X_LOCK_ADDR 0xa
#define ADE791X_LOCK_SIZE 1

#define ADE791X_LOCK_KEY 0xca
#define ADE791X_UNLOCK_KEY 0x9c

// SYNC_SNAP
#define ADE791X_SYNC_SNAP_ADDR 0xb
#define ADE791X_SYNC_SNAP_SIZE 1
// SYNC
#define ADE791X_SYNC_SNAP_SYNC_OFFSET 0
#define ADE791X_SYNC_SNAP_SYNC_SIZE 1
#define ADE791X_SYNC_SNAP_SYNC_MASK 0x1
// SNAP
#define ADE791X_SYNC_SNAP_SNAP_OFFSET 1
#define ADE791X_SYNC_SNAP_SNAP_SIZE 1
#define ADE791X_SYNC_SNAP_SNAP_MASK 0x1

// COUNTER0
#define ADE791X_COUNTER0_ADDR 0xc
#define ADE791X_COUNTER0_SIZE 1

// COUNTER1
#define ADE791X_COUNTER1_ADDR 0xd
#define ADE791X_COUNTER1_SIZE 1

// EMI_CTRL
#define ADE791X_EMI_CTRL_ADDR 0xe
#define ADE791X_EMI_CTRL_SIZE 1

// STATUS1
#define ADE791X_STATUS1_ADDR 0xf
#define ADE791X_STATUS1_SIZE 1
// Version
#define ADE791X_STATUS1_VERSION_OFFSET 0
#define ADE791X_STATUS1_VERSION_SIZE 3
#define ADE791X_STATUS1_VERSION_MASK 0x7
// ADC_NA
#define ADE791X_STATUS1_ADC_NA_OFFSET 3
#define ADE791X_STATUS1_ADC_NA_SIZE 1
#define ADE791X_STATUS1_ADC_NA_MASK 0x1

// TEMPOS
#define ADE791X_TEMPOS_ADDR 0x18
#define ADE791X_TEMPOS_SIZE 1

/*
// 
#define ADE791X__ADDR 0x
#define ADE791X__SIZE 1
// 
#define ADE791X___OFFSET 
#define ADE791X___SIZE 1
#define ADE791X___MASK 0x1
*/


//! Размер команды.
#define ADE791X_CMD_SIZE 1
//! Смещение адреса в команде.
#define ADE791X_CMD_ADDRESS_OFFSET 3
//! Бит чтения данных.
#define ADE791X_CMD_READ_EN 0x4

//! Сообщение команды.
#define ADE791X_CMD_MESSAGE 0
//! Сообщение данных.
#define ADE791X_DATA_MESSAGE 1

#pragma pack(push, 1)
typedef struct _Ade791x_Burst_Data {
    uint8_t current[3];
    uint8_t voltage1[3];
    uint8_t voltage2[3];
    uint8_t crc[2];
} ade791x_burst_data_t;
#pragma pack(pop)


/**
 * Выставляет значения управляющих пинов для начала обмена данными с измерителем.
 * @param ade791x Измеритель.
 */
ALWAYS_INLINE static void ade791x_pins_begin(ade791x_t* ade791x)
{
    GPIO_ResetBits(ade791x->ce_gpio, ade791x->ce_pin);
}

/**
 * Выставлят значения управляющих пинов для окончания обмена данными с измерителем.
 * @param ade791x Измеритель.
 */
ALWAYS_INLINE static void ade791x_pins_end(ade791x_t* ade791x)
{
    GPIO_SetBits(ade791x->ce_gpio, ade791x->ce_pin);
}

/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool ade791x_wait_current_op(ade791x_t* ade791x)
{
    // Если шина занята, да ещё и не нами - возврат ошибки занятости.
    if(spi_bus_busy(ade791x->spi) && spi_bus_transfer_id(ade791x->spi) != ade791x->transfer_id) return false;
    
    future_wait(&ade791x->future);
    spi_bus_wait(ade791x->spi);
    
    return true;
}

/**
 * Начинает обмен данными с измерителем.
 * @param ade791x Измеритель.
 */
static void ade791x_begin(ade791x_t* ade791x)
{
    future_set_result(&ade791x->future, int_to_pvoid(E_NO_ERROR));
    
    future_start(&ade791x->future);
    
    ade791x_pins_begin(ade791x);
}

/**
 * Завершает обмен данными с измерителем.
 * @param ade791x Измеритель.
 * @param err Код ошибки.
 */
static void ade791x_end(ade791x_t* ade791x, err_t err)
{
    future_finish(&ade791x->future, int_to_pvoid(err));
    
    ade791x_pins_end(ade791x);
}

/*
static void ade791x_cmd_msg_end(spi_message_t* message)
{
    ade791x_t* ade791x = (ade791x_t*)message->sender_data;
    
    if(ade791x != NULL){
    }
}
*/

/**
 * Отправляет данные в измеритель.
 * Асинхронно.
 * @param ade791x Измеритель.
 * @param messages_count Число сообщений.
 * @return Код ошибки.
 */
static err_t ade791x_transfer(ade791x_t* ade791x, size_t messages_count)
{
    if(!spi_bus_set_transfer_id(ade791x->spi, ade791x->transfer_id)) return E_BUSY;
    
    ade791x_begin(ade791x);
    
    err_t err = spi_bus_transfer(ade791x->spi, ade791x->messages, messages_count);
    
    if(err != E_NO_ERROR){
        ade791x_end(ade791x, err);
        return err;
    }
    
    return E_NO_ERROR;
}

bool ade791x_spi_callback(ade791x_t* ade791x)
{
    if(spi_bus_transfer_id(ade791x->spi) != ade791x->transfer_id) return false;
    
    ade791x_end(ade791x, spi_bus_status(ade791x->spi) != SPI_STATUS_TRANSFERED ? E_IO_ERROR : E_NO_ERROR);
    
    return true;
}

err_t ade791x_init(ade791x_t* ade791x, ade791x_init_t* ade791x_init)
{
    // Clear.
    memset(ade791x, 0x0, sizeof(ade791x_t));
    
    // SPI.
    ade791x->spi = ade791x_init->spi;
    
    // CE.
    ade791x->ce_gpio = ade791x_init->ce_gpio;
    ade791x->ce_pin = ade791x_init->ce_pin;
    
    future_init(&ade791x->future);
    
    // Значения по-умолчанию для этих пинов.
    ade791x_pins_end(ade791x);
    
    // Идентификатор передачи.
    ade791x->transfer_id = ade791x_init->transfer_id;
    
    return E_NO_ERROR;
}

bool ade791x_busy(ade791x_t* ade791x)
{
    return future_running(&ade791x->future);
}

err_t ade791x_error(ade791x_t* ade791x)
{
    return pvoid_to_int(err_t, future_result(&ade791x->future));
}

err_t ade791x_wait(ade791x_t* ade791x)
{
    future_wait(&ade791x->future);
    return pvoid_to_int(err_t, future_result(&ade791x->future));
}

void ade791x_broadcast_begin(ade791x_t* ade791x)
{
    ade791x_pins_begin(ade791x);
}

void ade791x_broadcast_end(ade791x_t* ade791x)
{
    ade791x_pins_end(ade791x);
}

/**
 * Получает указатель на очередной пакет приёма/передачи.
 * @param ade791x Измеритель.
 * @param index Значение предыдущего индекса.
 * @return Указатель на пакет.
 */
ALWAYS_INLINE static uint8_t* ade791x_get_buffer(ade791x_t* ade791x, size_t size, size_t* index)
{
#ifdef ADE791X_GET_MEM_DEBUG
    if(index == NULL) return NULL;
#endif
    
    size_t i = *index;
    
#ifdef ADE791X_GET_MEM_DEBUG
    if(i + size > ADE791X_BUFFER_SIZE) return NULL;
#endif
    
    (*index) += size;
    
    return &ade791x->buffer[i];
}

/**
 * Получает указатель на сообщение.
 * @param ade791x Измеритель.
 * @param index Значение предыдущего индекса.
 * @return Указатель на данные в бефере.
 */
ALWAYS_INLINE static spi_message_t* ade791x_get_message(ade791x_t* ade791x, size_t* index)
{
#ifdef ADE791X_GET_MEM_DEBUG
    if(index == NULL) return NULL;
#endif
    
    size_t i = *index;
    
#ifdef ADE791X_GET_MEM_DEBUG
    if(i >= ADE791X_MESSAGES_COUNT) return NULL;
#endif
    
    (*index) ++;
    
    return &ade791x->messages[i];
}

err_t ade791x_read(ade791x_t* ade791x, ade791x_address_t addr, void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    if(size == 0) return E_INVALID_VALUE;
    
    if(!ade791x_wait_current_op(ade791x)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = ade791x_get_buffer(ade791x, ADE791X_CMD_SIZE, &buffer_index);
#ifdef ADE791X_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = ade791x_get_message(ade791x, &message_index);
#ifdef ADE791X_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = ade791x_get_message(ade791x, &message_index);
#ifdef ADE791X_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = (addr << ADE791X_CMD_ADDRESS_OFFSET) | ADE791X_CMD_READ_EN;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, ADE791X_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data, size);
    if(err != E_NO_ERROR) return err;
    
    return ade791x_transfer(ade791x, 2);
}

err_t ade791x_write(ade791x_t* ade791x, ade791x_address_t addr, const void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    if(size == 0) return E_INVALID_VALUE;
    
    if(!ade791x_wait_current_op(ade791x)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = ade791x_get_buffer(ade791x, ADE791X_CMD_SIZE, &buffer_index);
#ifdef ADE791X_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = ade791x_get_message(ade791x, &message_index);
#ifdef ADE791X_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = ade791x_get_message(ade791x, &message_index);
#ifdef ADE791X_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = (addr << ADE791X_CMD_ADDRESS_OFFSET);
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, ADE791X_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = spi_message_init(data_msg, SPI_WRITE, data, NULL, size);
    if(err != E_NO_ERROR) return err;
    
    return ade791x_transfer(ade791x, 2);
}

err_t ade791x_read_data(ade791x_t* ade791x, ade791x_data_t* data)
{
    if(data == NULL) return E_NULL_POINTER;
    
    ade791x_burst_data_t raw_data;
    
    err_t err = E_NO_ERROR;
    
    err = ade791x_read(ade791x, 0, &raw_data, sizeof(ade791x_burst_data_t));
    if(err != E_NO_ERROR) return err;
    
    err = ade791x_wait(ade791x);
    if(err != E_NO_ERROR) return err;
    
    uint16_t data_crc = ((uint16_t)raw_data.crc[0] << 8) | raw_data.crc[1];
    
    uint16_t crc = crc16_ccitt_first();
    crc = crc16_ccitt_next(crc, &raw_data.current[2]);
    crc = crc16_ccitt_next(crc, &raw_data.current[1]);
    crc = crc16_ccitt_next(crc, &raw_data.current[0]);
    crc = crc16_ccitt_next(crc, &raw_data.voltage1[2]);
    crc = crc16_ccitt_next(crc, &raw_data.voltage1[1]);
    crc = crc16_ccitt_next(crc, &raw_data.voltage1[0]);
    crc = crc16_ccitt_next(crc, &raw_data.voltage2[2]);
    crc = crc16_ccitt_next(crc, &raw_data.voltage2[1]);
    crc = crc16_ccitt_next(crc, &raw_data.voltage2[0]);
    
    if(crc != data_crc) return E_CRC;
    
    data->current = ade791x_expand(raw_data.current);
    data->voltage1 = ade791x_expand(raw_data.voltage1);
    data->voltage2 = ade791x_expand(raw_data.voltage2);
    
    return E_NO_ERROR;
}

/**
 * Читает значение 24 бит с расширением до 24 бит.
 * @param ade791x Измеритель.
 * @param address Адрес.
 * @param value Значение.
 * @return Код ошибки.
 */
static err_t ade791x_read_value24(ade791x_t* ade791x, ade791x_address_t address, int32_t* value)
{
    if(value == NULL) return E_NULL_POINTER;
    
    uint8_t data[3] = {0};
    err_t err = E_NO_ERROR;
    
    err = ade791x_read(ade791x, address, data, 3);
    if(err != E_NO_ERROR) return err;
    
    err = ade791x_wait(ade791x);
    if(err != E_NO_ERROR) return err;
    
    *value = ade791x_expand(data);
    
    return E_NO_ERROR;
}

/**
 * Читает значение 16 бит.
 * @param ade791x Измеритель.
 * @param address Адрес.
 * @param value Значение.
 * @return Код ошибки.
 */
static err_t ade791x_read_value16(ade791x_t* ade791x, ade791x_address_t address, uint16_t* value)
{
    if(value == NULL) return E_NULL_POINTER;
    
    uint8_t data[2] = {0};
    err_t err = E_NO_ERROR;
    
    err = ade791x_read(ade791x, address, data, 2);
    if(err != E_NO_ERROR) return err;
    
    err = ade791x_wait(ade791x);
    if(err != E_NO_ERROR) return err;
    
    *value = ((uint16_t)data[0] << 8) | data[1];
    
    return E_NO_ERROR;
}

/**
 * Читает значение 8 бит.
 * @param ade791x Измеритель.
 * @param address Адрес.
 * @param value Значение.
 * @return Код ошибки.
 */
static err_t ade791x_read_value8(ade791x_t* ade791x, ade791x_address_t address, uint8_t* value)
{
    if(value == NULL) return E_NULL_POINTER;
    
    err_t err = ade791x_read(ade791x, address, value, 1);
    if(err != E_NO_ERROR) return err;
    
    return ade791x_wait(ade791x);
}

err_t ade791x_read_current(ade791x_t* ade791x, int32_t* value)
{
    return ade791x_read_value24(ade791x, ADE791X_IWV_ADDR, value);
}

err_t ade791x_read_voltage1(ade791x_t* ade791x, int32_t* value)
{
    return ade791x_read_value24(ade791x, ADE791X_V1WV_ADDR, value);
}

err_t ade791x_read_voltage2(ade791x_t* ade791x, int32_t* value)
{
    return ade791x_read_value24(ade791x, ADE791X_V2WV_ADDR, value);
}

err_t ade791x_read_adc_crc(ade791x_t* ade791x, uint16_t* crc)
{
    return ade791x_read_value16(ade791x, ADE791X_ADC_CRC_ADDR, crc);
}

err_t ade791x_read_conf_crc(ade791x_t* ade791x, uint16_t* crc)
{
    return ade791x_read_value16(ade791x, ADE791X_CTRL_CRC_ADDR, crc);
}

err_t ade791x_read_counter_snapshot(ade791x_t* ade791x, uint16_t* cnt_snapshot)
{
    return ade791x_read_value16(ade791x, ADE791X_CNT_SNAPSHOT_ADDR, cnt_snapshot);
}

err_t ade791x_read_counter0(ade791x_t* ade791x, uint8_t* counter)
{
    return ade791x_read_value8(ade791x, ADE791X_COUNTER0_ADDR, counter);
}

err_t ade791x_read_counter1(ade791x_t* ade791x, uint8_t* counter)
{
    return ade791x_read_value8(ade791x, ADE791X_COUNTER1_ADDR, counter);
}

err_t ade791x_read_config(ade791x_t* ade791x, ade791x_config_t* config)
{
    if(config == NULL) return E_NULL_POINTER;
    
    uint8_t data = 0;
    err_t err = E_NO_ERROR;
    
    err = ade791x_read(ade791x, ADE791X_CONFIG_ADDR, &data, ADE791X_CONFIG_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = ade791x_wait(ade791x);
    if(err != E_NO_ERROR) return err;
    
    config->clock_out_enabled = BIT_VALUE_MASK(data, ADE791X_CONFIG_CLKOUT_EN_OFFSET, ADE791X_CONFIG_CLKOUT_EN_MASK);
    config->power_down_enabled = BIT_VALUE_MASK(data, ADE791X_CONFIG_PWRDWN_EN_OFFSET, ADE791X_CONFIG_PWRDWN_EN_MASK);
    config->temp_enabled = BIT_VALUE_MASK(data, ADE791X_CONFIG_TEMP_EN_OFFSET, ADE791X_CONFIG_TEMP_EN_MASK);
    config->adc_freq = BIT_VALUE_MASK(data, ADE791X_CONFIG_ADC_FREQ_OFFSET, ADE791X_CONFIG_ADC_FREQ_MASK);
    config->soft_reset = BIT_VALUE_MASK(data, ADE791X_CONFIG_SWRST_OFFSET, ADE791X_CONFIG_SWRST_MASK);
    config->adc_lpf = BIT_VALUE_MASK(data, ADE791X_CONFIG_BW_OFFSET, ADE791X_CONFIG_BW_MASK);
    //config-> = BIT_VALUE_MASK(data, ADE791X_CONFIG__OFFSET, ADE791X_CONFIG__MASK);
    
    return E_NO_ERROR;
}

err_t ade791x_write_config(ade791x_t* ade791x, const ade791x_config_t* config)
{
    if(config == NULL) return E_NULL_POINTER;
    
    uint8_t data = 0;
    err_t err = E_NO_ERROR;
    
    data |= config->clock_out_enabled << ADE791X_CONFIG_CLKOUT_EN_OFFSET;
    data |= config->power_down_enabled << ADE791X_CONFIG_PWRDWN_EN_OFFSET;
    data |= config->temp_enabled << ADE791X_CONFIG_TEMP_EN_OFFSET;
    data |= config->adc_freq << ADE791X_CONFIG_ADC_FREQ_OFFSET;
    data |= config->soft_reset << ADE791X_CONFIG_SWRST_OFFSET;
    data |= config->adc_lpf << ADE791X_CONFIG_BW_OFFSET;
    //data |= config-> << ADE791X_CONFIG_;
    
    err = ade791x_write(ade791x, ADE791X_CONFIG_ADDR, &data, ADE791X_CONFIG_SIZE);
    if(err != E_NO_ERROR) return err;
    
    return ade791x_wait(ade791x);
}

err_t ade791x_read_status0(ade791x_t* ade791x, ade791x_status0_t* status)
{
    if(status == NULL) return E_NULL_POINTER;
    
    uint8_t data = 0;
    err_t err = E_NO_ERROR;
    
    err = ade791x_read(ade791x, ADE791X_STATUS0_ADDR, &data, ADE791X_STATUS0_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = ade791x_wait(ade791x);
    if(err != E_NO_ERROR) return err;
    
    status->reset_in_progress = BIT_VALUE_MASK(data, ADE791X_STATUS0_RESET_ON_OFFSET, ADE791X_STATUS0_RESET_ON_MASK);
    status->crc_changed = BIT_VALUE_MASK(data, ADE791X_STATUS0_CRC_STAT_OFFSET, ADE791X_STATUS0_CRC_STAT_MASK);
    status->config_protected = BIT_VALUE_MASK(data, ADE791X_STATUS0_IC_PROT_OFFSET, ADE791X_STATUS0_IC_PROT_MASK);
    
    return E_NO_ERROR;
}

err_t ade791x_config_lock(ade791x_t* ade791x)
{
    uint8_t data = ADE791X_LOCK_KEY;
    err_t err = E_NO_ERROR;
    
    err = ade791x_write(ade791x, ADE791X_LOCK_ADDR, &data, ADE791X_LOCK_SIZE);
    if(err != E_NO_ERROR) return err;
    
    return ade791x_wait(ade791x);
}

err_t ade791x_config_unlock(ade791x_t* ade791x)
{
    uint8_t data = ADE791X_UNLOCK_KEY;
    err_t err = E_NO_ERROR;
    
    err = ade791x_write(ade791x, ADE791X_LOCK_ADDR, &data, ADE791X_LOCK_SIZE);
    if(err != E_NO_ERROR) return err;
    
    return ade791x_wait(ade791x);
}

err_t ade791x_read_sync_snap(ade791x_t* ade791x, ade791x_sync_snap_t* sync_snap)
{
    if(sync_snap == NULL) return E_NULL_POINTER;
    
    uint8_t data = 0;
    err_t err = E_NO_ERROR;
    
    err = ade791x_read(ade791x, ADE791X_SYNC_SNAP_ADDR, &data, ADE791X_SYNC_SNAP_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = ade791x_wait(ade791x);
    if(err != E_NO_ERROR) return err;
    
    sync_snap->sync = BIT_VALUE_MASK(data, ADE791X_SYNC_SNAP_SYNC_OFFSET, ADE791X_SYNC_SNAP_SYNC_MASK);
    sync_snap->snap = BIT_VALUE_MASK(data, ADE791X_SYNC_SNAP_SNAP_OFFSET, ADE791X_SYNC_SNAP_SNAP_MASK);
    
    return E_NO_ERROR;
}

err_t ade791x_write_sync_snap(ade791x_t* ade791x, const ade791x_sync_snap_t* sync_snap)
{
    if(sync_snap == NULL) return E_NULL_POINTER;
    
    uint8_t data = 0;
    err_t err = E_NO_ERROR;
    
    data |= sync_snap->sync << ADE791X_SYNC_SNAP_SYNC_OFFSET;
    data |= sync_snap->snap << ADE791X_SYNC_SNAP_SNAP_OFFSET;
    
    err = ade791x_write(ade791x, ADE791X_SYNC_SNAP_ADDR, &data, ADE791X_SYNC_SNAP_SIZE);
    if(err != E_NO_ERROR) return err;
    
    return ade791x_wait(ade791x);
}

err_t ade791x_read_emi_control(ade791x_t* ade791x, uint8_t* emi_control)
{
    if(emi_control == NULL) return E_NULL_POINTER;
    
    err_t err = E_NO_ERROR;
    
    err = ade791x_read(ade791x, ADE791X_EMI_CTRL_ADDR, emi_control, ADE791X_EMI_CTRL_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = ade791x_wait(ade791x);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t ade791x_write_emi_control(ade791x_t* ade791x, uint8_t emi_control)
{
    err_t err = E_NO_ERROR;
    
    err = ade791x_write(ade791x, ADE791X_EMI_CTRL_ADDR, &emi_control, ADE791X_EMI_CTRL_SIZE);
    if(err != E_NO_ERROR) return err;
    
    return ade791x_wait(ade791x);
}

err_t ade791x_read_status1(ade791x_t* ade791x, ade791x_status1_t* status)
{
    if(status == NULL) return E_NULL_POINTER;
    
    uint8_t data = 0;
    err_t err = E_NO_ERROR;
    
    err = ade791x_read(ade791x, ADE791X_STATUS1_ADDR, &data, ADE791X_STATUS1_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = ade791x_wait(ade791x);
    if(err != E_NO_ERROR) return err;
    
    status->version = BIT_VALUE_MASK(data, ADE791X_STATUS1_VERSION_OFFSET, ADE791X_STATUS1_VERSION_MASK);
    status->adc_not_accessed = BIT_VALUE_MASK(data, ADE791X_STATUS1_ADC_NA_OFFSET, ADE791X_STATUS1_ADC_NA_MASK);
    
    return E_NO_ERROR;
}

err_t ade791x_read_tempos(ade791x_t* ade791x, int8_t* tempos)
{
    return ade791x_read_value8(ade791x, ADE791X_TEMPOS_ADDR, (uint8_t*)tempos);
}

int32_t ade791x_expand(const uint8_t* value)
{
    if(value == NULL) return 0;
    
    uint32_t res;
    
    if(value[0] & 0x80) res = 0xff000000;
    else res = 0;
    
    res |= (uint32_t)value[0] << 16;
    res |= (uint32_t)value[1] << 8;
    res |= (uint32_t)value[2];
    
    return (int32_t)res;
}
