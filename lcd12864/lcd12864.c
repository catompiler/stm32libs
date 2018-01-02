#include "lcd12864.h"
#include "defs/defs.h"
#include "utils/delay.h"
#include "bits/bits.h"

//! Максимальный адрес в одном контроллере.
#define LCD12864_HALF_ADDRESS_MAX    63
//! Ширина памяти одного контроллера.
#define LCD12864_HALF_WIDTH          64

//! Размер половины экрана в байтах.
#define LCD12864_HALF_SIZE_BYTES     (LCD12864_SIZE_BYTES / 2)

//! Время импульса RESET.
#define LCD12864_RESET_PULSE_US      1500

//! Задержка для установки R/W и RS значений.
//#define LCD12864_RW_RS_SETUP_TIME_NS 0
//! Время полупериода цикла чтения/записи бита.
#define LCD12864_E_CYCLE_HALF_TIME_NS  210
#define LCD12864_E_CYCLE_HALF_TIME_CYCLES  15
//! Число попыток чтения флага занятости
//! (Время ожидания = число попыток * 
//! (LCD12864_RW_RS_SETUP_TIME_NS + LCD12864_CYCLE_HALF_TIME_NS * 2))
//! Максимальное время выполнения операции 1.53 мс,
//! Возьмём удвоенное время.
//! Максимальное время выполнения.
#define LCD12864_MAX_EXECUTION_TIME_US 2000
//! Максимальное число попыток.
#ifdef LCD12864_RW_RS_SETUP_TIME_US
#define LCD12864_WAIT_BUSY_MAX_COUNT ((LCD12864_MAX_EXECUTION_TIME_US * 1000) / (LCD12864_RW_RS_SETUP_TIME_NS + LCD12864_E_CYCLE_HALF_TIME_NS * 2))
#else
#define LCD12864_WAIT_BUSY_MAX_COUNT ((LCD12864_MAX_EXECUTION_TIME_US * 1000) / (LCD12864_E_CYCLE_HALF_TIME_NS * 2))
#endif


//! Команды LCD.
//! Включение/выключение дисплея/курсора/моргания.
#define LCD12864_CMD_DISPLAY_ON_OFF           0x3e
//! Вкл/Выкл дисплея.
#define LCD12864_ON_OFF_DISPLAY_BIT           0

//! Установка страницы.
#define LCD12864_CMD_SET_START_LINE           0xc0
//! Маска X.
#define LCD12864_START_LINE_MASK              0x3f

//! Установка страницы.
#define LCD12864_CMD_SET_PAGE                 0xb8
//! Маска X.
#define LCD12864_PAGE_MASK                    0x7

//! Установка адреса.
#define LCD12864_CMD_SET_ADDRESS              0x40
//! Маска адреса.
#define LCD12864_ADDRESS_MASK                 0x3f

//! Маска флага занятости.
#define LCD12864_BUSY_FLAG_MASK         0x80



ALWAYS_INLINE static void lcd12864_delay_half_e_period()
{
    delay_cycles(LCD12864_E_CYCLE_HALF_TIME_CYCLES);
}

/**
 * Устанавливает шину данных на выход.
 * @param lcd LCD.
 */
ALWAYS_INLINE static void lcd12864_data_bus_set_out(lcd12864_t* lcd)
{
    lcd->gpio_data->CRL &= lcd->gpio_data_crl_mask_inv;
    lcd->gpio_data->CRH &= lcd->gpio_data_crh_mask_inv;
    __NOP();
    lcd->gpio_data->CRL |= lcd->gpio_data_crl_out;
    lcd->gpio_data->CRH |= lcd->gpio_data_crh_out;
    __NOP();
    lcd->gpio_data->BRR = lcd->gpio_data_mask;
}

/**
 * Устанавливает шину данных на вход.
 * @param lcd LCD.
 */
ALWAYS_INLINE static void lcd12864_data_bus_set_in(lcd12864_t* lcd)
{
    lcd->gpio_data->BRR = lcd->gpio_data_mask;
    __NOP();
    lcd->gpio_data->CRL &= lcd->gpio_data_crl_mask_inv;
    lcd->gpio_data->CRH &= lcd->gpio_data_crh_mask_inv;
    __NOP();
    lcd->gpio_data->CRL |= lcd->gpio_data_crl_in;
    lcd->gpio_data->CRH |= lcd->gpio_data_crh_in;
}

/**
 * Записывает байт данных в шину.
 * @param lcd LCD.
 * @param data Байт данных.
 */
ALWAYS_INLINE static void lcd12864_data_bus_write(lcd12864_t* lcd, uint8_t data)
{
    lcd->gpio_data->BRR = lcd->gpio_data_mask;
    __NOP();
    lcd->gpio_data->BSRR = ((uint32_t)data) << lcd->gpio_data_offset;
    __NOP();
}

/**
 * Читает байт данных из шины.
 * @param lcd LCD.
 * @return Байт данных.
 */
ALWAYS_INLINE static uint8_t lcd12864_data_bus_read(lcd12864_t* lcd)
{
    uint32_t tmp = lcd->gpio_data->IDR;
    return tmp >> lcd->gpio_data_offset;
}

/**
 * Выбирает контроллеры LCD.
 * @param lcd LCD.
 * @param cs1 Контроллер 1.
 * @param cs2 Контроллер 2.
 */
ALWAYS_INLINE static void lcd12864_select_chip(lcd12864_t* lcd, bool cs1, bool cs2)
{
    if(cs1)
        { lcd->gpio_cs1->BSRR = lcd->gpio_pin_cs1; }
    else
        { lcd->gpio_cs1->BRR = lcd->gpio_pin_cs1; }
    __NOP();
    if(cs2)
        { lcd->gpio_cs2->BSRR = lcd->gpio_pin_cs2; }
    else
        { lcd->gpio_cs2->BRR = lcd->gpio_pin_cs2; }
    __NOP();
}

/**
 * Записывает команду в LCD.
 * @param lcd LCD.
 * @param cmd Команда.
 */
ALWAYS_INLINE static void lcd12864_cmd_write(lcd12864_t* lcd, uint8_t cmd)
{
    // RS = 0 (C).
    lcd->gpio_dc->BRR = lcd->gpio_pin_dc;
    __NOP();
    // RW = 0 (W).
    lcd->gpio_rw->BRR = lcd->gpio_pin_rw;
    __NOP();
    // OUT.
    lcd12864_data_bus_set_out(lcd);
    // write.
    lcd12864_data_bus_write(lcd, cmd);
    // E = 1.
    lcd->gpio_e->BSRR = lcd->gpio_pin_e;
    lcd12864_delay_half_e_period();
    // E = 0.
    lcd->gpio_e->BRR = lcd->gpio_pin_e;
    lcd12864_delay_half_e_period();
    // IN.
    lcd12864_data_bus_set_in(lcd);
}

/**
 * Записывает данные в LCD.
 * @param lcd LCD.
 * @param data Данные.
 */
ALWAYS_INLINE static void lcd12864_data_write(lcd12864_t* lcd, uint8_t data)
{
    // RS = 1 (D).
    lcd->gpio_dc->BSRR = lcd->gpio_pin_dc;
    __NOP();
    // RW = 0 (W).
    lcd->gpio_rw->BRR = lcd->gpio_pin_rw;
    __NOP();
    // OUT.
    lcd12864_data_bus_set_out(lcd);
    // write.
    lcd12864_data_bus_write(lcd, data);
    // E = 1.
    lcd->gpio_e->BSRR = lcd->gpio_pin_e;
    lcd12864_delay_half_e_period();
    // E = 0.
    lcd->gpio_e->BRR = lcd->gpio_pin_e;
    lcd12864_delay_half_e_period();
    // IN.
    lcd12864_data_bus_set_in(lcd);
}

/**
 * Читает байт данных из LCD.
 * @param lcd LCD.
 * @return Байт данных.
 */
ALWAYS_INLINE static uint8_t lcd12864_data_read(lcd12864_t* lcd)
{
    uint8_t data = 0;
    // RS = 1 (D).
    lcd->gpio_dc->BSRR = lcd->gpio_pin_dc;
    __NOP();
    // RW = 1 (R).
    lcd->gpio_rw->BSRR = lcd->gpio_pin_rw;
    __NOP();
    // IN.
    lcd12864_data_bus_set_in(lcd);
    // E = 1.
    lcd->gpio_e->BSRR = lcd->gpio_pin_e;
    lcd12864_delay_half_e_period();
    // read.
    data = lcd12864_data_bus_read(lcd);
    // E = 0.
    lcd->gpio_e->BRR = lcd->gpio_pin_e;
    lcd12864_delay_half_e_period();
    
    return data;
}

/**
 * Читает статус LCD.
 * @param lcd LCD.
 * @return Статус LCD.
 */
ALWAYS_INLINE static uint8_t lcd12864_status_read(lcd12864_t* lcd)
{
    uint8_t data = 0;
    // RS = 0 (C).
    lcd->gpio_dc->BRR = lcd->gpio_pin_dc;
    __NOP();
    // RW = 1 (R).
    lcd->gpio_rw->BSRR = lcd->gpio_pin_rw;
    __NOP();
    // IN.
    lcd12864_data_bus_set_in(lcd);
    // E = 1.
    lcd->gpio_e->BSRR = lcd->gpio_pin_e;
    lcd12864_delay_half_e_period();
    // read.
    data = lcd12864_data_bus_read(lcd);
    // E = 0.
    lcd->gpio_e->BRR = lcd->gpio_pin_e;
    lcd12864_delay_half_e_period();
    
    return data;
}

/**
 * Проверяет занятость выбранного контроллера LCD.
 * @param lcd LCD.
 * @return Флаг занятости выбранного контроллера LCD.
 */
ALWAYS_INLINE static bool lcd12864_chip_busy(lcd12864_t* lcd)
{
    return (lcd12864_status_read(lcd) & LCD12864_BUSY_FLAG_MASK) != 0;
}

/**
 * Ждёт освобождения выбранного контроллера LCD.
 * @param lcd LCD.
 * @return флаг освободжения выбранного контроллера LCD.
 */
ALWAYS_INLINE static bool lcd12864_chip_wait(lcd12864_t* lcd)
{
    uint16_t i;
    for(i = 0; i < LCD12864_WAIT_BUSY_MAX_COUNT; i ++){
        if(!lcd12864_chip_busy(lcd)) return true;
    }
    return false;
}

err_t lcd12864_init(lcd12864_t* lcd, lcd12864_init_t* lcd_init)
{
    if(lcd_init->gpio_data_offset > 8) return E_INVALID_VALUE;
    
    GPIO_InitTypeDef gpio_is;
    gpio_is.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_is.GPIO_Speed = GPIO_Speed_50MHz;
    
    // Reset.
    lcd->gpio_reset = lcd_init->gpio_reset;
    lcd->gpio_pin_reset = lcd_init->gpio_pin_reset;
    gpio_is.GPIO_Pin = lcd->gpio_pin_reset;
    GPIO_Init(lcd->gpio_reset, &gpio_is);
    lcd->gpio_reset->BSRR = lcd->gpio_pin_reset;
    
    // CS1.
    lcd->gpio_cs1 = lcd_init->gpio_cs1;
    lcd->gpio_pin_cs1 = lcd_init->gpio_pin_cs1;
    gpio_is.GPIO_Pin = lcd->gpio_pin_cs1;
    GPIO_Init(lcd->gpio_cs1, &gpio_is);
    
    // CS2.
    lcd->gpio_cs2 = lcd_init->gpio_cs2;
    lcd->gpio_pin_cs2 = lcd_init->gpio_pin_cs2;
    gpio_is.GPIO_Pin = lcd->gpio_pin_cs2;
    GPIO_Init(lcd->gpio_cs2, &gpio_is);
    
    // E.
    lcd->gpio_e = lcd_init->gpio_e;
    lcd->gpio_pin_e = lcd_init->gpio_pin_e;
    gpio_is.GPIO_Pin = lcd->gpio_pin_e;
    GPIO_Init(lcd->gpio_e, &gpio_is);
    
    // RW.
    lcd->gpio_rw = lcd_init->gpio_rw;
    lcd->gpio_pin_rw = lcd_init->gpio_pin_rw;
    gpio_is.GPIO_Pin = lcd->gpio_pin_rw;
    GPIO_Init(lcd->gpio_rw, &gpio_is);
    
    // DC.
    lcd->gpio_dc = lcd_init->gpio_dc;
    lcd->gpio_pin_dc = lcd_init->gpio_pin_dc;
    gpio_is.GPIO_Pin = lcd->gpio_pin_dc;
    GPIO_Init(lcd->gpio_dc, &gpio_is);
    
    // DATA.
    lcd->gpio_data = lcd_init->gpio_data;
    lcd->gpio_data_offset = lcd_init->gpio_data_offset;
    lcd->gpio_data_mask = BIT_MAKE_MASK((uint32_t)8, lcd->gpio_data_offset);
    //lcd->gpio_data_mask_inv = ~lcd->gpio_data_mask;
    
    // Просчитанные заранее значения регистров конфигурации GPIO шины данных.
    // Маска.
    uint64_t gpio_data_cr_mask = (uint64_t)0xffffffff << (lcd->gpio_data_offset * 4);
    lcd->gpio_data_crl_mask_inv = ~(gpio_data_cr_mask & 0xffffffff);
    lcd->gpio_data_crh_mask_inv = ~(gpio_data_cr_mask >> 32);
    // Выход - OUT_PP 50MHz.
    uint64_t gpio_data_cr_out = (uint64_t)0x33333333 << (lcd->gpio_data_offset * 4);
    lcd->gpio_data_crl_out = gpio_data_cr_out & 0xffffffff;
    lcd->gpio_data_crh_out = gpio_data_cr_out >> 32;
    // Вход - IN_FLOATING.
    uint64_t gpio_data_cr_in = (uint64_t)0x44444444 << (lcd->gpio_data_offset * 4);
    lcd->gpio_data_crl_in = gpio_data_cr_in & 0xffffffff;
    lcd->gpio_data_crh_in = gpio_data_cr_in >> 32;
    
    lcd12864_data_bus_set_in(lcd);
    
    return E_NO_ERROR;
}

err_t lcd12864_reset(lcd12864_t* lcd)
{
    lcd12864_select_chip(lcd, true, true);
    
    lcd->gpio_reset->BRR = lcd->gpio_pin_reset;
    
    delay_us(LCD12864_RESET_PULSE_US);
    
    lcd->gpio_reset->BSRR = lcd->gpio_pin_reset;
    
    lcd12864_wait(lcd);
    
    return E_NO_ERROR;
}

bool lcd12864_busy(lcd12864_t* lcd)
{
    lcd12864_select_chip(lcd, true, false);
    if(lcd12864_status_read(lcd) & LCD12864_BUSY_FLAG_MASK) return true;
    
    lcd12864_select_chip(lcd, false, true);
    return (lcd12864_status_read(lcd) & LCD12864_BUSY_FLAG_MASK) != 0;
}

bool lcd12864_wait(lcd12864_t* lcd)
{
    uint16_t i;
    for(i = 0; i < LCD12864_WAIT_BUSY_MAX_COUNT; i ++){
        if(!lcd12864_busy(lcd)) return true;
    }
    return false;
}

err_t lcd12864_control(lcd12864_t* lcd, bool display_on)
{
    if(!lcd12864_wait(lcd)) return E_BUSY;
    
    uint8_t cmd = LCD12864_CMD_DISPLAY_ON_OFF;
    BIT_SET(cmd, LCD12864_ON_OFF_DISPLAY_BIT, display_on);
    
    lcd12864_select_chip(lcd, true, true);
    lcd12864_cmd_write(lcd, cmd);
    
    return E_NO_ERROR;
}

err_t lcd12864_set_start_line(lcd12864_t* lcd, uint8_t start_line)
{
    if(!lcd12864_wait(lcd)) return E_BUSY;
    
    uint8_t cmd = LCD12864_CMD_SET_START_LINE;
    cmd |= (start_line & LCD12864_START_LINE_MASK);
    
    lcd12864_select_chip(lcd, true, true);
    lcd12864_cmd_write(lcd, cmd);
    
    return E_NO_ERROR;
}

/**
 * Устанавливает страницу.
 * @param lcd LCD.
 * @param page Страница.
 */
ALWAYS_INLINE static void lcd12864_set_page(lcd12864_t* lcd, uint8_t page)
{
    uint8_t cmd = LCD12864_CMD_SET_PAGE;
    cmd |= (page & LCD12864_PAGE_MASK);
    
    lcd12864_cmd_write(lcd, cmd);
}

/**
 * Устанавливает адрес.
 * @param lcd LCD.
 * @param address Адрес.
 */
ALWAYS_INLINE static void lcd12864_set_address(lcd12864_t* lcd, uint8_t address)
{
    uint8_t cmd = LCD12864_CMD_SET_ADDRESS;
    cmd |= (address & LCD12864_ADDRESS_MASK);
    
    lcd12864_cmd_write(lcd, cmd);
}

err_t lcd12864_clear(lcd12864_t* lcd)
{
    if(!lcd12864_wait(lcd)) return E_BUSY;
    
    size_t i;
    for(i = 0; i < LCD12864_HALF_SIZE_BYTES; i ++){
        if((i & 0x3f) == 0){
            lcd12864_wait(lcd);
            lcd12864_select_chip(lcd, true, true);
            lcd12864_set_page(lcd, i >> 6);
            
            lcd12864_wait(lcd);
            lcd12864_select_chip(lcd, true, true);
            lcd12864_set_address(lcd, 0);
        }
        lcd12864_wait(lcd);
        lcd12864_select_chip(lcd, true, true);
        lcd12864_data_write(lcd, 0);
    }
    return E_NO_ERROR;
}

err_t lcd12864_write(lcd12864_t* lcd, const uint8_t* data, size_t count)
{
    if(count > LCD12864_SIZE_BYTES) return E_OUT_OF_RANGE;
    if(!lcd12864_wait(lcd)) return E_BUSY;
    
    bool cs1 = true;
    bool cs2 = false;
    
    size_t i;
    for(i = 0; i < count; i ++){
        if((i & 0x3f) == 0){
            
            if((i & 0x7f) == 0){
                lcd12864_wait(lcd);
                lcd12864_select_chip(lcd, true, true);
                lcd12864_set_page(lcd, i / 128);
                
                lcd12864_wait(lcd);
                lcd12864_select_chip(lcd, true, true);
                lcd12864_set_address(lcd, 0);
            }
            
            cs1 = (i & 0x40) == 0;
            cs2 = !cs1;
            
            lcd12864_select_chip(lcd, cs1, cs2);
        }
        lcd12864_chip_wait(lcd);
        lcd12864_data_write(lcd, data[i]);
    }
    
    return E_NO_ERROR;
}
