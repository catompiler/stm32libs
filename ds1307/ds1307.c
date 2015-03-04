#include "ds1307.h"
#include <string.h>
#include <stddef.h>
#include "utils/utils.h"
#include "bits/bits.h"


static err_t ds1307_do(ds1307_t* rtc);

bool ds1307_i2c_callback(ds1307_t* rtc)
{
    if(i2c_bus_transfer_id(rtc->i2c) != DS1307_I2C_TRANSFER_ID) return false;
    
    switch(i2c_bus_status(rtc->i2c)){
        case I2C_STATUS_IDLE:
        case I2C_STATUS_TRANSFERING:
            return false;
        default:
            break;
    }
    
    ds1307_do(rtc);
    
    return true;
}

static void ds1307_start(ds1307_t* rtc, ds1307_status_t status)
{
    future_start(&rtc->future);
    rtc->status = status;
}

static void ds1307_next(ds1307_t* rtc, ds1307_status_t status)
{
    rtc->status = status;
}

static void ds1307_finish(ds1307_t* rtc, ds1307_status_t status, void* result)
{
    future_finish(&rtc->future, result);
    rtc->status = status;
}

static err_t ds1307_do(ds1307_t* rtc)
{
    err_t err = E_NO_ERROR;
    switch(rtc->status){
        case DS1307_STATUS_READ:
            if(!i2c_bus_set_transfer_id(rtc->i2c, DS1307_I2C_TRANSFER_ID)){
                ds1307_finish(rtc, DS1307_STATUS_ERROR, int_to_pvoid(E_BUSY));
                break;
            }
            rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].direction = I2C_READ;
            err = i2c_bus_transfer(rtc->i2c, rtc->i2c_messages, DS1307_I2C_MESSAGES_COUNT);
            if(err != E_NO_ERROR){
                ds1307_finish(rtc, DS1307_STATUS_ERROR, int_to_pvoid(err));
            }else{
                ds1307_next(rtc, DS1307_STATUS_READING);
            }
            break;
        case DS1307_STATUS_READING:
            if(i2c_bus_status(rtc->i2c) == I2C_STATUS_TRANSFERED){
                ds1307_finish(rtc, DS1307_STATUS_READED, int_to_pvoid(E_NO_ERROR));
            }else{
                ds1307_finish(rtc, DS1307_STATUS_ERROR, int_to_pvoid(E_IO_ERROR));
            }
            break;
        case DS1307_STATUS_WRITE:
            if(!i2c_bus_set_transfer_id(rtc->i2c, DS1307_I2C_TRANSFER_ID)){
                ds1307_finish(rtc, DS1307_STATUS_ERROR, int_to_pvoid(E_BUSY));
                break;
            }
            rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].direction = I2C_WRITE;
            err = i2c_bus_transfer(rtc->i2c, rtc->i2c_messages, DS1307_I2C_MESSAGES_COUNT);
            if(err != E_NO_ERROR){
                ds1307_finish(rtc, DS1307_STATUS_ERROR, int_to_pvoid(err));
            }else{
                ds1307_next(rtc, DS1307_STATUS_WRITING);
            }
            break;
        case DS1307_STATUS_WRITING:
            if(i2c_bus_status(rtc->i2c) == I2C_STATUS_TRANSFERED){
                ds1307_finish(rtc, DS1307_STATUS_WRITED, int_to_pvoid(E_NO_ERROR));
            }else{
                ds1307_finish(rtc, DS1307_STATUS_ERROR, int_to_pvoid(E_IO_ERROR));
            }
            break;
        default:
            break;
    }
    return err;
}

err_t ds1307_init(ds1307_t* rtc, i2c_bus_t* i2c)
{
    if(i2c == NULL) return E_NULL_POINTER;
    
    memset(&rtc->memory, 0x0, sizeof(ds1307mem_t));
    
    rtc->i2c = i2c;
    
    rtc->status = DS1307_STATUS_NONE;
    
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

bool ds1307_done(ds1307_t* rtc)
{
    return future_done(&rtc->future);
}

void ds1307_wait(ds1307_t* rtc)
{
    future_wait(&rtc->future);
}

ds1307_status_t ds1307_status(ds1307_t* rtc)
{
    return rtc->status;
}

err_t ds1307_error(ds1307_t* rtc)
{
    return pvoid_to_int(err_t, future_result(&rtc->future));
}

err_t ds1307_read(ds1307_t* rtc)
{
    if(future_running(&rtc->future)) return E_DS1307_BUSY;
    
    rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].data_size = sizeof(ds1307mem_t);
    
    ds1307_start(rtc, DS1307_STATUS_READ);
    
    return ds1307_do(rtc);
}

err_t ds1307_write(ds1307_t* rtc)
{
    if(future_running(&rtc->future)) return E_DS1307_BUSY;
    
    rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].data_size = sizeof(ds1307mem_t);
    
    ds1307_start(rtc, DS1307_STATUS_WRITE);
    
    return ds1307_do(rtc);
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
    if(future_running(&rtc->future)) return E_DS1307_BUSY;
    
    rtc->i2c_messages[DS1307_I2C_MESSAGE_DATA].data_size = sizeof(seconds_byte_t);
    
    ds1307_start(rtc, DS1307_STATUS_WRITE);
    
    return ds1307_do(rtc);
}
