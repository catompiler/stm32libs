#include "ds1307.h"
#include <string.h>
#include <stddef.h>
#include "utils/utils.h"
#include "bits/bits.h"



/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool ds1307_wait_current_op(ds1307_t* rtc)
{
    // Если шина занята не нами - возврат ошибки занятости.
    if(i2c_bus_busy(rtc->i2c) && (i2c_bus_transfer_id(rtc->i2c) != rtc->i2c_transfer_id)) return false;
    // Подождём выполнения предыдущей операции.
    ds1307_wait(rtc);
    // Подождём шину i2c.
    i2c_bus_wait(rtc->i2c);
    return true;
}

static void ds1307_start(ds1307_t* rtc)
{
    future_start(&rtc->future);
}

static void ds1307_end(ds1307_t* rtc, err_t err)
{
    future_finish(&rtc->future, int_to_pvoid(err));
}

bool ds1307_i2c_callback(ds1307_t* rtc)
{
    if(i2c_bus_transfer_id(rtc->i2c) != rtc->i2c_transfer_id) return false;
    
    if(i2c_bus_status(rtc->i2c) == I2C_STATUS_TRANSFERED){
        ds1307_end(rtc, E_NO_ERROR);
    }else{
        ds1307_end(rtc, E_IO_ERROR);
    }
    
    return true;
}

static err_t ds1307_write_data(ds1307_t* rtc, uint8_t page_address, const void* data, size_t data_size)
{
    // Установим идентификатор передачи.
    if(!i2c_bus_set_transfer_id(rtc->i2c, rtc->i2c_transfer_id)) return E_BUSY;
    
    ds1307_start(rtc);
    
    rtc->rom_address = page_address;
    rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].direction = I2C_WRITE;
    rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].data = (void*)data;
    rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].data_size = data_size;
    
    err_t err = i2c_bus_transfer(rtc->i2c, rtc->i2c_messages, DS1307_I2C_MESSAGES_COUNT);
    
    if(err != E_NO_ERROR){
        ds1307_end(rtc, err);
    }
    return err;
}

static err_t ds1307_read_data(ds1307_t* rtc, uint8_t page_address, void* data, size_t data_size)
{
    // Установим идентификатор передачи.
    if(!i2c_bus_set_transfer_id(rtc->i2c, rtc->i2c_transfer_id)) return E_BUSY;
    
    ds1307_start(rtc);
    
    rtc->rom_address = page_address;
    rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].direction = I2C_READ;
    rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].data = data;
    rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].data_size = data_size;
    
    err_t err = i2c_bus_transfer(rtc->i2c, rtc->i2c_messages, DS1307_I2C_MESSAGES_COUNT);
    
    if(err != E_NO_ERROR){
        ds1307_end(rtc, err);
    }
    return err;
}

err_t ds1307_init(ds1307_t* rtc, i2c_bus_t* i2c)
{
    if(i2c == NULL) return E_NULL_POINTER;
    
    memset(&rtc->memory, 0x0, sizeof(ds1307mem_t));
    
    rtc->i2c_transfer_id = DS1307_DEFAULT_I2C_TRANSFER_ID;
    
    rtc->i2c = i2c;
    
    rtc->rom_address = 0;
    
    i2c_message_init(&rtc->i2c_messages[DS1307_I2C_MESSAGE_ROM_ADDRESS], DS1307_I2C_ADDRESS, I2C_WRITE, &rtc->rom_address, 1);
    i2c_message_init(&rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA], DS1307_I2C_ADDRESS, I2C_READ, &rtc->memory, sizeof(ds1307mem_t));
    
    future_init(&rtc->future);
    future_set_result(&rtc->future, int_to_pvoid(E_NO_ERROR));
    
    return E_NO_ERROR;
}

bool ds1307_in_process(ds1307_t* rtc)
{
    return future_running(&rtc->future);
}

void ds1307_reset(ds1307_t* rtc)
{
    ds1307_end(rtc, E_NO_ERROR);
}

void ds1307_timeout(ds1307_t* rtc)
{
    ds1307_end(rtc, E_TIME_OUT);
}

bool ds1307_done(ds1307_t* rtc)
{
    return future_done(&rtc->future);
}

err_t ds1307_wait(ds1307_t* rtc)
{
    future_wait(&rtc->future);
    return ds1307_error(rtc);
}

err_t ds1307_error(ds1307_t* rtc)
{
    return pvoid_to_int(err_t, future_result(&rtc->future));
}

i2c_transfer_id_t ds1307_i2c_transfer_id(const ds1307_t* rtc)
{
    return rtc->i2c_transfer_id;
}

void ds1307_set_i2c_transfer_id(ds1307_t* rtc, i2c_transfer_id_t transfer_id)
{
    rtc->i2c_transfer_id = transfer_id;
}

err_t ds1307_read(ds1307_t* rtc)
{
    if(!ds1307_wait_current_op(rtc)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    err = ds1307_read_data(rtc, 0, &rtc->memory, sizeof(ds1307mem_t));
    
    return err;
}

err_t ds1307_write(ds1307_t* rtc)
{
    if(!ds1307_wait_current_op(rtc)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    err = ds1307_write_data(rtc, 0, &rtc->memory, sizeof(ds1307mem_t));
    
    return err;
}

void ds1307_datetime_get(ds1307_t* rtc, ds1307_datetime_t* datetime)
{
    datetime->seconds = rtc->memory.seconds_byte.seconds10 * 10 + rtc->memory.seconds_byte.seconds;
    datetime->minutes = rtc->memory.minutes_byte.minutes10 * 10 + rtc->memory.minutes_byte.minutes;
    if(rtc->memory.hours_byte.ampm){
        datetime->is_ampm = true;
        datetime->pm = BIT_VALUE(rtc->memory.hours_byte.hours10, 1);
        datetime->hours   = (rtc->memory.hours_byte.hours10 & 0x1) * 10 + rtc->memory.hours_byte.hours;
    }else{
        datetime->is_ampm = false;
        datetime->pm = false;
        datetime->hours   = rtc->memory.hours_byte.hours10 * 10 + rtc->memory.hours_byte.hours;
    }
    datetime->day = rtc->memory.day_byte.day;
    datetime->date = rtc->memory.date_byte.date10 * 10 + rtc->memory.date_byte.date;
    datetime->month = rtc->memory.month_byte.month10 * 10 + rtc->memory.month_byte.month;
    datetime->year = rtc->memory.year_byte.year10 * 10 + rtc->memory.year_byte.year;
}

void ds1307_datetime_set(ds1307_t* rtc, ds1307_datetime_t* datetime)
{
    rtc->memory.seconds_byte.seconds10 = datetime->seconds / 10;
    rtc->memory.seconds_byte.seconds   = datetime->seconds % 10;
    
    rtc->memory.minutes_byte.minutes10 = datetime->minutes / 10;
    rtc->memory.minutes_byte.minutes   = datetime->minutes % 10;
    
    rtc->memory.hours_byte.ampm = datetime->is_ampm;
    if(datetime->is_ampm){
        rtc->memory.hours_byte.hours10 = datetime->hours & 0x1;
        if(datetime->pm) BIT_ON(rtc->memory.hours_byte.hours10, 1);
    }else{
        rtc->memory.hours_byte.hours10 = datetime->hours / 10;
    }
    rtc->memory.hours_byte.hours = datetime->hours % 10;
    
    rtc->memory.day_byte.day = datetime->day;
    
    rtc->memory.date_byte.date10 = datetime->date / 10;
    rtc->memory.date_byte.date   = datetime->date % 10;
    
    rtc->memory.month_byte.month10 = datetime->month / 10;
    rtc->memory.month_byte.month   = datetime->month % 10;
    
    rtc->memory.year_byte.year10 = datetime->year / 10;
    rtc->memory.year_byte.year   = datetime->year % 10;
}

bool ds1307_running(ds1307_t* rtc)
{
    return !rtc->memory.seconds_byte.clock_halt;
}

void ds1307_set_running(ds1307_t* rtc, bool running)
{
    rtc->memory.seconds_byte.clock_halt = !running;
}

err_t ds1307_write_running(ds1307_t* rtc)
{
    if(!ds1307_wait_current_op(rtc)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    err = ds1307_write_data(rtc, 0, &rtc->memory, sizeof(seconds_byte_t));
    if(err != E_NO_ERROR) return err;
    
    return ds1307_wait(rtc);
}
