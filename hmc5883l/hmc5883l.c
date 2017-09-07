#include "hmc5883l.h"
#include "utils/utils.h"
#include "counter/counter.h"
#include "cordic/cordic32.h"
#include "bits/bits.h"
#include <stddef.h>
#include <string.h>


//! Адрес регистра частоты / режима измерений.
#define HMC5883L_REG_CRA_ADDRESS        0

//! Адрес регистра разрешения.
#define HMC5883L_REG_CRB_ADDRESS        1

//! Адрес режима.
#define HMC5883L_REG_MODE_ADDRESS       2

//! Адрес начала данных.
#define HMC5883L_COMPASS_DATA_ADDRESS   3


//! Максимальные значения.
#define HMC5883L_SAMPLES_MAX            HMC5883L_SAMPLES_8
#define HMC5883L_OUTPUT_RATE_MAX        HMC5883L_OUTPUT_RATE_75_HZ
#define HMC5883L_MEASUREMENT_MODE_MAX   HMC5883L_MEASUREMENT_MODE_NEGATIVE_BIAS
#define HMC5883L_RESOULUTION_MAX        HMC5883L_RESOULUTION_4_35_mG_PER_LSB
#define HMC5883L_OPERATING_MODE_MAX     HMC5883L_OPERATING_MODE_IDLE

//! Битовые смещения и размеры.
//! Число выборок.
#define HMC5883L_CRA_SAMPLES_OFFSET     5
#define HMC5883L_CRA_SAMPLES_SIZE       2
#define HMC5883L_CRA_SAMPLES_MASK       0x3
//! Частота измерений.
#define HMC5883L_CRA_OUTPUT_RATE_OFFSET 2
#define HMC5883L_CRA_OUTPUT_RATE_SIZE   3
#define HMC5883L_CRA_OUTPUT_RATE_MASK   0x7
//! Режим измерений.
#define HMC5883L_CRA_MEASUREMENT_MODE_OFFSET 0
#define HMC5883L_CRA_MEASUREMENT_MODE_SIZE   2
#define HMC5883L_CRA_MEASUREMENT_MODE_MASK   0x3
//! Разрешение.
#define HMC5883L_CRB_RESOLUTION_OFFSET     5
#define HMC5883L_CRB_RESOLUTION_SIZE       3
#define HMC5883L_CRB_RESOLUTION_MASK       0x7
//! Частота измерений.
#define HMC5883L_MODE_FAST_I2C_OFFSET 7
#define HMC5883L_MODE_FAST_I2C_SIZE   1
#define HMC5883L_MODE_FAST_I2C_MASK   0x1
//! Режим измерений.
#define HMC5883L_MODE_OPERATING_MODE_OFFSET 0
#define HMC5883L_MODE_OPERATING_MODE_SIZE   2
#define HMC5883L_MODE_OPERATING_MODE_MASK   0x3




/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool hmc5883l_wait_current_op(hmc5883l_t* compass)
{
    // Если шина занята не нами - возврат ошибки занятости.
    if(i2c_bus_busy(compass->i2c) && (i2c_bus_transfer_id(compass->i2c) != compass->i2c_transfer_id)) return false;
    // Подождём выполнения предыдущей операции.
    hmc5883l_wait(compass);
    // Подождём шину i2c.
    i2c_bus_wait(compass->i2c);
    return true;
}

static void hmc5883l_start(hmc5883l_t* compass)
{
    // Запустим будущее.
    future_start(&compass->future);
}

static void hmc5883l_end(hmc5883l_t* compass, err_t err)
{
    //! Остановим будущее.
    future_finish(&compass->future, int_to_pvoid(err));
}

bool hmc5883l_i2c_callback(hmc5883l_t* compass)
{
    if(i2c_bus_transfer_id(compass->i2c) != compass->i2c_transfer_id) return false;
    
    if(i2c_bus_status(compass->i2c) == I2C_STATUS_TRANSFERED){
        hmc5883l_end(compass, E_NO_ERROR);
    }else{
        hmc5883l_end(compass, E_IO_ERROR);
    }
    
    return true;
}

static err_t hmc5883l_write_data(hmc5883l_t* compass, uint8_t page_address, const void* data, size_t data_size)
{
    // Установим идентификатор передачи.
    if(!i2c_bus_set_transfer_id(compass->i2c, compass->i2c_transfer_id)) return E_BUSY;
    
    hmc5883l_start(compass);
    
    compass->rom_address = page_address;
    compass->i2c_messages[HMC5883L_I2C_MESSAGE_DATA].direction = I2C_WRITE;
    compass->i2c_messages[HMC5883L_I2C_MESSAGE_DATA].data = (void*)data;
    compass->i2c_messages[HMC5883L_I2C_MESSAGE_DATA].data_size = data_size;
    
    err_t err = i2c_bus_transfer(compass->i2c, compass->i2c_messages, HMC5883L_I2C_MESSAGES_COUNT);
    
    if(err != E_NO_ERROR){
        hmc5883l_end(compass, err);
    }
    return err;
}

static err_t hmc5883l_read_data(hmc5883l_t* compass, uint8_t page_address, void* data, size_t data_size)
{
    // Установим идентификатор передачи.
    if(!i2c_bus_set_transfer_id(compass->i2c, compass->i2c_transfer_id)) return E_BUSY;
    
    hmc5883l_start(compass);
    
    compass->rom_address = page_address;
    compass->i2c_messages[HMC5883L_I2C_MESSAGE_DATA].direction = I2C_READ;
    compass->i2c_messages[HMC5883L_I2C_MESSAGE_DATA].data = data;
    compass->i2c_messages[HMC5883L_I2C_MESSAGE_DATA].data_size = data_size;
    
    err_t err = i2c_bus_transfer(compass->i2c, compass->i2c_messages, HMC5883L_I2C_MESSAGES_COUNT);
    
    if(err != E_NO_ERROR){
        hmc5883l_end(compass, err);
    }
    return err;
}

err_t hmc5883l_init(hmc5883l_t* compass, i2c_bus_t* i2c, i2c_address_t address)
{
    if(i2c == NULL) return E_NULL_POINTER;
    
    compass->i2c = i2c;
    compass->i2c_transfer_id = HMC5883L_DEFAULT_I2C_TRANSFER_ID;
    compass->rom_address = 0;
    
    i2c_message_init(&compass->i2c_messages[HMC5883L_I2C_MESSAGE_ROM_ADDRESS], address, I2C_WRITE, &compass->rom_address, 1);
    i2c_message_init(&compass->i2c_messages[HMC5883L_I2C_MESSAGE_DATA], address, I2C_READ, &compass->raw_data, sizeof(hmc5883l_raw_data_t));
    
    future_init(&compass->future);
    
    memset(&compass->data, 0x0, sizeof(hmc5883l_data_t));
    memset(&compass->raw_data, 0x0, sizeof(hmc5883l_raw_data_t));
    memset(&compass->conf_data, 0x0, sizeof(hmc5883l_conf_data_t));
    
    return E_NO_ERROR;
}

void hmc5883l_reset(hmc5883l_t* compass)
{
    hmc5883l_end(compass, E_NO_ERROR);
}

void hmc5883l_timeout(hmc5883l_t* compass)
{
    hmc5883l_end(compass, E_TIME_OUT);
}

bool hmc5883l_done(const hmc5883l_t* compass)
{
    return future_done(&compass->future);
}

err_t hmc5883l_wait(const hmc5883l_t* compass)
{
    future_wait(&compass->future);
    return hmc5883l_error(compass);
}

bool hmc5883l_busy(const hmc5883l_t* compass)
{
    return future_running(&compass->future);
}

err_t hmc5883l_error(const hmc5883l_t* compass)
{
    return pvoid_to_int(err_t, future_result(&compass->future));
}

i2c_transfer_id_t hmc5883l_i2c_transfer_id(const hmc5883l_t* compass)
{
    return compass->i2c_transfer_id;
}

void hmc5883l_set_i2c_transfer_id(hmc5883l_t* compass, i2c_transfer_id_t transfer_id)
{
    compass->i2c_transfer_id = transfer_id;
}

err_t hmc5883l_read_configuration(hmc5883l_t* compass, hmc5883l_samples_t* samples,
                                  hmc5883l_output_rate_t* output_rate, hmc5883l_measurement_mode_t* measurement_mode)
{
    if(!hmc5883l_wait_current_op(compass)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    err = hmc5883l_read_data(compass, HMC5883L_REG_CRA_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = hmc5883l_wait(compass);
    if(err != E_NO_ERROR) return err;
    
    if(samples) *samples = (data >> HMC5883L_CRA_SAMPLES_OFFSET) & HMC5883L_CRA_SAMPLES_MASK;
    if(output_rate) *output_rate = (data >> HMC5883L_CRA_OUTPUT_RATE_OFFSET) & HMC5883L_CRA_OUTPUT_RATE_MASK;
    if(measurement_mode) *measurement_mode = (data >> HMC5883L_CRA_MEASUREMENT_MODE_OFFSET) & HMC5883L_CRA_MEASUREMENT_MODE_MASK;
    
    return E_NO_ERROR;
}

err_t hmc5883l_configure(hmc5883l_t* compass, hmc5883l_samples_t samples,
                         hmc5883l_output_rate_t output_rate, hmc5883l_measurement_mode_t measurement_mode)
{
    if(samples > HMC5883L_SAMPLES_MAX) return E_INVALID_VALUE;
    if(output_rate > HMC5883L_OUTPUT_RATE_MAX) return E_INVALID_VALUE;
    if(measurement_mode > HMC5883L_MEASUREMENT_MODE_MAX) return E_INVALID_VALUE;
    
    if(!hmc5883l_wait_current_op(compass)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    data = (samples << HMC5883L_CRA_SAMPLES_OFFSET) |
           (output_rate << HMC5883L_CRA_OUTPUT_RATE_OFFSET) |
           (measurement_mode << HMC5883L_CRA_MEASUREMENT_MODE_OFFSET);
    
    err = hmc5883l_write_data(compass, HMC5883L_REG_CRA_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    return hmc5883l_wait(compass);
}

err_t hmc5883l_read_resolution(hmc5883l_t* compass, hmc5883l_resolution_t* resolution)
{
    if(resolution == NULL) return E_NULL_POINTER;
    
    if(!hmc5883l_wait_current_op(compass)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    err = hmc5883l_read_data(compass, HMC5883L_REG_CRB_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = hmc5883l_wait(compass);
    if(err != E_NO_ERROR) return err;
    
    *resolution = (data >> HMC5883L_CRB_RESOLUTION_OFFSET) & HMC5883L_CRB_RESOLUTION_MASK;
    
    return E_NO_ERROR;
}

err_t hmc5883l_set_resolution(hmc5883l_t* compass, hmc5883l_resolution_t resolution)
{
    if(resolution > HMC5883L_RESOULUTION_MAX) return E_INVALID_VALUE;
    
    if(!hmc5883l_wait_current_op(compass)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    data = resolution << HMC5883L_CRB_RESOLUTION_OFFSET;
    
    err = hmc5883l_write_data(compass, HMC5883L_REG_CRB_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    return hmc5883l_wait(compass);
}

err_t hmc5883l_read_mode(hmc5883l_t* compass, hmc5883l_operating_mode_t* op_mode, uint8_t* mode_flags)
{
    if(!hmc5883l_wait_current_op(compass)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    err = hmc5883l_read_data(compass, HMC5883L_REG_MODE_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    err = hmc5883l_wait(compass);
    if(err != E_NO_ERROR) return err;
    
    if(mode_flags) *mode_flags = data & HMC5883L_CRB_RESOLUTION_MASK;
    if(op_mode) *op_mode = (data >> HMC5883L_MODE_OPERATING_MODE_OFFSET) & HMC5883L_MODE_OPERATING_MODE_MASK;
    
    return E_NO_ERROR;
}

err_t hmc5883l_set_mode(hmc5883l_t* compass, hmc5883l_operating_mode_t op_mode, uint8_t mode_flags)
{
    if(op_mode > HMC5883L_OPERATING_MODE_MAX) return E_INVALID_VALUE;
    
    if(!hmc5883l_wait_current_op(compass)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    uint8_t data = 0;
    
    data = (op_mode << HMC5883L_MODE_OPERATING_MODE_OFFSET) | mode_flags;
    
    err = hmc5883l_write_data(compass, HMC5883L_REG_MODE_ADDRESS, &data, 1);
    if(err != E_NO_ERROR) return err;
    
    return hmc5883l_wait(compass);
}

err_t hmc5883l_read(hmc5883l_t* compass)
{
    if(!hmc5883l_wait_current_op(compass)) return E_BUSY;
    
    return hmc5883l_read_data(compass, HMC5883L_COMPASS_DATA_ADDRESS, &compass->raw_data, sizeof(hmc5883l_raw_data_t));
}

/**
 * Получает значение сырых данных для напряжённости магнитного поля в 1 Гаусс.
 * @return Значение сырых данных для напряжённости магнитного поля в 1 Гаусс.
 */
static int32_t hmc5883l_raw_value_1gauss(hmc5883l_t* compass)
{
    switch(compass->conf_data.resolution){
        default:
        case HMC5883L_RESOULUTION_0_73_mG_PER_LSB:
            return 1370;
        case HMC5883L_RESOULUTION_0_92_mG_PER_LSB:
            return 1090;
        case HMC5883L_RESOULUTION_1_22_mG_PER_LSB:
            return 820;
        case HMC5883L_RESOULUTION_1_52_mG_PER_LSB:
            return 660;
        case HMC5883L_RESOULUTION_2_27_mG_PER_LSB:
            return 440;
        case HMC5883L_RESOULUTION_2_56_mG_PER_LSB:
            return 390;
        case HMC5883L_RESOULUTION_3_03_mG_PER_LSB:
            return 330;
        case HMC5883L_RESOULUTION_4_35_mG_PER_LSB:
            return 230;
    }
}

void hmc5883l_calculate(hmc5883l_t* compass)
{
    int16_t compass_x = __REV16(compass->raw_data.compass_x);
    int16_t compass_y = __REV16(compass->raw_data.compass_y);
    int16_t compass_z = __REV16(compass->raw_data.compass_z);

    // Единичное значение.
    int32_t raw_one_gauss = hmc5883l_raw_value_1gauss(compass);

    // Вычислим данные компаса.
    // X.
    compass->data.compass_x = fixed32_make_from_fract((int32_t)compass_x, raw_one_gauss);
    // Y.
    compass->data.compass_y = fixed32_make_from_fract((int32_t)compass_y, raw_one_gauss);
    // Z.
    compass->data.compass_z = fixed32_make_from_fract((int32_t)compass_z, raw_one_gauss);
}

fixed32_t hmc5883l_compass_x(const hmc5883l_t* compass)
{
    return compass->data.compass_x;
}

fixed32_t hmc5883l_compass_y(const hmc5883l_t* compass)
{
    return compass->data.compass_y;
}

fixed32_t hmc5883l_compass_z(const hmc5883l_t* compass)
{
    return compass->data.compass_z;
}
