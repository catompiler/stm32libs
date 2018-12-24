#include "lcd44780.h"
#include "utils/delay.h"
#include "bits/bits.h"


//! Время полупериода цикла доступа к экрану (E).
#define LCD44780_E_CYCLE_HALF_TIME_US 1
//! Задержка для установки R/W и RS значений.
//#define LCD44780_RW_RS_SETUP_TIME_US 0
//! Число попыток чтения флага занятости
//! (Время ожидания = число попыток *
//! (LCD44780_RW_RS_SETUP_TIME_NS + LCD44780_CYCLE_HALF_TIME_NS * 2))
//! Максимальное время выполнения операции 1.53 мс.
//! Возьмём увеличенное время.
//! Максимальное время выполнения.
#define LCD44780_MAX_EXECUTION_TIME_US 2000
//! Максимальное число попыток.
#ifdef LCD44780_RW_RS_SETUP_TIME_US
#define LCD44780_WAIT_BUSY_MAX_COUNT ((LCD44780_MAX_EXECUTION_TIME_US) / (LCD44780_RW_RS_SETUP_TIME_US + LCD44780_E_CYCLE_HALF_TIME_US * 2))
#else
#define LCD44780_WAIT_BUSY_MAX_COUNT ((LCD44780_MAX_EXECUTION_TIME_US) / (LCD44780_E_CYCLE_HALF_TIME_US * 2))
#endif

//! Команды LCD.
//! Очистка.
#define LCD44780_CMD_CLEAR                      0x1

//! Домой.
#define LCD44780_CMD_HOME                       0x2

//! Режим движения курсора / дисплея.
#define LCD44780_CMD_ENTRY_MODE                 0x4
//! Номер бита режима движения курсора.
#define LCD44780_ENTRY_MODE_CURSOR_BIT          1
//! Номер бита режима движения дисплея.
#define LCD44780_ENTRY_MODE_DISPLAY_BIT         0

//! Включение/выключение дисплея/курсора/моргания.
#define LCD44780_CMD_DISPLAY_ON_OFF             0x8
//! Вкл/Выкл дисплея.
#define LCD44780_ON_OFF_DISPLAY_BIT             2
//! Вкл/Выкл курсора.
#define LCD44780_ON_OFF_CURSOR_BIT              1
//! Вкл/Выкл моргания курсора.
#define LCD44780_ON_OFF_CURSOR_BLINK_BIT        0

//! Сдвиг курсора / дисплея.
#define LCD44780_CMD_CURSOR_DISPLAY_SHIFT       0x10
//! Бит сдвига курсора/дисплея.
#define LCD44780_SHIFT_DISPLAY_CURSOR_BIT       3
//! Бит сдвига влево/вправо.
#define LCD44780_SHIFT_RIGHT_LEFT_BIT           2

//! Конфигурация дисплея.
#define LCD44780_CMD_FUNCTION_SET               0x20
//! Ширина шины.
#define LCD44780_FUNCTION_SET_DATA_WIDTH_BIT    4
//! Число линий.
#define LCD44780_FUNCTION_SET_LINE_NUMBER_BIT   3
//! Тип шрифта
#define LCD44780_FUNCTION_SET_FONT_TYPE_BIT     2

//! Установка адреса знакогенератора.
#define LCD44780_CMD_SET_CGRAM_ADDRESS  0x40
//! Маска адреса.
#define LCD44780_CGRAM_ADDRESS_MASK     0x3f

//! Установка адреса видеопамяти.
#define LCD44780_CMD_SET_DDRAM_ADDRESS  0x80
//! Маска адреса.
#define LCD44780_DDRAM_ADDRESS_MASK     0x7f

//! Верхние 4 бита команды конфигурации дисплея,
//! предназначенной для перевода его на 4 битную шину.
#define LCD44780_FUNCTION_SET_DATA_WIDTH_4BIT   0x2

//! Маска флага занятости.
#define LCD44780_BUSY_FLAG_MASK         0x80
#define LCD44780_ADDRESS_MASK 0x7f

/**
 * Ждёт половину цикла чтения/записи.
 */
ALWAYS_INLINE static void lcd44780_delay_half_e_period(void)
{
    delay_us(LCD44780_E_CYCLE_HALF_TIME_US);
}

/**
 * Устанавливает направление шины.
 * @param lcd Экран.
 * @param dir Направление шины.
 * @return Флаг установки направления.
 */
ALWAYS_INLINE static bool lcd44780_data_bus_set_dir(lcd44780_t* lcd, lcd44780_bus_direction_t dir)
{
    if(!lcd->set_bus_direction) return false;

    lcd->set_bus_direction(dir);

    return true;
}

/**
 * Сбрасывает пины.
 * @param lcd Экран.
 */
ALWAYS_INLINE static void lcd44780_pins_reset(lcd44780_t* lcd)
{
    gpio_reset(lcd->e_gpio, lcd->e_pin);
    gpio_reset(lcd->rs_gpio, lcd->rs_pin);
    gpio_reset(lcd->rw_gpio, lcd->rw_pin);

    gpio_reset(lcd->data_gpio, lcd->data_pins_mask);
    lcd44780_data_bus_set_dir(lcd, LCD44780_BUS_DIRECTION_IN);
}

/**
 * Настройка пинов в начале доступа к экрану.
 * @param lcd Экран.
 * @param data Флаг данных.
 * @param read Флаг чтения.
 */
static void lcd44780_begin(lcd44780_t* lcd, bool data, bool read)
{
    if(data){
        gpio_set(lcd->rs_gpio, lcd->rs_pin);
    }

    if(read){
        gpio_set(lcd->rw_gpio, lcd->rw_pin);
    }else{
        lcd44780_data_bus_set_dir(lcd, LCD44780_BUS_DIRECTION_OUT);
    }
}

/**
 * Настройка пинов в конце доступа к экрану.
 * @param lcd Экран.
 */
static void lcd44780_end(lcd44780_t* lcd)
{
    gpio_reset(lcd->rs_gpio, lcd->rs_pin);
    gpio_reset(lcd->rw_gpio, lcd->rw_pin);

    lcd44780_data_bus_set_dir(lcd, LCD44780_BUS_DIRECTION_IN);
}

/**
 * Записывает данные в шину.
 * @param lcd Экран.
 * @param data Данные.
 */
static void lcd44780_data_bus_write_raw(lcd44780_t* lcd, uint8_t data)
{
    gpio_set_output_value(lcd->data_gpio, lcd->data_pins_mask, ((uint16_t)data) << lcd->data_pins_offset);

    gpio_set(lcd->e_gpio, lcd->e_pin);
    lcd44780_delay_half_e_period();

    gpio_reset(lcd->e_gpio, lcd->e_pin);
    lcd44780_delay_half_e_period();
}

/**
 * Записывает данные в шину 4 бит.
 * @param lcd Экран.
 * @param data Данные.
 */
ALWAYS_INLINE static void lcd44780_data_bus_write_4bit(lcd44780_t* lcd, uint8_t data)
{
    lcd44780_data_bus_write_raw(lcd, data >> 4);
    lcd44780_data_bus_write_raw(lcd, data & 0xf);
}

/**
 * Записывает данные в шину 8 бит.
 * @param lcd Экран.
 * @param data Данные.
 */
ALWAYS_INLINE static void lcd44780_data_bus_write_8bit(lcd44780_t* lcd, uint8_t data)
{
    lcd44780_data_bus_write_raw(lcd, data);
}

/**
 * Записывает данные в шину
 * в зависимости от ширины.
 * @param lcd Экран.
 * @param data Данные.
 */
static void lcd44780_data_bus_write(lcd44780_t* lcd, uint8_t data)
{
    if(lcd->data_bus_width == LCD44780_BUS_WIDTH_8BIT){
        lcd44780_data_bus_write_8bit(lcd, data);
    }else{
        lcd44780_data_bus_write_4bit(lcd, data);
    }
}

/**
 * Читает данные из шины.
 * @param lcd Экран.
 * @return Данные.
 */
static uint8_t lcd44780_data_bus_read_raw(lcd44780_t* lcd)
{
    uint8_t data = 0;

    gpio_set(lcd->e_gpio, lcd->e_pin);
    lcd44780_delay_half_e_period();

    data = gpio_input_value(lcd->data_gpio, lcd->data_pins_mask) >> lcd->data_pins_offset;

    gpio_reset(lcd->e_gpio, lcd->e_pin);
    lcd44780_delay_half_e_period();

    return data;
}

/**
 * Читает данные из шины 4 бит.
 * @param lcd Экран.
 * @return Данные.
 */
ALWAYS_INLINE static uint8_t lcd44780_data_bus_read_4bit(lcd44780_t* lcd)
{
    uint8_t data_hi = lcd44780_data_bus_read_raw(lcd);
    uint8_t data_lo = lcd44780_data_bus_read_raw(lcd);

    return (data_hi << 4) | data_lo;
}

/**
 * Читает данные из шины 8 бит.
 * @param lcd Экран.
 * @return Данные.
 */
ALWAYS_INLINE static uint8_t lcd44780_data_bus_read_8bit(lcd44780_t* lcd)
{
    return lcd44780_data_bus_read_raw(lcd);
}

/**
 * Читает данные из шины
 * в зависимости от ширины.
 * @param lcd Экран.
 * @return Данные.
 */
static uint8_t lcd44780_data_bus_read(lcd44780_t* lcd)
{
    uint8_t data = 0;

    if(lcd->data_bus_width == LCD44780_BUS_WIDTH_8BIT){
        data = lcd44780_data_bus_read_8bit(lcd);
    }else{
        data = lcd44780_data_bus_read_4bit(lcd);
    }

    return data;
}

/**
 * Читает состояние экрана
 * (флаг занятости и текущий адрес).
 * @param lcd Экран.
 * @return Состояние.
 */
static uint8_t lcd44780_read_bf_address(lcd44780_t* lcd)
{
    uint8_t status = 0;

    lcd44780_begin(lcd, false, true);

    status = lcd44780_data_bus_read(lcd);

    lcd44780_end(lcd);

    return status;
}

/**
 * Записывает команду в экран.
 * @param lcd Экран.
 * @param cmd Команда.
 */
static void lcd44780_cmd_write(lcd44780_t* lcd, uint8_t cmd)
{
    lcd44780_begin(lcd, false, false);

    lcd44780_data_bus_write(lcd, cmd);

    lcd44780_end(lcd);
}

/**
 * Записывает данные в экран.
 * @param lcd Экран.
 * @param data Данные.
 */
static void lcd44780_data_write(lcd44780_t* lcd, uint8_t data)
{
    lcd44780_begin(lcd, true, false);

    lcd44780_data_bus_write(lcd, data);

    lcd44780_end(lcd);
}

/**
 * Читает данные из экрана.
 * @param lcd Экран.
 * @return Данные.
 */
static uint8_t lcd44780_data_read(lcd44780_t* lcd)
{
    uint8_t data = 0;

    lcd44780_begin(lcd, true, true);

    data = lcd44780_data_bus_read(lcd);

    lcd44780_end(lcd);

    return data;
}

/**
 * Обновляет маску пинов шины данных экрана.
 * @param lcd Экран.
 */
ALWAYS_INLINE static void lcd44780_update_data_bus_mask(lcd44780_t* lcd)
{
    size_t bus_width = (lcd->data_bus_width == LCD44780_BUS_WIDTH_8BIT) ? 8 : 4;
    lcd->data_pins_mask = BIT_MAKE_MASK(bus_width, lcd->data_pins_offset);
}

/**
 * Переводит экран на 4х-битную шину данных.
 * @param lcd Экран.
 */
static void lcd44780_switch_to_4bit(lcd44780_t* lcd)
{
    lcd44780_begin(lcd, false, false);

    lcd44780_data_bus_write_raw(lcd, LCD44780_FUNCTION_SET_DATA_WIDTH_4BIT);

    lcd44780_end(lcd);
}

err_t lcd44780_init(lcd44780_t* lcd, lcd44780_init_t* init)
{
    if(init == NULL) return E_NULL_POINTER;
    if(!init->set_bus_direction) return E_NULL_POINTER;

    // RS.
    lcd->rs_gpio = init->rs_gpio;
    lcd->rs_pin = init->rs_pin;
    // RW.
    lcd->rw_gpio = init->rw_gpio;
    lcd->rw_pin = init->rw_pin;
    // E.
    lcd->e_gpio = init->e_gpio;
    lcd->e_pin = init->e_pin;
    // Data.
    lcd->data_gpio = init->data_gpio;
    lcd->set_bus_direction = init->set_bus_direction;
    lcd->data_bus_width = init->data_bus_width;
    lcd->data_pins_offset = init->data_pins_offset;

    lcd44780_update_data_bus_mask(lcd);
    lcd44780_pins_reset(lcd);
    if(lcd->data_bus_width == LCD44780_BUS_WIDTH_4BIT){
        lcd44780_switch_to_4bit(lcd);
    }

    return E_NO_ERROR;
}

bool lcd44780_busy(lcd44780_t*lcd)
{
    return (lcd44780_read_bf_address(lcd) & LCD44780_BUSY_FLAG_MASK) != 0;
}

bool lcd44780_wait(lcd44780_t* lcd)
{
    size_t i;

    for(i = 0; i < LCD44780_WAIT_BUSY_MAX_COUNT; i ++){
        if(!lcd44780_busy(lcd)) return true;
    }

    return false;
}

uint8_t lcd44780_address(lcd44780_t* lcd)
{
    return lcd44780_read_bf_address(lcd) & LCD44780_ADDRESS_MASK;
}


err_t lcd44780_clear(lcd44780_t* lcd)
{
    if(!lcd44780_wait(lcd)) return E_BUSY;

    lcd44780_cmd_write(lcd, LCD44780_CMD_CLEAR);

    return E_NO_ERROR;
}

err_t lcd44780_home(lcd44780_t* lcd)
{
    if(!lcd44780_wait(lcd)) return E_BUSY;

    lcd44780_cmd_write(lcd, LCD44780_CMD_HOME);

    return E_NO_ERROR;
}

err_t lcd44780_entry_mode_set(lcd44780_t* lcd, lcd44780_cursor_move_t cur_move,
                                               lcd44780_display_shift_t dpy_shift)
{
    if(!lcd44780_wait(lcd)) return E_BUSY;

    uint8_t cmd = LCD44780_CMD_ENTRY_MODE;

    cmd |= cur_move << LCD44780_ENTRY_MODE_CURSOR_BIT;
    cmd |= dpy_shift << LCD44780_ENTRY_MODE_DISPLAY_BIT;

    lcd44780_cmd_write(lcd, cmd);

    return E_NO_ERROR;
}

err_t lcd44780_display_control(lcd44780_t* lcd, lcd44780_display_control_t dpy_ctl,
                                                lcd44780_cursor_control_t cur_ctl,
                                                lcd44780_cursor_blink_control_t cur_bl_ctl)
{
    if(!lcd44780_wait(lcd)) return E_BUSY;

    uint8_t cmd = LCD44780_CMD_DISPLAY_ON_OFF;

    cmd |= dpy_ctl << LCD44780_ON_OFF_DISPLAY_BIT;
    cmd |= cur_ctl << LCD44780_ON_OFF_CURSOR_BIT;
    cmd |= cur_bl_ctl << LCD44780_ON_OFF_CURSOR_BLINK_BIT;

    lcd44780_cmd_write(lcd, cmd);

    return E_NO_ERROR;
}

err_t lcd44780_shift_control(lcd44780_t* lcd, lcd44780_shift_mode_t mode,
                                              lcd44780_shift_direction_t dir)
{
    if(!lcd44780_wait(lcd)) return E_BUSY;

    uint8_t cmd = LCD44780_CMD_CURSOR_DISPLAY_SHIFT;

    cmd |= mode << LCD44780_SHIFT_DISPLAY_CURSOR_BIT;
    cmd |= dir << LCD44780_SHIFT_RIGHT_LEFT_BIT;

    lcd44780_cmd_write(lcd, cmd);

    return E_NO_ERROR;
}

err_t lcd44780_function_set(lcd44780_t* lcd, lcd44780_line_number_t line_num,
                                             lcd44780_font_type_t font_type)
{
    if(!lcd44780_wait(lcd)) return E_BUSY;

    uint8_t cmd = LCD44780_CMD_FUNCTION_SET;

    cmd |= lcd->data_bus_width << LCD44780_FUNCTION_SET_DATA_WIDTH_BIT;
    cmd |= line_num << LCD44780_FUNCTION_SET_LINE_NUMBER_BIT;
    cmd |= font_type << LCD44780_FUNCTION_SET_FONT_TYPE_BIT;

    lcd44780_cmd_write(lcd, cmd);

    return E_NO_ERROR;
}

err_t lcd44780_set_cgram(lcd44780_t* lcd, lcd44780_address_t addr)
{
    if(!lcd44780_wait(lcd)) return E_BUSY;

    uint8_t cmd = LCD44780_CMD_SET_CGRAM_ADDRESS;

    cmd |= addr & LCD44780_CGRAM_ADDRESS_MASK;

    lcd44780_cmd_write(lcd, cmd);

    return E_NO_ERROR;
}

err_t lcd44780_set_ddram(lcd44780_t* lcd, lcd44780_address_t addr)
{
    if(!lcd44780_wait(lcd)) return E_BUSY;

    uint8_t cmd = LCD44780_CMD_SET_DDRAM_ADDRESS;

    cmd |= addr & LCD44780_DDRAM_ADDRESS_MASK;

    lcd44780_cmd_write(lcd, cmd);

    return E_NO_ERROR;
}

err_t lcd44780_write_data(lcd44780_t* lcd, uint8_t data)
{
    if(!lcd44780_wait(lcd)) return E_BUSY;

    lcd44780_data_write(lcd, data);

    return E_NO_ERROR;
}

err_t lcd44780_read_data(lcd44780_t* lcd, uint8_t* data)
{
    if(data == NULL) return E_NULL_POINTER;
    if(!lcd44780_wait(lcd)) return E_BUSY;

    *data = lcd44780_data_read(lcd);

    return E_NO_ERROR;
}

err_t lcd44780_puts(lcd44780_t* lcd, const char* str)
{
    err_t err = E_NO_ERROR;

    while(*str){
        err = lcd44780_write_data(lcd, *str++);
        if(err != E_NO_ERROR) break;
    }

    return err;
}

