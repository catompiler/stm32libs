#include "lcd8544.h"
#include "utils/utils.h"
#include "utils/delay.h"
#include "bits/bits.h"
#include <string.h>
//#include <stdio.h>


//! Продолжительность ресета.
#define LCD8544_RESET_TIMER_US  150


//! Максимальные значения параметров
#define LCD8544_ADDRESSING_MODE_MAX     LCD8544_ADDRESSING_MODE_VERTICAL
#define LCD8544_INSTRUCTION_SET_MAX     LCD8544_INSTRUCTION_SET_EXTENDED
#define LCD8544_DISPLAY_MODE_MAX        LCD8544_DISPLAY_MODE_INVERSE
#define LCD8544_Y_ADDRESS_MAX           (LCD8544_RAM_HEIGHT - 1)
#define LCD8544_X_ADDRESS_MAX           (LCD8544_RAM_WIDTH - 1)
#define LCD8544_TEMP_COEF_MAX           LCD8544_TEMP_COEF_3
#define LCD8544_BIAS_SYSTEM_MAX         LCD8544_BIAS_SYSTEM_1_DIV_10

//! Команды и их параметры.
#define LCD8544_CMD_FUNCTION_SET                0x20
#define LCD8544_CMD_BIT_POWER_DOWN              2
#define LCD8544_CMD_BIT_ADDRESSING_MODE         1
#define LCD8544_CMD_BIT_INSTRUCTION_SET         0

#define LCD8544_CMD_DISPLAY_CONTROL             0x8
#define LCD8544_CMD_BIT_D                       0
#define LCD8544_CMD_BIT_E                       2

#define LCD8544_CMD_SET_Y_ADDRESS               0x40

#define LCD8544_CMD_SET_X_ADDRESS               0x80

#define LCD8544_CMD_SET_TEMP_COEF               0x4

#define LCD8544_CMD_BIAS_SYSTEM                 0x10

#define LCD8544_CMD_SET_VOLTAGE                 0x80


/**
 * Выставляет значения управляющих пинов для начала обмена данными с LCD.
 * @param lcd LCD.
 * @param is_data Флаг данные/команда.
 */
ALWAYS_INLINE static void lcd8544_pins_begin(lcd8544_t* lcd, bool is_data)
{
    if(is_data){
        GPIO_SetBits(lcd->dc_gpio, lcd->dc_pin);
    }else{
        GPIO_ResetBits(lcd->dc_gpio, lcd->dc_pin);
    }
    GPIO_ResetBits(lcd->ce_gpio, lcd->ce_pin);
}

/**
 * Выставлят значения управляющих пинов для окончания обмена данными с LCD.
 * @param lcd LCD.
 */
ALWAYS_INLINE static void lcd8544_pins_end(lcd8544_t* lcd)
{
    GPIO_SetBits(lcd->ce_gpio, lcd->ce_pin);
    GPIO_SetBits(lcd->dc_gpio, lcd->dc_pin);
}

/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool lcd8544_wait_current_op(lcd8544_t* lcd)
{
    // Если шина занята, да ещё и не нами - возврат ошибки занятости.
    if(spi_bus_busy(lcd->spi) && spi_bus_transfer_id(lcd->spi) != lcd->transfer_id) return false;
    
    future_wait(&lcd->future);
    
    return true;
}

/**
 * Начинает обмен данными с LCD.
 * @param lcd LCD.
 * @param is_data Флаг данные/команда.
 */
static void lcd8544_begin(lcd8544_t* lcd, bool is_data)
{
    future_set_result(&lcd->future, int_to_pvoid(E_NO_ERROR));
    
    future_start(&lcd->future);
    
    lcd8544_pins_begin(lcd, is_data);
}

/**
 * Завершает обмен данными с LCD.
 * @param lcd LCD.
 * @param err Код ошибки.
 */
static void lcd8544_end(lcd8544_t* lcd, err_t err)
{
    future_finish(&lcd->future, int_to_pvoid(err));
    
    lcd8544_pins_end(lcd);
}

/**
 * Отправляет данные в LCD.
 * Асинхронно.
 * @param lcd LCD.
 * @param is_data Флаг данные/команда.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
static err_t lcd8544_send(lcd8544_t* lcd, bool is_data, const uint8_t* data, size_t size)
{
    if(!spi_bus_set_transfer_id(lcd->spi, lcd->transfer_id)) return E_BUSY;
    
    err_t err = spi_message_init(&lcd->message, SPI_WRITE, data, NULL, size);
    if(err != E_NO_ERROR) return err;
    
    lcd8544_begin(lcd, is_data);
    
    err = spi_bus_transfer(lcd->spi, &lcd->message, 1);
    
    if(err != E_NO_ERROR){
        lcd8544_end(lcd, err);
        return err;
    }
    
    return E_NO_ERROR;
}

bool lcd8544_spi_callback(lcd8544_t* lcd)
{
    if(spi_bus_transfer_id(lcd->spi) != lcd->transfer_id) return false;
    
    lcd8544_end(lcd, spi_bus_status(lcd->spi) != SPI_STATUS_TRANSFERED ? E_IO_ERROR : E_NO_ERROR);
    
    return true;
}

err_t lcd8544_init(lcd8544_t* lcd, lcd8544_init_t* lcd_init)
{
    // Clear.
    memset(lcd, 0x0, sizeof(lcd8544_t));
    
    // SPI.
    lcd->spi = lcd_init->spi;
    
    // CE.
    lcd->ce_gpio = lcd_init->ce_gpio;
    lcd->ce_pin = lcd_init->ce_pin;
    
    // D/C.
    lcd->dc_gpio = lcd_init->dc_gpio;
    lcd->dc_pin = lcd_init->dc_pin;
    
    // RESET.
    lcd->rst_gpio = lcd_init->reset_gpio;
    lcd->rst_pin = lcd_init->reset_pin;
    
    future_init(&lcd->future);
    
    // Значения по-умолчанию для этих пинов.
    lcd8544_pins_end(lcd);
    
    // Высокий уровень для пина сброса.
    GPIO_SetBits(lcd->rst_gpio, lcd->rst_pin);
    // Сбросим дисплей.
    //lcd8544_reset(lcd);
    
    // Идентификатор передачи.
    lcd->transfer_id = lcd_init->transfer_id;
    
    return E_NO_ERROR;
}

bool lcd8544_busy(lcd8544_t* lcd)
{
    return future_running(&lcd->future);
}

err_t lcd8544_error(lcd8544_t* lcd)
{
    return pvoid_to_int(err_t, future_result(&lcd->future));
}

err_t lcd8544_wait(lcd8544_t* lcd)
{
    future_wait(&lcd->future);
    return pvoid_to_int(err_t, future_result(&lcd->future));
}

void lcd8544_reset(lcd8544_t* lcd)
{
    //wait
    future_wait(&lcd->future);
    
    GPIO_ResetBits(lcd->rst_gpio, lcd->rst_pin);
    delay_us(LCD8544_RESET_TIMER_US);
    GPIO_SetBits(lcd->rst_gpio, lcd->rst_pin);
}

err_t lcd8544_function_set(lcd8544_t* lcd, bool power_on,
                           lcd8544_addressing_mode_t addressing_mode,
                           lcd8544_instruction_set_t instruction_set)
{
    if(addressing_mode > LCD8544_ADDRESSING_MODE_MAX) return E_INVALID_VALUE;
    if(instruction_set > LCD8544_INSTRUCTION_SET_MAX) return E_INVALID_VALUE;

    if(!lcd8544_wait_current_op(lcd)) return E_BUSY;
    
    lcd->data_cmd = LCD8544_CMD_FUNCTION_SET;
    
    if(!power_on) BIT_ON(lcd->data_cmd, LCD8544_CMD_BIT_POWER_DOWN);
    if(addressing_mode) BIT_ON(lcd->data_cmd, LCD8544_CMD_BIT_ADDRESSING_MODE);
    if(instruction_set) BIT_ON(lcd->data_cmd, LCD8544_CMD_BIT_INSTRUCTION_SET);
    
    err_t err = lcd8544_send(lcd, false, &lcd->data_cmd, 1);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t lcd8544_write_data(lcd8544_t* lcd, uint8_t data)
{
    if(!lcd8544_wait_current_op(lcd)) return E_BUSY;
    
    lcd->data_cmd = data;
    
    err_t err = lcd8544_send(lcd, true, &lcd->data_cmd, 1);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t lcd8544_write(lcd8544_t* lcd, const uint8_t* data, size_t data_size)
{
    if(data_size == 0) return E_NO_ERROR;
    if(data == NULL) return E_NULL_POINTER;
    if(data_size > LCD8544_RAM_SIZE) return E_OUT_OF_RANGE;
    
    if(!lcd8544_wait_current_op(lcd)) return E_BUSY;
    
    err_t err = lcd8544_send(lcd, true, data, data_size);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t lcd8544_set_display_mode(lcd8544_t* lcd, lcd8544_display_mode_t mode)
{
    if(mode > LCD8544_DISPLAY_MODE_MAX) return E_INVALID_VALUE;
    
    if(!lcd8544_wait_current_op(lcd)) return E_BUSY;
    
    lcd->data_cmd = LCD8544_CMD_DISPLAY_CONTROL;
    
    if(BIT_TEST(mode, 0)) BIT_ON(lcd->data_cmd, LCD8544_CMD_BIT_E);
    if(BIT_TEST(mode, 1)) BIT_ON(lcd->data_cmd, LCD8544_CMD_BIT_D);
    
    err_t err = lcd8544_send(lcd, false, &lcd->data_cmd, 1);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t lcd8544_set_y_address(lcd8544_t* lcd, uint8_t address)
{
    if(address > LCD8544_Y_ADDRESS_MAX) return E_INVALID_VALUE;
    
    if(!lcd8544_wait_current_op(lcd)) return E_BUSY;
    
    lcd->data_cmd = LCD8544_CMD_SET_Y_ADDRESS | address;
    
    err_t err = lcd8544_send(lcd, false, &lcd->data_cmd, 1);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t lcd8544_set_x_address(lcd8544_t* lcd, uint8_t address)
{
    if(address > LCD8544_X_ADDRESS_MAX) return E_INVALID_VALUE;
    
    if(!lcd8544_wait_current_op(lcd)) return E_BUSY;
    
    lcd->data_cmd = LCD8544_CMD_SET_X_ADDRESS | address;
    
    err_t err = lcd8544_send(lcd, false, &lcd->data_cmd, 1);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t lcd8544_set_temp_coef(lcd8544_t* lcd, lcd8544_temp_coef_t coef)
{
    if(coef > LCD8544_TEMP_COEF_MAX) return E_INVALID_VALUE;
    
    if(!lcd8544_wait_current_op(lcd)) return E_BUSY;
    
    lcd->data_cmd = LCD8544_CMD_SET_TEMP_COEF | coef;
    
    err_t err = lcd8544_send(lcd, false, &lcd->data_cmd, 1);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t lcd8544_set_bias_system(lcd8544_t* lcd, lcd8544_bias_system_t bias)
{
    if(bias > LCD8544_BIAS_SYSTEM_MAX) return E_INVALID_VALUE;
    
    if(!lcd8544_wait_current_op(lcd)) return E_BUSY;
    
    lcd->data_cmd = LCD8544_CMD_BIAS_SYSTEM | bias;
    
    err_t err = lcd8544_send(lcd, false, &lcd->data_cmd, 1);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t lcd8544_set_voltage(lcd8544_t* lcd, uint16_t mV)
{
    if(mV < LCD8544_VOLTAGE_MIN_mV || mV > LCD8544_VOLTAGE_MAX_mV) return E_INVALID_VALUE;
    
    if(!lcd8544_wait_current_op(lcd)) return E_BUSY;
    
    uint8_t volts_data = (uint8_t)((mV - LCD8544_VOLTAGE_MIN_mV) / 60);
    
    lcd->data_cmd = LCD8544_CMD_SET_VOLTAGE | volts_data;
    
    err_t err = lcd8544_send(lcd, false, &lcd->data_cmd, 1);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

