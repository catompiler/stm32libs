#include "lm75.h"
#include <stddef.h>
#include "utils/utils.h"
#include "utils/net.h"
#include "bits/bits.h"
#include "defs/defs.h"
#include "stm32f10x.h"


//! Номер сообщения с адресом памяти.
#define LM75_I2C_MESSAGE_ROM_ADDRESS 0
//! Номер сообщения с данными.
#define LM75_I2C_MESSAGE_DATA        1

// Регистры.
// Температура.
#define LM75_REG_TEMP_ADDR     0
#define LM75_REG_TEMP_SIZE     2
// Конфигурация.
#define LM75_REG_CONF_ADDR     1
#define LM75_REG_CONF_SIZE     1
// Гистерезис.
#define LM75_REG_THYST_ADDR     2
#define LM75_REG_THYST_SIZE     2
// Температура сравнения.
#define LM75_REG_TOS_ADDR     3
#define LM75_REG_TOS_SIZE     2

// Битовые поля.
// Очередь ошибок.
#define LM75_CONF_FAULT_QUEUE_OFFSET 0x3
#define LM75_CONF_FAULT_QUEUE_MASK 0x3
// Полярность пина OS.
#define LM75_CONF_OS_POLARITY_OFFSET 0x2
#define LM75_CONF_OS_POLARITY_MASK 0x1
// Режим пина OS.
#define LM75_CONF_OS_MODE_OFFSET 0x1
#define LM75_CONF_OS_MODE_MASK 0x1
// Останов.
#define LM75_CONF_SHUTDOWN_OFFSET 0x0
#define LM75_CONF_SHUTDOWN_MASK 0x1


/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool lm75_wait_current_op(lm75_t* sensor)
{
    // Если шина занята.
    if(i2c_bus_busy(sensor->i2c)){
        // Если не нами - возврат ошибки занятости.
        if(i2c_bus_transfer_id(sensor->i2c) != sensor->transfer_id) return false;
        // Подождём выполнения предыдущей операции.
        future_wait(&sensor->future);
        // Подождём шину i2c.
        i2c_bus_wait(sensor->i2c);
    } // Иначе если будущее выполняется при не занятой шине - ошибка.
    else if(future_running(&sensor->future)){
        // Завершим будущее.
        future_finish(&sensor->future, int_to_pvoid(E_IO_ERROR));
    }
    return true;
}

ALWAYS_INLINE static void lm75_start(lm75_t* sensor)
{
    future_start(&sensor->future);
}

ALWAYS_INLINE static void lm75_end(lm75_t* sensor, err_t err)
{
    future_finish(&sensor->future, int_to_pvoid(err));
}

bool lm75_i2c_callback(lm75_t* sensor)
{
    if(i2c_bus_transfer_id(sensor->i2c) != sensor->transfer_id) return false;
    
    lm75_end(sensor, i2c_bus_status(sensor->i2c) != I2C_STATUS_TRANSFERED ? E_IO_ERROR : E_NO_ERROR);
    
    return true;
}

err_t lm75_transfer(lm75_t* sensor, uint8_t rom_address, i2c_direction_t data_direction, void* data, size_t data_size)
{
	// Установим идентификатор передачи.
    if(!i2c_bus_set_transfer_id(sensor->i2c, sensor->transfer_id)) return E_BUSY;
    
    lm75_start(sensor);
    
    sensor->rom_address = rom_address;
    sensor->i2c_messages[LM75_I2C_MESSAGE_DATA].direction = data_direction;
    sensor->i2c_messages[LM75_I2C_MESSAGE_DATA].data = data;
    sensor->i2c_messages[LM75_I2C_MESSAGE_DATA].data_size = data_size;
    
    err_t err = i2c_bus_transfer(sensor->i2c, sensor->i2c_messages, LM75_I2C_MESSAGES_COUNT);
    
    if(err != E_NO_ERROR){
        lm75_end(sensor, err);
    }
    return err;
}

err_t lm75_init(lm75_t* sensor, i2c_bus_t* i2c, i2c_address_t address)
{
    if(i2c == NULL) return E_NULL_POINTER;
    
    sensor->i2c = i2c;
	
    sensor->transfer_id = LM75_I2C_DEFAULT_TRANSFER_ID;
    
    sensor->rom_address = 0;
	
    i2c_message_setup(&sensor->i2c_messages[LM75_I2C_MESSAGE_ROM_ADDRESS], address, I2C_WRITE, &sensor->rom_address, 1);
    i2c_message_setup(&sensor->i2c_messages[LM75_I2C_MESSAGE_DATA], address, I2C_READ, NULL, 0);
	
    future_init(&sensor->future);
    future_set_result(&sensor->future, int_to_pvoid(E_NO_ERROR));
    
    return E_NO_ERROR;
}

i2c_transfer_id_t lm75_i2c_transfer_id(const lm75_t* sensor)
{
    return sensor->transfer_id;
}

void lm75_set_i2c_transfer_id(lm75_t* sensor, i2c_transfer_id_t transfer_id)
{
    sensor->transfer_id = transfer_id;
}

i2c_address_t lm75_i2c_address(const lm75_t* sensor)
{
    return sensor->i2c_messages[LM75_I2C_MESSAGE_ROM_ADDRESS].address;
}

void lm75_set_i2c_address(lm75_t* sensor, i2c_address_t address)
{
    sensor->i2c_messages[LM75_I2C_MESSAGE_ROM_ADDRESS].address = address;
    sensor->i2c_messages[LM75_I2C_MESSAGE_DATA].address = address;
}

i2c_bus_t* lm75_i2c_bus(const lm75_t* sensor)
{
    return sensor->i2c;
}

void lm75_set_i2c_bus(lm75_t* sensor, i2c_bus_t* i2c)
{
    sensor->i2c = i2c;
}

void lm75_reset(lm75_t* sensor)
{
    lm75_end(sensor, E_NO_ERROR);
}

void lm75_timeout(lm75_t* sensor)
{
    lm75_end(sensor, E_TIME_OUT);
}

bool lm75_busy(lm75_t* sensor)
{
    return future_running(&sensor->future);
}

bool lm75_done(lm75_t* sensor)
{
    return future_done(&sensor->future);
}

err_t lm75_error(lm75_t* sensor)
{
    return pvoid_to_int(err_t, future_result(&sensor->future));
}

err_t lm75_wait(lm75_t* sensor)
{
    future_wait(&sensor->future);
	
    return lm75_error(sensor);
}

err_t lm75_read_temp(lm75_t* sensor, fixed16_t* temp)
{
    if(temp == NULL) return E_NULL_POINTER;
    if(!lm75_wait_current_op(sensor)) return E_BUSY;
    
    uint16_t data = 0;
    
    err_t err = E_NO_ERROR;
    
    err = lm75_transfer(sensor, LM75_REG_TEMP_ADDR, I2C_READ, &data, LM75_REG_TEMP_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = lm75_wait(sensor);
    if(err != E_NO_ERROR) return err;
    
    *temp = ntohs(data);
    
    return E_NO_ERROR;
}

err_t lm75_read_conf(lm75_t* sensor, lm75_conf_t* conf)
{
    if(conf == NULL) return E_NULL_POINTER;
    if(!lm75_wait_current_op(sensor)) return E_BUSY;
    
    uint8_t data = 0;
    
    err_t err = E_NO_ERROR;
    
    err = lm75_transfer(sensor, LM75_REG_CONF_ADDR, I2C_READ, &data, LM75_REG_CONF_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = lm75_wait(sensor);
    if(err != E_NO_ERROR) return err;
    
    conf->fault_queue = BIT_VALUE_MASK(data, LM75_CONF_FAULT_QUEUE_OFFSET, LM75_CONF_FAULT_QUEUE_MASK);
    conf->os_polarity = BIT_VALUE_MASK(data, LM75_CONF_OS_POLARITY_OFFSET, LM75_CONF_OS_POLARITY_MASK);
    conf->os_mode = BIT_VALUE_MASK(data, LM75_CONF_OS_MODE_OFFSET, LM75_CONF_OS_MODE_MASK);
    conf->shutdown = BIT_VALUE_MASK(data, LM75_CONF_SHUTDOWN_OFFSET, LM75_CONF_SHUTDOWN_MASK);
    
    return E_NO_ERROR;
}

err_t lm75_write_conf(lm75_t* sensor, const lm75_conf_t* conf)
{
    if(conf == NULL) return E_NULL_POINTER;
    if(!lm75_wait_current_op(sensor)) return E_BUSY;
    
    uint8_t data = 0;
    
    // Да здравствует ARM!
    // orr r3, r3, r2, lsl #N
    data |= conf->fault_queue << LM75_CONF_FAULT_QUEUE_OFFSET;
    data |= conf->os_polarity << LM75_CONF_OS_POLARITY_OFFSET;
    data |= conf->os_mode << LM75_CONF_OS_MODE_OFFSET;
    data |= conf->shutdown << LM75_CONF_SHUTDOWN_OFFSET;
    
    err_t err = E_NO_ERROR;
    
    err = lm75_transfer(sensor, LM75_REG_CONF_ADDR, I2C_WRITE, &data, LM75_REG_CONF_SIZE);
    if(err != E_NO_ERROR) return err;
    
    return lm75_wait(sensor);
}

err_t lm75_read_temp_hyst(lm75_t* sensor, fixed16_t* temp)
{
    if(temp == NULL) return E_NULL_POINTER;
    if(!lm75_wait_current_op(sensor)) return E_BUSY;
    
    uint16_t data = 0;
    
    err_t err = E_NO_ERROR;
    
    err = lm75_transfer(sensor, LM75_REG_THYST_ADDR, I2C_READ, &data, LM75_REG_THYST_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = lm75_wait(sensor);
    if(err != E_NO_ERROR) return err;
    
    *temp = ntohs(data);
    
    return E_NO_ERROR;
}

err_t lm75_write_temp_hyst(lm75_t* sensor, fixed16_t temp)
{
    if(!lm75_wait_current_op(sensor)) return E_BUSY;
    
    uint16_t data = htons(temp);
    
    err_t err = E_NO_ERROR;
    
    err = lm75_transfer(sensor, LM75_REG_THYST_ADDR, I2C_WRITE, &data, LM75_REG_THYST_SIZE);
    if(err != E_NO_ERROR) return err;
    
    return lm75_wait(sensor);
}

err_t lm75_read_temp_os(lm75_t* sensor, fixed16_t* temp)
{
    if(temp == NULL) return E_NULL_POINTER;
    if(!lm75_wait_current_op(sensor)) return E_BUSY;
    
    uint16_t data = 0;
    
    err_t err = E_NO_ERROR;
    
    err = lm75_transfer(sensor, LM75_REG_TOS_ADDR, I2C_READ, &data, LM75_REG_TOS_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = lm75_wait(sensor);
    if(err != E_NO_ERROR) return err;
    
    *temp = ntohs(data);
    
    return E_NO_ERROR;
}

err_t lm75_write_temp_os(lm75_t* sensor, fixed16_t temp)
{
    if(!lm75_wait_current_op(sensor)) return E_BUSY;
    
    uint16_t data = htons(temp);
    
    err_t err = E_NO_ERROR;
    
    err = lm75_transfer(sensor, LM75_REG_TOS_ADDR, I2C_WRITE, &data, LM75_REG_TOS_SIZE);
    if(err != E_NO_ERROR) return err;
    
    return lm75_wait(sensor);
}
