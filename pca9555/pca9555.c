#include "pca9555.h"
#include <stddef.h>
#include "utils/utils.h"
#include "bits/bits.h"
#include "defs/defs.h"
#include "stm32f10x.h"


//! Номер сообщения с адресом памяти.
#define PCA9555_I2C_MESSAGE_ROM_ADDRESS 0
//! Номер сообщения с данными.
#define PCA9555_I2C_MESSAGE_DATA        1

// Номера пинов для каждого байта.
#define PCA9555_FIRST_BYTE_START_PIN     PCA9555_PIN_0
#define PCA9555_FIRST_BYTE_END_PIN       PCA9555_PIN_7
#define PCA9555_SECOND_BYTE_START_PIN    PCA9555_PIN_8
#define PCA9555_SECOND_BYTE_END_PIN      PCA9555_PIN_15

// Адреса памяти для регистров.
#define PCA9555_ROM_ADDRESS_ALL      0
#define PCA9555_ROM_ADDRESS_IDR0     0
#define PCA9555_ROM_ADDRESS_IDR1     1
#define PCA9555_ROM_ADDRESS_ODR0     2
#define PCA9555_ROM_ADDRESS_ODR1     3
#define PCA9555_ROM_ADDRESS_PIR0     4
#define PCA9555_ROM_ADDRESS_PIR1     5
#define PCA9555_ROM_ADDRESS_CR0      6
#define PCA9555_ROM_ADDRESS_CR1      7


/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool pca9555_wait_current_op(pca9555_t* ioport)
{
    // Если шина занята не нами - возврат ошибки занятости.
    if(i2c_bus_busy(ioport->i2c) && (i2c_bus_transfer_id(ioport->i2c) != ioport->transfer_id)) return false;
    // Подождём выполнения предыдущей операции.
    future_wait(&ioport->future);
    // Подождём шину i2c.
    i2c_bus_wait(ioport->i2c);
    return true;
}

ALWAYS_INLINE static void pca9555_start(pca9555_t* ioport)
{
    future_start(&ioport->future);
}

ALWAYS_INLINE static void pca9555_end(pca9555_t* ioport, err_t err)
{
    future_finish(&ioport->future, int_to_pvoid(err));
}

bool pca9555_i2c_callback(pca9555_t* ioport)
{
    if(i2c_bus_transfer_id(ioport->i2c) != ioport->transfer_id) return false;
    
    pca9555_end(ioport, i2c_bus_status(ioport->i2c) != I2C_STATUS_TRANSFERED ? E_IO_ERROR : E_NO_ERROR);
    
    return true;
}

err_t pca9555_transfer(pca9555_t* ioport, uint8_t rom_address, i2c_direction_t data_direction, void* data, size_t data_size)
{
	// Установим идентификатор передачи.
    if(!i2c_bus_set_transfer_id(ioport->i2c, ioport->transfer_id)) return E_BUSY;
    
    pca9555_start(ioport);
    
    ioport->rom_address = rom_address;
    ioport->i2c_messages[PCA9555_I2C_MESSAGE_DATA].direction = data_direction;
    ioport->i2c_messages[PCA9555_I2C_MESSAGE_DATA].data = data;
    ioport->i2c_messages[PCA9555_I2C_MESSAGE_DATA].data_size = data_size;
    
    err_t err = i2c_bus_transfer(ioport->i2c, ioport->i2c_messages, PCA9555_I2C_MESSAGES_COUNT);
    
    if(err != E_NO_ERROR){
        pca9555_end(ioport, err);
    }
    return err;
}

err_t pca9555_init(pca9555_t* ioport, i2c_bus_t* i2c, i2c_address_t address)
{
    if(i2c == NULL) return E_NULL_POINTER;
    
    // Значения по-умолчанию из даташита.
    ioport->memory.idr0 = 0x0;
    ioport->memory.idr1 = 0x0;
    ioport->memory.odr0 = 0xff;
    ioport->memory.odr1 = 0xff;
    ioport->memory.pir0 = 0x0;
    ioport->memory.pir1 = 0x0;
    ioport->memory.cr0 = 0xff;
    ioport->memory.cr1 = 0xff;
    
    ioport->i2c = i2c;
	
    ioport->transfer_id = PCA9555_I2C_DEFAULT_TRANSFER_ID;
    
    ioport->rom_address = 0;
    
    err_t err = E_NO_ERROR;
	
    err = i2c_message_init(&ioport->i2c_messages[PCA9555_I2C_MESSAGE_ROM_ADDRESS], address, I2C_WRITE, &ioport->rom_address, 1);
    if(err != E_NO_ERROR) return err;
	
    err = i2c_message_init(&ioport->i2c_messages[PCA9555_I2C_MESSAGE_DATA], address, I2C_READ, &ioport->memory, sizeof(pca9555_mem_t));
    if(err != E_NO_ERROR) return err;
	
    future_init(&ioport->future);
    future_set_result(&ioport->future, int_to_pvoid(E_NO_ERROR));
    
    return E_NO_ERROR;
}

i2c_transfer_id_t pca9555_i2c_transfer_id(const pca9555_t* ioport)
{
    return ioport->transfer_id;
}

void pca9555_set_i2c_transfer_id(pca9555_t* ioport, i2c_transfer_id_t transfer_id)
{
    ioport->transfer_id = transfer_id;
}

i2c_address_t pca9555_i2c_address(const pca9555_t* ioport)
{
    return ioport->i2c_messages[PCA9555_I2C_MESSAGE_ROM_ADDRESS].address;
}

void pca9555_set_i2c_address(pca9555_t* ioport, i2c_address_t address)
{
    ioport->i2c_messages[PCA9555_I2C_MESSAGE_ROM_ADDRESS].address = address;
    ioport->i2c_messages[PCA9555_I2C_MESSAGE_DATA].address = address;
}

bool pca9555_busy(pca9555_t* ioport)
{
    return future_running(&ioport->future);
}

bool pca9555_done(pca9555_t* ioport)
{
    return future_done(&ioport->future);
}

err_t pca9555_error(pca9555_t* ioport)
{
    return pvoid_to_int(err_t, future_result(&ioport->future));
}

err_t pca9555_wait(pca9555_t* ioport)
{
    future_wait(&ioport->future);
	
    return pca9555_error(ioport);
}

err_t pca9555_read_state(pca9555_t* ioport)
{
    if(!pca9555_wait_current_op(ioport)) return E_BUSY;
    
    return pca9555_transfer(ioport, PCA9555_ROM_ADDRESS_ALL, I2C_READ,
                            &ioport->memory, sizeof(pca9555_mem_t));
}

err_t pca9555_read_pins_state(pca9555_t* ioport)
{
    if(!pca9555_wait_current_op(ioport)) return E_BUSY;
    
    return pca9555_transfer(ioport, PCA9555_ROM_ADDRESS_IDR0, I2C_READ, &ioport->memory.idr0, 2);
}

err_t pca9555_write_state(pca9555_t* ioport)
{
    if(!pca9555_wait_current_op(ioport)) return E_BUSY;
    
    return pca9555_transfer(ioport, PCA9555_ROM_ADDRESS_ODR0, I2C_WRITE, &ioport->memory.odr0,
                            sizeof(pca9555_mem_t) - offsetof(pca9555_mem_t, odr0));
}

pca9555_pin_state_t pca9555_pin_state(pca9555_t* ioport, pca9555_pin_t pin)
{
    if(pin > PCA9555_FIRST_BYTE_END_PIN){
        return (ioport->memory.idr1 & (pin >> 8)) ? PCA9555_PIN_ON : PCA9555_PIN_OFF;
    }
    return (ioport->memory.idr0 & pin) ? PCA9555_PIN_ON : PCA9555_PIN_OFF;
}

pca9555_pin_state_t pca9555_pin_output_state(pca9555_t* ioport, pca9555_pin_t pin)
{
    if(pin > PCA9555_FIRST_BYTE_END_PIN){
        return (ioport->memory.odr1 & (pin >> 8)) ? PCA9555_PIN_ON : PCA9555_PIN_OFF;
    }
    return (ioport->memory.odr0 & pin) ? PCA9555_PIN_ON : PCA9555_PIN_OFF;
}

pca9555_pins_t pca9555_pins_state(pca9555_t* ioport, pca9555_pin_state_t state)
{
    pca9555_pins_t res = ((pca9555_pins_t)ioport->memory.idr1) << 8;
    res |= ioport->memory.idr0;
    
    if(state == PCA9555_PIN_OFF) res = ~res;
    
    return res;
}

pca9555_pins_t pca9555_pins_output_state(pca9555_t* ioport, pca9555_pin_state_t state)
{
    pca9555_pins_t res = ((pca9555_pins_t)ioport->memory.odr1) << 8;
    res |= ioport->memory.odr0;
    
    if(state == PCA9555_PIN_OFF) res = ~res;
    
    return res;
}

void pca9555_set_pins_state(pca9555_t* ioport, pca9555_pins_t pins, pca9555_pin_state_t state)
{
    uint8_t odr0_pins = pins & 0xff;
    uint8_t odr1_pins = pins >> 8;
    
    if(state == PCA9555_PIN_ON){
        ioport->memory.odr0 |= odr0_pins;
        ioport->memory.odr1 |= odr1_pins;
    }else{
        ioport->memory.odr0 &= ~odr0_pins;
        ioport->memory.odr1 &= ~odr1_pins;
    }
}

err_t pca9555_write_pins_state(pca9555_t* ioport)
{
    if(!pca9555_wait_current_op(ioport)) return E_BUSY;
    
    return pca9555_transfer(ioport, PCA9555_ROM_ADDRESS_ODR0, I2C_WRITE, &ioport->memory.odr0, 2);
}

pca9555_pin_inversion_t pca9555_pin_inversion(pca9555_t* ioport, pca9555_pin_t pin)
{
    if(pin > PCA9555_FIRST_BYTE_END_PIN){
        return (ioport->memory.pir1 & (pin >> 8)) ? PCA9555_PIN_INVERTED : PCA9555_PIN_NORMAL;
    }
    return (ioport->memory.pir0 & pin) ? PCA9555_PIN_INVERTED : PCA9555_PIN_NORMAL;
}

pca9555_pins_t pca9555_pins_inversion(pca9555_t* ioport, pca9555_pin_inversion_t inversion)
{
    pca9555_pins_t res = ((pca9555_pins_t)ioport->memory.pir1) << 8;
    res |= ioport->memory.pir0;
    
    if(inversion == PCA9555_PIN_NORMAL) res = ~res;
    
    return res;
}

void pca9555_set_pins_inversion(pca9555_t* ioport, pca9555_pins_t pins, pca9555_pin_inversion_t inversion)
{
    uint8_t pir0_pins = pins & 0xff;
    uint8_t pir1_pins = pins >> 8;
    
    if(inversion == PCA9555_PIN_INVERTED){
        ioport->memory.pir0 |= pir0_pins;
        ioport->memory.pir1 |= pir1_pins;
    }else{
        ioport->memory.pir0 &= ~pir0_pins;
        ioport->memory.pir1 &= ~pir1_pins;
    }
}

err_t pca9555_write_pins_inversion(pca9555_t* ioport)
{
    if(!pca9555_wait_current_op(ioport)) return E_BUSY;
    
    return pca9555_transfer(ioport, PCA9555_ROM_ADDRESS_PIR0, I2C_WRITE, &ioport->memory.pir0, 2);
}

pca9555_pin_direction_t pca9555_pin_direction(pca9555_t* ioport, pca9555_pin_t pin)
{
    if(pin > PCA9555_FIRST_BYTE_END_PIN){
        return (ioport->memory.cr1 & (pin >> 8)) ? PCA9555_PIN_INPUT : PCA9555_PIN_OUTPUT;
    }
    return (ioport->memory.cr0 & pin) ? PCA9555_PIN_INPUT : PCA9555_PIN_OUTPUT;
}

pca9555_pins_t pca9555_pins_direction(pca9555_t* ioport, pca9555_pin_direction_t direction)
{
    pca9555_pins_t res = ((pca9555_pins_t)ioport->memory.cr1) << 8;
    res |= ioport->memory.cr0;
    
    if(direction == PCA9555_PIN_OUTPUT) res = ~res;
    
    return res;
}

void pca9555_set_pins_direction(pca9555_t* ioport, pca9555_pins_t pins, pca9555_pin_direction_t direction)
{
    uint8_t cr0_pins = pins & 0xff;
    uint8_t cr1_pins = pins >> 8;
    
    if(direction == PCA9555_PIN_INPUT){
        ioport->memory.cr0 |= cr0_pins;
        ioport->memory.cr1 |= cr1_pins;
    }else{
        ioport->memory.cr0 &= ~cr0_pins;
        ioport->memory.cr1 &= ~cr1_pins;
    }
}

err_t pca9555_write_pins_direction(pca9555_t* ioport)
{
    if(!pca9555_wait_current_op(ioport)) return E_BUSY;
    
    return pca9555_transfer(ioport, PCA9555_ROM_ADDRESS_CR0, I2C_WRITE, &ioport->memory.cr0, 2);
}


