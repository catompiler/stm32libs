/**
 * @file lcd12864.h
 * Библиотека для работы с графическим (128x64) LCD на контроллере, совместимом с KS0108.
 */

#ifndef LCD12864_H
#define	LCD12864_H

#include <stm32f10x.h>
#include "errors/errors.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "defs/defs.h"

//! Размер экрана.
//! Ширина.
#define LCD12864_WIDTH   128
//! Высота.
#define LCD12864_HEIGHT  64
//! Высота в страницах.
#define LCD12864_PAGES_HEIGHT 8
//! Размер в байтах.
#define LCD12864_SIZE_BYTES (128 * 64 / 8)

//! Максимальные значения адресации.
//! Максимальный номер страницы.
#define LCD12864_PAGE_MAX        7
//! Максимальный адрес страницы.
#define LCD12864_ADDRESS_MAX     127

/**
 * Структура LCD.
 */
typedef struct _Lcd12864 {
    GPIO_TypeDef* gpio_reset;  //!< Порт пина RESET.
    uint16_t gpio_pin_reset;   //!< Пин RESET.
    GPIO_TypeDef* gpio_cs1;    //!< Порт пина CS1.
    uint16_t gpio_pin_cs1;     //!< Пин CS1.
    GPIO_TypeDef* gpio_cs2;    //!< Порт пина CS2.
    uint16_t gpio_pin_cs2;     //!< Пин CS2.
    GPIO_TypeDef* gpio_e;      //!< Порт пина E.
    uint16_t gpio_pin_e;       //!< Пин E.
    GPIO_TypeDef* gpio_rw;     //!< Порт пина RW.
    uint16_t gpio_pin_rw;      //!< Пин RW.
    GPIO_TypeDef* gpio_dc;     //!< Порт пина DC.
    uint16_t gpio_pin_dc;      //!< Пин DC.
    GPIO_TypeDef* gpio_data;   //!< Порт данных.
    uint32_t gpio_data_offset; //!< Смешение начала шины данных в порту.
    uint32_t gpio_data_mask;   //!< Маска в порту шины данных.
    uint32_t gpio_data_crl_mask_inv;//!< Инверсная маска в CRL полей шины данных.
    uint32_t gpio_data_crl_in;  //!< Значения полей в CRL шины данных для входа.
    uint32_t gpio_data_crl_out; //!< Значения полей в CRL шины данных для выхода.
    uint32_t gpio_data_crh_mask_inv;//!< Инверсная маска в CRH полей шины данных.
    uint32_t gpio_data_crh_in;  //!< Значения полей в CRH шины данных для входа.
    uint32_t gpio_data_crh_out; //!< Значения полей в CRH шины данных для выхода.
} lcd12864_t;

/**
 * Структура инициализации LCD.
 */
typedef struct _Lcd12864_Init {
    GPIO_TypeDef* gpio_reset;  //!< Порт пина RESET.
    uint16_t gpio_pin_reset;   //!< Пин RESET.
    GPIO_TypeDef* gpio_cs1;    //!< Порт пина CS1.
    uint16_t gpio_pin_cs1;     //!< Пин CS1.
    GPIO_TypeDef* gpio_cs2;    //!< Порт пина CS2.
    uint16_t gpio_pin_cs2;     //!< Пин CS2.
    GPIO_TypeDef* gpio_e;      //!< Порт пина E.
    uint16_t gpio_pin_e;       //!< Пин E.
    GPIO_TypeDef* gpio_rw;     //!< Порт пина RW.
    uint16_t gpio_pin_rw;      //!< Пин RW.
    GPIO_TypeDef* gpio_dc;     //!< Порт пина DC.
    uint16_t gpio_pin_dc;      //!< Пин DC.
    GPIO_TypeDef* gpio_data;   //!< Порт данных.
    uint32_t gpio_data_offset; //!< Смешение начала шины данных в порту.
} lcd12864_init_t;


/**
 * Инициализирует LCD и все нужные пины.
 * @param lcd LCD.
 * @param lcd_init Структура инициализации LCD.
 * @return Код ошибки.
 */
EXTERN err_t lcd12864_init(lcd12864_t* lcd, lcd12864_init_t* lcd_init);

/**
 * Сбрасывает LCD.
 * @param lcd LCD.
 * @return Код ошибки.
 */
EXTERN err_t lcd12864_reset(lcd12864_t* lcd);

/**
 * Проверяет занятость LCD.
 * @param lcd LCD.
 * @return флаг занятости LCD.
 */
EXTERN bool lcd12864_busy(lcd12864_t* lcd);

/**
 * Ждёт сброса флага занятости LCD.
 * @param lcd LCD.
 * @return флаг освобождения LCD.
 */
EXTERN bool lcd12864_wait(lcd12864_t* lcd);

/**
 * Устанавливает включенность дисплея, курсора и мигания курсора.
 * @param lcd LCD.
 * @param display_on флаг включения дисплея.
 */
EXTERN err_t lcd12864_control(lcd12864_t* lcd, bool display_on);

/**
 * Устанавливает стартовую линию LCD.
 * @param lcd LCD.
 * @param start_line стартовая линия.
 * @return Код ошибки.
 */
EXTERN err_t lcd12864_set_start_line(lcd12864_t* lcd, uint8_t start_line);

/**
 * Очищает LCD.
 * @param lcd LCD.
 * @return Код ошибки.
 */
EXTERN err_t lcd12864_clear(lcd12864_t* lcd);

/**
 * Записывает данные в LCD.
 * @param lcd LCD.
 * @param data Данные.
 * @param count Размер данных.
 * @return Код ошибки.
 */
EXTERN err_t lcd12864_write(lcd12864_t* lcd, const uint8_t* data, size_t count);

#endif	/* LCD12864_H */

