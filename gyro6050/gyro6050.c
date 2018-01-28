#include "gyro6050.h"
#include "utils/utils.h"
#include "counter/counter.h"
#include "cordic/cordic32.h"
#include <stddef.h>
#include <string.h>


//! Адрес делителя частоты вывода.
#define GYRO6050_SMPRT_DIV_ADDRESS      25

//! Адрес регистра конфигурации EXT_SYNC и DLPF.
#define GYRO6050_CONTROL_ADDRESS        26

//! Адрес регистра конфигурации гироскопа.
#define GYRO6050_GYRO_CONTROL_ADDRESS   27
//! Смещение значения диапазона данных гироскопа.
#define GYRO6050_GYRO_CONTROL_FS_SEL_OFFSET     3

//! Адрес регистра конфигурации акселерометра.
#define GYRO6050_ACCEL_CONTROL_ADDRESS  28
//! Смещение значения диапазона данных акселерометра.
#define GYRO6050_ACCEL_CONTROL_FS_SEL_OFFSET    3

//! Адрес регистра конфигурации пина сигнала о прерываниях.
#define GYRO6050_INT_PIN_CFG_ADDRESS    55

//! Адрес регистра конфигурации сигнала о прерываниях.
#define GYRO6050_INT_CFG_ADDRESS        56

//! Адрес начала данных
//! подряд:
//! Акселерометр -> температура -> гироскоп.
#define GYRO6050_ACCEL_TEMP_GYRO_DATA_ADDRESS      59
//! Адрес начала данных акселерометра.
#define GYRO6050_ACCEL_DATA_ADDRESS                59
//! Адрес начала данных температуры.
#define GYRO6050_TEMP_DATA_ADDRESS                 65
//! Адрес начала данных гироскопа.
#define GYRO6050_GYRO_DATA_ADDRESS                 67

//! Первый регистр управления питанием.
#define GYRO6050_PWR_MNGT_1_ADDRESS     107
//! Бит сна.
#define GYRO6050_PWR_MNGT_1_SLEEP_BIT   6

//! Второй регистр управления питанием.
#define GYRO6050_PWR_MNGT_2_ADDRESS     108


//! Максимальные значения.
#define GYRO6050_DLPF_MAX               GYRO6050_DLPF_A5HZ_G5HZ
#define GYRO6050_GYRO_SCALE_RANGE_MAX   GYRO6050_GYRO_SCALE_RANGE_2000_DPS
#define GYRO6050_ACCEL_SCALE_RANGE_MAX  GYRO6050_ACCEL_SCALE_RANGE_16G

/*
//! Направление обмена данными.
//! Чтение.
#define GYRO6050_READ   0
//! Запись.
#define GYRO6050_WRITE  1
//! Тип направления обмена данными.
typedef uint8_t gyro6050_io_direction_t;
*/

//! Константы.
// --

//! Состояния гироскопа (текущие действия).
//! Бездействие.
#define GYRO6050_STATE_IDLE                     0
//! Чтение делителя частоты.
#define GYRO6050_STATE_RATE_DIVISOR_READ        1
//! Запись делителя частоты.
#define GYRO6050_STATE_RATE_DIVISOR_WRITE       2
//! Чтение DLPF.
#define GYRO6050_STATE_DLPF_READ                3
//! Запись DLPF.
#define GYRO6050_STATE_DLPF_WRITE               4
//! Чтение диапазона данных гироскопа.
#define GYRO6050_STATE_GYRO_RANGE_READ          5
//! Запись диапазона данных гироскопа.
#define GYRO6050_STATE_GYRO_RANGE_WRITE         6
//! Чтение диапазона данных акселерометра.
#define GYRO6050_STATE_ACCEL_RANGE_READ         7
//! Запись диапазона данных акселерометра.
#define GYRO6050_STATE_ACCEL_RANGE_WRITE        8
//! Запись бита sleep.
#define GYRO6050_STATE_SLEEP_WRITE              9
//! Запись конфигурации пина сигнала о прерываниях.
#define GYRO6050_STATE_INT_PIN_CONF_WRITE       10
//! Чтение конфигурации пина сигнала о прерываниях.
#define GYRO6050_STATE_INT_PIN_CONF_READ        11
//! Запись конфигурации сигнала о прерываниях.
#define GYRO6050_STATE_INT_CONF_WRITE           12
//! Чтение конфигурации сигнала о прерываниях.
#define GYRO6050_STATE_INT_CONF_READ            13
//! Чтение данных акселерометра и гироскопа.
#define GYRO6050_STATE_DATA_READ                14
//! Чтение данных для калибровки гироскопа.
#define GYRO6050_STATE_CALIBRATION_READ         15



/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool gyro6050_wait_current_op(gyro6050_t* gyro)
{
    // Если шина занята не нами - возврат ошибки занятости.
    if(i2c_bus_busy(gyro->i2c) && (i2c_bus_transfer_id(gyro->i2c) != gyro->i2c_transfer_id)) return false;
    // Подождём выполнения предыдущей операции.
    gyro6050_wait(gyro);
    // Подождём шину i2c.
    i2c_bus_wait(gyro->i2c);
    return true;
}

static void gyro6050_start(gyro6050_t* gyro)
{
    // Запустим будущее.
    future_start(&gyro->future);
}

static void gyro6050_end(gyro6050_t* gyro, err_t err)
{
    //! Остановим будущее.
    future_finish(&gyro->future, int_to_pvoid(err));
}

bool gyro6050_i2c_callback(gyro6050_t* gyro)
{
    if(i2c_bus_transfer_id(gyro->i2c) != gyro->i2c_transfer_id) return false;
    
    if(i2c_bus_status(gyro->i2c) == I2C_STATUS_TRANSFERED){
        gyro6050_end(gyro, E_NO_ERROR);
    }else{
        gyro6050_end(gyro, E_IO_ERROR);
    }
    
    return true;
}

static err_t gyro6050_write_data(gyro6050_t* gyro, uint8_t page_address, const void* data, size_t data_size)
{
    // Установим идентификатор передачи.
    if(!i2c_bus_set_transfer_id(gyro->i2c, gyro->i2c_transfer_id)) return E_BUSY;
    
    gyro6050_start(gyro);
    
    gyro->rom_address = page_address;
    gyro->i2c_messages[GYRO6050_I2C_MESSAGE_DATA].direction = I2C_WRITE;
    gyro->i2c_messages[GYRO6050_I2C_MESSAGE_DATA].data = (void*)data;
    gyro->i2c_messages[GYRO6050_I2C_MESSAGE_DATA].data_size = data_size;
    
    err_t err = i2c_bus_transfer(gyro->i2c, gyro->i2c_messages, GYRO6050_I2C_MESSAGES_COUNT);
    
    if(err != E_NO_ERROR){
        gyro6050_end(gyro, err);
    }
    return err;
}

static err_t gyro6050_read_data(gyro6050_t* gyro, uint8_t page_address, void* data, size_t data_size)
{
    // Установим идентификатор передачи.
    if(!i2c_bus_set_transfer_id(gyro->i2c, gyro->i2c_transfer_id)) return E_BUSY;
    
    gyro6050_start(gyro);
    
    gyro->rom_address = page_address;
    gyro->i2c_messages[GYRO6050_I2C_MESSAGE_DATA].direction = I2C_READ;
    gyro->i2c_messages[GYRO6050_I2C_MESSAGE_DATA].data = data;
    gyro->i2c_messages[GYRO6050_I2C_MESSAGE_DATA].data_size = data_size;
    
    //err_t err = i2c_master_read_at(gyro->i2c_address, &gyro->rom_address, 1, data, data_size);
    
    err_t err = i2c_bus_transfer(gyro->i2c, gyro->i2c_messages, GYRO6050_I2C_MESSAGES_COUNT);
    
    if(err != E_NO_ERROR){
        gyro6050_end(gyro, err);
    }
    return err;
}

err_t gyro6050_init(gyro6050_t* gyro, i2c_bus_t* i2c, i2c_address_t address)
{
    if(i2c == NULL) return E_NULL_POINTER;
    
    gyro->i2c = i2c;
    gyro->i2c_transfer_id = GYRO6050_DEFAULT_I2C_TRANSFER_ID;
    gyro->rom_address = 0;
    
    i2c_message_init(&gyro->i2c_messages[GYRO6050_I2C_MESSAGE_ROM_ADDRESS], address, I2C_WRITE, &gyro->rom_address, 1);
    i2c_message_init(&gyro->i2c_messages[GYRO6050_I2C_MESSAGE_DATA], address, I2C_READ, &gyro->raw_data, sizeof(gyro6050_raw_data_t));
    
    future_init(&gyro->future);
    
    memset(&gyro->data, 0x0, sizeof(gyro6050_data_t));
    memset(&gyro->raw_data, 0x0, sizeof(gyro6050_raw_data_t));
    memset(&gyro->conf_data, 0x0, sizeof(gyro6050_conf_data_t));
    
    return E_NO_ERROR;
}

void gyro6050_reset(gyro6050_t* gyro)
{
    gyro6050_end(gyro, E_NO_ERROR);
}

void gyro6050_timeout(gyro6050_t* gyro)
{
    gyro6050_end(gyro, E_TIME_OUT);
}

bool gyro6050_done(const gyro6050_t* gyro)
{
    return future_done(&gyro->future);
}

err_t gyro6050_wait(const gyro6050_t* gyro)
{
    future_wait(&gyro->future);
    return gyro6050_error(gyro);
}

bool gyro6050_busy(const gyro6050_t* gyro)
{
    return future_running(&gyro->future);
}

err_t gyro6050_error(const gyro6050_t* gyro)
{
    return pvoid_to_int(err_t, future_result(&gyro->future));
}

i2c_transfer_id_t gyro6050_i2c_transfer_id(const gyro6050_t* gyro)
{
    return gyro->i2c_transfer_id;
}

void gyro6050_set_i2c_transfer_id(gyro6050_t* gyro, i2c_transfer_id_t transfer_id)
{
    gyro->i2c_transfer_id = transfer_id;
}

err_t gyro6050_read_rate_divisor(gyro6050_t* gyro, uint8_t* divisor)
{
    if(divisor == NULL) return E_NULL_POINTER;
    
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    err = gyro6050_read_data(gyro, GYRO6050_SMPRT_DIV_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gyro6050_wait(gyro);
    if(err != E_NO_ERROR) return err;
    
    *divisor = data;
    
    return E_NO_ERROR;
}

err_t gyro6050_set_rate_divisor(gyro6050_t* gyro, uint8_t divisor)
{
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    data = divisor;
    
    err = gyro6050_write_data(gyro, GYRO6050_SMPRT_DIV_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    return gyro6050_wait(gyro);
}

err_t gyro6050_read_dlpf(gyro6050_t* gyro, gyro6050_dlpf_t* dlpf)
{
    if(dlpf == NULL) return E_NULL_POINTER;
    
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    err = gyro6050_read_data(gyro, GYRO6050_CONTROL_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gyro6050_wait(gyro);
    if(err != E_NO_ERROR) return err;
    
    *dlpf = data & 0x7;
    
    return E_NO_ERROR;
}

err_t gyro6050_set_dlpf(gyro6050_t* gyro, gyro6050_dlpf_t dlpf)
{
    if(dlpf > GYRO6050_DLPF_MAX) return E_INVALID_VALUE;
    
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    data = dlpf;
    
    err = gyro6050_write_data(gyro, GYRO6050_CONTROL_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    return gyro6050_wait(gyro);
}

err_t gyro6050_read_gyro_scale_range(gyro6050_t* gyro, gyro6050_gyro_scale_range_t* range)
{
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    err = gyro6050_read_data(gyro, GYRO6050_GYRO_CONTROL_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gyro6050_wait(gyro);
    if(err != E_NO_ERROR) return err;
    
    gyro->conf_data.gyro_scale_range = (data >> GYRO6050_GYRO_CONTROL_FS_SEL_OFFSET) & 0x3;
    
    if(range) *range = gyro->conf_data.gyro_scale_range;
    
    return E_NO_ERROR;
}

err_t gyro6050_set_gyro_scale_range(gyro6050_t* gyro, gyro6050_gyro_scale_range_t range)
{
    if(range > GYRO6050_GYRO_SCALE_RANGE_MAX) return E_INVALID_VALUE;
    
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    data = range << GYRO6050_GYRO_CONTROL_FS_SEL_OFFSET;
    
    err = gyro6050_write_data(gyro, GYRO6050_GYRO_CONTROL_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gyro6050_wait(gyro);
    if(err != E_NO_ERROR) return err;
    
    gyro->conf_data.gyro_scale_range = range;
    
    return E_NO_ERROR;
}

err_t gyro6050_read_accel_scale_range(gyro6050_t* gyro, gyro6050_accel_scale_range_t* range)
{
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    err = gyro6050_read_data(gyro, GYRO6050_ACCEL_CONTROL_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gyro6050_wait(gyro);
    if(err != E_NO_ERROR) return err;
    
    gyro->conf_data.accel_scale_range = (data >> GYRO6050_ACCEL_CONTROL_FS_SEL_OFFSET) & 0x3;
    
    if(range) *range = gyro->conf_data.accel_scale_range;
    
    return E_NO_ERROR;
}

err_t gyro6050_set_accel_scale_range(gyro6050_t* gyro, gyro6050_accel_scale_range_t range)
{
    if(range > GYRO6050_ACCEL_SCALE_RANGE_MAX) return E_INVALID_VALUE;
    
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    data = range << GYRO6050_ACCEL_CONTROL_FS_SEL_OFFSET;
    
    err = gyro6050_write_data(gyro, GYRO6050_ACCEL_CONTROL_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gyro6050_wait(gyro);
    if(err != E_NO_ERROR) return err;
    
    gyro->conf_data.accel_scale_range = range;
    
    return E_NO_ERROR;
}

err_t gyro6050_int_pin_configure(gyro6050_t* gyro, gyro6050_int_pin_conf_t conf)
{
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    data = conf;
    
    err = gyro6050_write_data(gyro, GYRO6050_INT_PIN_CFG_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    return gyro6050_wait(gyro);
}

err_t gyro6050_read_int_pin_config(gyro6050_t* gyro, gyro6050_int_pin_conf_t* conf)
{
    if(conf == NULL) return E_NULL_POINTER;
    
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    err = gyro6050_read_data(gyro, GYRO6050_INT_PIN_CFG_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gyro6050_wait(gyro);
    if(err != E_NO_ERROR) return err;
    
    *conf = data;
    
    return E_NO_ERROR;
}

err_t gyro6050_int_configure(gyro6050_t* gyro, gyro6050_int_conf_t conf)
{
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    data = conf;
    
    err = gyro6050_write_data(gyro, GYRO6050_INT_CFG_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    return gyro6050_wait(gyro);
}

err_t gyro6050_read_int_config(gyro6050_t* gyro, gyro6050_int_conf_t* conf)
{
    if(conf == NULL) return E_NULL_POINTER;
    
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    err = gyro6050_read_data(gyro, GYRO6050_INT_CFG_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = gyro6050_wait(gyro);
    if(err != E_NO_ERROR) return err;
    
    *conf = data;
    
    return E_NO_ERROR;
}

err_t gyro6050_power_control(gyro6050_t* gyro, gyro6050_power_t power_flags, gyro6050_clock_t clock)
{
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    data = power_flags | clock;
    
    err = gyro6050_write_data(gyro, GYRO6050_PWR_MNGT_1_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    return gyro6050_wait(gyro);
}

err_t gyro6050_read(gyro6050_t* gyro)
{
    if(!gyro6050_wait_current_op(gyro)) return E_BUSY;
    
    return gyro6050_read_data(gyro, GYRO6050_ACCEL_TEMP_GYRO_DATA_ADDRESS, &gyro->raw_data, sizeof(gyro6050_raw_data_t));
}
/**
 * Получает значение сырых данных для угловой скорости в 1 градус / с.
 * @return Значение сырых данных для угловой скорости в 1 градус / с.
 */
static int32_t gyro6050_raw_value_1dps(gyro6050_t* gyro)
{
    switch(gyro->conf_data.gyro_scale_range){
        default:
        case GYRO6050_GYRO_SCALE_RANGE_250_DPS:
            return (131); // 32768 / 250
        case GYRO6050_GYRO_SCALE_RANGE_500_DPS:
            return (66); // 32768 / 500
        case GYRO6050_GYRO_SCALE_RANGE_1000_DPS:
            return (33); // 32768 / 1000
        case GYRO6050_GYRO_SCALE_RANGE_2000_DPS:
            return (16); // 32768 / 2000
    }
}

/**
 * Получает значение сырых данных для ускорения в 1 g.
 * @return Значение сырых данных для ускорения в 1 g.
 */
static int32_t gyro6050_raw_value_1g(gyro6050_t* gyro)
{
    switch(gyro->conf_data.accel_scale_range){
        default:
        case GYRO6050_ACCEL_SCALE_RANGE_2G:
            return (16384); // 32768 / 2
        case GYRO6050_ACCEL_SCALE_RANGE_4G:
            return (8192); // 32768 / 4
        case GYRO6050_ACCEL_SCALE_RANGE_8G:
            return (4096); // 32768 / 8
        case GYRO6050_ACCEL_SCALE_RANGE_16G:
            return (2048); // 32768 / 16
    }
}

void gyro6050_calculate(gyro6050_t* gyro)
{
    int16_t temp = __REV16(gyro->raw_data.temp);
    int16_t accel_x = __REV16(gyro->raw_data.accel_x);
    int16_t accel_y = __REV16(gyro->raw_data.accel_y);
    int16_t accel_z = __REV16(gyro->raw_data.accel_z);
    int16_t gyro_w_x = __REV16(gyro->raw_data.gyro_w_x);
    int16_t gyro_w_y = __REV16(gyro->raw_data.gyro_w_y);
    int16_t gyro_w_z = __REV16(gyro->raw_data.gyro_w_z);

    // Вычислим температуру.
    gyro->data.temp = fixed16_make_from_fract((int32_t)temp, 340) + fixed16_make_from_fract(3653, 100);
    
    // Единичные значения.
    // Единичное ускорение.
    int32_t raw_one_g = gyro6050_raw_value_1g(gyro);
    // Единичная угловая скорость.
    int32_t raw_one_dps = gyro6050_raw_value_1dps(gyro);
    
    // Вычислим данные акселерометра.
    // X.
    gyro->data.accel_x = fixed32_make_from_fract((int32_t)accel_x, raw_one_g);
    // Y.
    gyro->data.accel_y = fixed32_make_from_fract((int32_t)accel_y, raw_one_g);
    // Z.
    gyro->data.accel_z = fixed32_make_from_fract((int32_t)accel_z, raw_one_g);
    
    // Вычислим данные гироскопа.
    // X.
    gyro->data.gyro_w_x = fixed32_make_from_fract((int32_t)gyro_w_x, raw_one_dps);
    // Y.
    gyro->data.gyro_w_y = fixed32_make_from_fract((int32_t)gyro_w_y, raw_one_dps);
    // Z.
    gyro->data.gyro_w_z = fixed32_make_from_fract((int32_t)gyro_w_z, raw_one_dps);
}

err_t gyro6050_data_get(gyro6050_t* gyro, gyro6050_data_t* data)
{
    if(data == NULL) return E_NULL_POINTER;
    
    memcpy(data, &gyro->data, sizeof(gyro6050_data_t));
    
    return E_NO_ERROR;
}

fixed16_t gyro6050_temp(const gyro6050_t* gyro)
{
    return gyro->data.temp;
}

fixed32_t gyro6050_accel_x(const gyro6050_t* gyro)
{
    return gyro->data.accel_x;
}

fixed32_t gyro6050_accel_y(const gyro6050_t* gyro)
{
    return gyro->data.accel_y;
}

fixed32_t gyro6050_accel_z(const gyro6050_t* gyro)
{
    return gyro->data.accel_z;
}

fixed32_t gyro6050_gyro_w_x(const gyro6050_t* gyro)
{
    return gyro->data.gyro_w_x;
}

fixed32_t gyro6050_gyro_w_y(const gyro6050_t* gyro)
{
    return gyro->data.gyro_w_y;
}

fixed32_t gyro6050_gyro_w_z(const gyro6050_t* gyro)
{
    return gyro->data.gyro_w_z;
}
