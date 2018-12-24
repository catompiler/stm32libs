/**
 * @file lcd44780.h Библиотека экрана на контроллере HD44780.
 */

#ifndef LCD44780_H_
#define LCD44780_H_

#include <stm32f10x.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "gpio/gpio.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


//! Ширина шины данных.
typedef enum _Lcd44780_Bus_Width {
    LCD44780_BUS_WIDTH_4BIT = 0, //!< 4 Бита.
    LCD44780_BUS_WIDTH_8BIT = 1  //!< 8 Бит.
} lcd44780_bus_width_t;

//! Перемещение курсора.
typedef enum _Lcd44780_Cursor_Move {
    LCD44780_CURSOR_MOVE_LEFT = 0, //!< Влево - декремент адреса.
    LCD44780_CURSOR_MOVE_RIGHT = 1  //!< Вправо - инкремент адреса.
} lcd44780_cursor_move_t;

//! Cмещение экрана.
typedef enum _Lcd44780_Display_Shift {
    LCD44780_DISPLAY_SHIFT_OFF = 0, //!< Смещение запрещено.
    LCD44780_DISPLAY_SHIFT_ON = 1 //!< Смещение разрешено.
} lcd44780_display_shift_t;

//! Контроль экрана.
typedef enum _Lcd44780_Display_Control {
    LCD44780_DISPLAY_OFF = 0, //!< Экран выключен.
    LCD44780_DISPLAY_ON = 1, //!< Экран включен.
} lcd44780_display_control_t;

//! Контроль курсора.
typedef enum _Lcd44780_Cursor_Control {
    LCD44780_CURSOR_OFF = 0, //!< Курсор выключен.
    LCD44780_CURSOR_ON = 1, //!< Курсор включен.
} lcd44780_cursor_control_t;

//! Контроль моргания курсора.
typedef enum _Lcd44780_Cursor_Blink_Control {
    LCD44780_CURSOR_BLINK_OFF = 0, //!< Моргание курсора выключено.
    LCD44780_CURSOR_BLINK_ON = 1, //!< Моргание курсора включено.
} lcd44780_cursor_blink_control_t;

//! Режим сдвига.
typedef enum _Lcd44780_Shift_Mode {
    LCD44780_SHIFT_CURSOR = 0, //!< Сдвигать курсор.
    LCD44780_SHIFT_DISPLAY = 1 //!< Сдвигать экран.
} lcd44780_shift_mode_t;

//! Направление сдвига.
typedef enum _Lcd44780_Shift_direction {
    LCD44780_SHIFT_DIRECTION_LEFT = 0, //!< Влево.
    LCD44780_SHIFT_DIRECTION_RIGHT = 1 //!< Вправо.
} lcd44780_shift_direction_t;

//! Число линий.
typedef enum _Lcd44780_Line_Number {
    LCD44780_LINE_NUMBER_1 = 0, //!< Одна линия.
    LCD44780_LINE_NUMBER_2 = 1 //!< Две линии.
} lcd44780_line_number_t;

//! Тип шрифта.
typedef enum _Lcd44780_Font_Type {
    LCD44780_FONT_TYPE_5X8 = 0, //!< Шрифт 5x8.
    LCD44780_FONT_TYPE_5X11 = 1 //!< Шрифт 5x11.
} lcd44780_font_type_t;

//! Тип адреса.
typedef uint8_t lcd44780_address_t;

//! Направление шины данных экрана.
typedef enum _Lcd44780_Bus_Direction {
    LCD44780_BUS_DIRECTION_OUT = 0, //!< Выход.
    LCD44780_BUS_DIRECTION_IN = 1, //!< Вход.
} lcd44780_bus_direction_t;

//! Каллбэк установки направления шины.
typedef void (*lcd44780_bus_set_direction_t)(lcd44780_bus_direction_t dir);

//! Структура инициализации экрана.
typedef struct _Lcd44780_Init {
    // Контрольные пины.
    GPIO_TypeDef* rs_gpio; //!< Порт RS.
    gpio_pin_t rs_pin; //!< Пин RS.
    GPIO_TypeDef* rw_gpio; //!< Порт RW.
    gpio_pin_t rw_pin; //!< Пин RW.
    GPIO_TypeDef* e_gpio; //!< Порт E.
    gpio_pin_t e_pin; //!< Пин E.
    // Данные.
    GPIO_TypeDef* data_gpio; //!< Порт данных.
    gpio_pin_t data_pins_offset; //!< Смещение пинов данных.
    lcd44780_bus_width_t data_bus_width; //!< Ширина шины данных.
    lcd44780_bus_set_direction_t set_bus_direction; //!< Каллбэк установки направления шины.
} lcd44780_init_t;



//! Структура экрана.
typedef struct _Lcd44780 {
    // Контрольные пины.
    GPIO_TypeDef* rs_gpio; //!< Порт RS.
    gpio_pin_t rs_pin; //!< Пин RS.
    GPIO_TypeDef* rw_gpio; //!< Порт RW.
    gpio_pin_t rw_pin; //!< Пин RW.
    GPIO_TypeDef* e_gpio; //!< Порт E.
    gpio_pin_t e_pin; //!< Пин E.
    // Данные.
    GPIO_TypeDef* data_gpio; //!< Порт данных.
    gpio_pin_t data_pins_offset; //!< Смещение пинов данных.
    gpio_pin_t data_pins_mask; //!< Маска пинов данных.
    lcd44780_bus_width_t data_bus_width; //!< Ширина шины данных.
    lcd44780_bus_set_direction_t set_bus_direction; //!< Каллбэк установки направления шины.
} lcd44780_t;


/**
 * Инициализирует экран.
 * @param lcd Экран.
 * @param init Структура инициализации экрана.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_init(lcd44780_t* lcd, lcd44780_init_t* init);

/**
 * Получает флаг занятости экрана.
 * @param lcd Экран.
 * @return Флаг занятости.
 */
EXTERN bool lcd44780_busy(lcd44780_t*lcd);

/**
 * Ожидает завершения занятости экрана.
 * @param lcd Экран.
 * @return Флаг завершения занятости экрана.
 */
EXTERN bool lcd44780_wait(lcd44780_t* lcd);

/**
 * Получает текущий адрес.
 * @param lcd Экран.
 * @return Адрес.
 */
EXTERN uint8_t lcd44780_address(lcd44780_t* lcd);

/**
 * Очищает LCD.
 * @param lcd Экран.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_clear(lcd44780_t* lcd);

/**
 * Возвращает курсор LCD домой.
 * @param lcd Экран.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_home(lcd44780_t* lcd);

/**
 * Устанавливает режимы смещения курсора/экрана.
 * @param lcd Экрана.
 * @param cur_move Режим смещения курсора.
 * @param dpy_shift Режим смещения экрана.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_entry_mode_set(lcd44780_t* lcd, lcd44780_cursor_move_t cur_move,
                                                      lcd44780_display_shift_t dpy_shift);

/**
 * Устанавливает включенность экрана, курсора и мигания курсора.
 * @param lcd Экран.
 * @param dpy_ctl Включения экрана.
 * @param cur_ctl Включения курсора.
 * @param cur_bl_ctl Включения моргания курсора.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_display_control(lcd44780_t* lcd, lcd44780_display_control_t dpy_ctl,
                                                       lcd44780_cursor_control_t cur_ctl,
                                                       lcd44780_cursor_blink_control_t cur_bl_ctl);

/**
 * Устанавливает режим смещение экрана/курсора
 * @param lcd Экран.
 * @param mode Режим смещения.
 * @param dir Направление смещения.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_shift_control(lcd44780_t* lcd, lcd44780_shift_mode_t mode,
                                                     lcd44780_shift_direction_t dir);

/**
 * Задаёт конфигурацию экрана.
 * @param lcd Экран.
 * @param line_num Число линий.
 * @param font_type Тип шрифта.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_function_set(lcd44780_t* lcd, lcd44780_line_number_t line_num,
                                                    lcd44780_font_type_t font_type);

/**
 * Устанавливает курсор на CGRAM на заданный адрес.
 * @param lcd Экран.
 * @param addr Адрес.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_set_cgram(lcd44780_t* lcd, lcd44780_address_t addr);

/**
 * Устанавливает курсор на DDRAM на заданный адрес.
 * @param lcd Экран.
 * @param addr Адрес.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_set_ddram(lcd44780_t* lcd, lcd44780_address_t addr);

/**
 * Записывает данные в экран.
 * @param lcd Экран.
 * @param data Данные.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_write_data(lcd44780_t* lcd, uint8_t data);

/**
 * Считывает данные из экрана.
 * @param lcd Экран.
 * @param data Данные.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_read_data(lcd44780_t* lcd, uint8_t* data);

/**
 * Записывает символ в экран.
 * @param lcd Экран.
 * @param c Символ.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t lcd44780_putc(lcd44780_t* lcd, char c)
{
    return lcd44780_write_data(lcd, (uint8_t)c);
}

/**
 * Записывает строку в экран.
 * @param lcd Экран.
 * @param str Строка.
 * @return Код ошибки.
 */
EXTERN err_t lcd44780_puts(lcd44780_t* lcd, const char* str);

#endif /* LCD44780_H_ */
