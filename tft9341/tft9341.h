/**
 * @file tft9341.h
 * Библиотека для работы с TFT на контроллере ILI9341.
 */

#ifndef TFT9341_H
#define	TFT9341_H

#include <stm32f10x.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "spi/spi.h"
#include "errors/errors.h"
#include "future/future.h"


//! Вспомогательный макрос для создания 16 битного цвета.
#define TFT9341_MAKE_RGB565_IMPL(r, g0, g1, b) ( (g0) | ((r) << 3) | ((b) << 8) | ((g1) << 13) )
//! Создаёт цвет RGB (16 бит) по заданным компонентам (0..255 на каждый канал).
#define TFT9341_MAKE_RGB565(r, g, b) TFT9341_MAKE_RGB565_IMPL(((r) >> 3), (((g) >> 2) & 0x7), ((g) >> 5), ((b) >> 3))


#define TFT9341_BUFFER_SIZE 14 
#define TFT9341_MESSAGES_COUNT 6

/**
 * Тип структуры дисплея.
 */
typedef struct _Tft9341 {
    //! Шина SPI.
    spi_bus_t* spi;
    //! Идентификатор передачи.
    spi_transfer_id_t transfer_id;
    //! Порт выбора ведомого.
    GPIO_TypeDef* ce_gpio;
    //! Пин выбора ведомого.
    uint16_t ce_pin;
    //! Порт выбора данных/команды.
    GPIO_TypeDef* dc_gpio;
    //! Пин выбора данных/команды.
    uint16_t dc_pin;
    //! Порт сброса.
    GPIO_TypeDef* rst_gpio;
    //! Пин сброса.
    uint16_t rst_pin;
    //! Будущее.
    future_t future;
    //! Буфер.
    uint8_t buffer[TFT9341_BUFFER_SIZE];
    //! Сообщения SPI.
    spi_message_t messages[TFT9341_MESSAGES_COUNT];
} tft9341_t;

/**
 * Тип структуры инициализации дисплея.
 */
typedef struct _Lcd8544_init {
    spi_bus_t*          spi; //!< Шина SPI.
    spi_transfer_id_t   transfer_id; //!< Идентификатор передачи.
    GPIO_TypeDef*       ce_gpio; //!< Порт выбора ведомого.
    uint16_t            ce_pin; //!< Пин выбора ведомого.
    GPIO_TypeDef*       dc_gpio; //!< Порт выбора данных/команды.
    uint16_t            dc_pin; //!< Пин выбора данных/команды.
    GPIO_TypeDef*       reset_gpio; //!< Порт сброса.
    uint16_t            reset_pin; //!< Пин сброса.
} tft9341_init_t;

//! Идентификатор передачи по-умолчанию.
#define TFT9341_DEFAULT_TRANSFER_ID 93


//! Размер пиксела.
//! 16 Бит, 2 байта.
#define TFT9341_PIXEL_SIZE_16BIT       2
//! 18 Бит, 3 байта.
#define TFT9341_PIXEL_SIZE_18BIT       3

//! Минимум байт на пиксел.
#define TFT9341_PIXEL_SIZE_MIN         TFT9341_PIXEL_SIZE_16BIT

//! Максимум байт на пиксел.
#define TFT9341_PIXEL_SIZE_MAX         TFT9341_PIXEL_SIZE_18BIT

//! Число пикселов.
#define TFT9341_PIXELS_COUNT           (320 * 240)

/**
 * Тип идентификатора экрана.
 */
typedef struct _Tft9341_Id {
    uint8_t id1; //!< Байт 1 идентификатора.
    uint8_t id2; //!< Байт 2 идентификатора.
    uint8_t id3; //!< Байт 3 идентификатора.
} tft9341_id_t;

//! Состояние бустера.
typedef enum _Tft9341_Booster_Status {
    TFT9341_BOOSTER_OFF = 0, //!< On.
    TFT9341_BOOSTER_ON = 1 //!< Off.
} tft9341_booster_status_t;

//! Порядок строк.
typedef enum _Tft9341_Row_Address_Order {
    TFT9341_ROW_TOP_TO_BOTTOM = 0, //!< Сверху вниз.
    TFT9341_ROW_BOTTOM_TO_TOP = 1 //!< Снизу вверх.
} tft9341_row_address_order_t;

//! Порядок столбцов.
typedef enum _Tft9341_Col_Address_Order {
    TFT9341_COL_LEFT_TO_RIGHT = 0, //!< Слева направо.
    TFT9341_COL_RIGHT_TO_LEFT = 1 //!< Справа налево.
} tft9341_col_address_order_t;

//! Подмена строк и столбцов.
typedef enum _Tft9341_Row_Col_Exchange {
    TFT9341_ROW_COL_NORMAL_MODE = 0, //!< Нормальный режим.
    TFT9341_ROW_COL_REVERSE_MODE = 1 //!< Реверсный режим.
} tft9341_row_col_exchange_t;

//! Вертикальное обновление.
typedef enum _Tft9341_Vertical_Refresh {
    TFT9341_REFRESH_TOP_TO_BOTTOM = 0, //!< Сверху вниз.
    TFT9341_REFRESH_BOTTOM_TO_TOP = 1 //!< Снизу вверх.
} tft9341_vertical_refresh_t;

//! Порядок цвета.
typedef enum _Tft9341_Color_Order {
    TFT9341_COLOR_ORDER_RGB = 0, //! RGB
    TFT9341_COLOR_ORDER_BGR = 1 //! BGR
} tft9341_color_order_t;

//! Горизонтальное обновление.
typedef enum _Tft9341_Horizontal_Refresh {
    TFT9341_REFRESH_LEFT_TO_RIGHT = 0, //!< Сверху вниз.
    TFT9341_REFRESH_RIGHT_TO_LEFT = 1 //!< Снизу вверх.
} tft9341_horizontal_refresh_t;

//! Формат пиксела интерфейса.
typedef enum _Tft9341_Pixel_Format {
    TFT9341_PIXEL_16BIT = 5, //!< Пиксел 16 бит.
    TFT9341_PIXEL_18BIT = 6 //!< Пиксел 18 бит.
} tft9341_pixel_format_t;

//! Режим IDLE.
typedef enum _Tft9341_Idle_Mode {
    TFT9341_IDLE_MODE_OFF = 0, //!< Off.
    TFT9341_IDLE_MODE_ON = 1 //!< On.
} tft9341_idle_mode_t;

//! Частичный режим.
typedef enum _Tft9341_Partial_Mode {
    TFT9341_PARTIAL_MODE_OFF = 0, //!< Off.
    TFT9341_PARTIAL_MODE_ON = 1 //!< On.
} tft9341_partial_mode_t;

//! Режим сна.
typedef enum _Tft9341_Sleep_Mode {
    TFT9341_SLEEP_MODE_IN = 0, //!< Off.
    TFT9341_SLEEP_MODE_OUT = 1 //!< On.
} tft9341_sleep_mode_t;

//! Нормальный режим.
typedef enum _Tft9341_Normal_Mode {
    TFT9341_NORMAL_MODE_OFF = 0, //!< Off.
    TFT9341_NORMAL_MODE_ON = 1 //!< On.
} tft9341_normal_mode_t;

//! Состояние экрана.
typedef enum _Tft9341_Display_Mode {
    TFT9341_DISPLAY_OFF = 0, //!< On.
    TFT9341_DISPLAY_ON = 1 //!< Off.
} tft9341_display_mode_t;

//! Tearing effect line. Яхз ВТФ.
typedef enum _Tft9341_Tearing_Effect_Line {
    TFT9341_TEARING_EFFECT_LINE_OFF, //!< Off.
    TFT9341_TEARING_EFFECT_LINE_ON //!< On.
} tft9341_tearing_effect_line_t;

//! Кривая гаммы.
typedef enum _Tft9341_Gamma_Curve {
    TFT9341_GAMMA_CURVE0 = 0, //!< GC0
    TFT9341_GAMMA_CURVE1 = 1, //!< GC1
    TFT9341_GAMMA_CURVE2 = 2, //!< GC2
    TFT9341_GAMMA_CURVE3 = 3 //!< GC3
} tft9341_gamma_curve_t;

//! Tearing effect line mode. Яхз ВТФ.
typedef enum _Tft9341_Tearing_Effect_Mode {
    TFT9341_TEARING_EFFECT_MODE1 = 0, //!< V-Blanking only.
    TFT9341_TEARING_EFFECT_MODE2 = 1 //!< Both H and V blanking.
} tft9341_tearing_effect_mode_t;

//! Контроль яркости.
typedef enum _Tft9341_Brightness_Control {
    TFT9341_BRIGHTNESS_CONTROL_OFF = 0, //!< Контроль яркости вкл.
    TFT9341_BRIGHTNESS_CONTROL_ON = 1 //!< Контроль яркости выкл.
} tft9341_brightness_control_t;

//! Затемнение экрана.
typedef enum _Tft9341_Display_Dimming {
    TFT9341_DISPLAY_DIMMING_OFF = 0, //!< Off.
    TFT9341_DISPLAY_DIMMING_ON = 1 //!< On.
} tft9341_display_dimming_t;

//! Состояние подсветки.
typedef enum _Tft9341_Backlight_Mode {
    TFT9341_BACKLIGHT_OFF = 0, //!< Off.
    TFT9341_BACKLIGHT_ON = 1 //!< On.
} tft9341_backlight_mode_t;

//! Настройка адаптивной регулировки яркости.
//! Content Adaptive Brightness Control.
typedef enum _Tft9341_CABC {
    TFT9341_CABC_OFF = 0, //!< Выкл.
    TFT9341_CABC_UI = 1, //!< Интерфейс пользователя.
    TFT9341_CABC_PICTURE = 2, //!< Статичное изображение.
    TFT9341_CABC_MOVING_IMAGE = 3 //!< Движущееся изображение.
} tft9341_cabc_t;

/**
 * Структура состояния экрана.
 */
typedef struct _Tft9341_Status {
    tft9341_booster_status_t booster_status; //!< Состояние бустера.
    tft9341_row_address_order_t row_address_order; //!< Порядок строк.
    tft9341_col_address_order_t col_address_order; //!< Порядок столбцов.
    tft9341_row_col_exchange_t row_col_exchange; //!< Подмена строк и столбцов.
    tft9341_vertical_refresh_t vertical_refresh; //!< Вертикальное обновление.
    tft9341_color_order_t color_order; //!< Порядок цвета.
    tft9341_horizontal_refresh_t horizontal_order; //!< Горизонтальное обновление.
    tft9341_pixel_format_t pixel_format; //!< Формат пиксела.
    tft9341_idle_mode_t idle_mode; //!< Режим IDLE.
    tft9341_partial_mode_t partial_mode; //!< Частичный режим.
    tft9341_sleep_mode_t sleep_mode; //!< Режим сна.
    tft9341_normal_mode_t normal_mode; //!< Нормальный режим.
    tft9341_display_mode_t display_mode; //!< Состояние экрана.
    tft9341_tearing_effect_line_t tearing_effect_line; //!< Tearing effect line.
    tft9341_gamma_curve_t gamma_curve; //!< Кривая гаммы.
    tft9341_tearing_effect_mode_t tearing_effect_mode; //!< Tearing effect line mode.
} tft9341_status_t;

/**
 * Структура режима питания экрана.
 */
typedef struct _Tft9341_Power_mode {
    tft9341_booster_status_t booster_status; //!< Состояние бустера.
    tft9341_idle_mode_t idle_mode; //!< Режим IDLE.
    tft9341_partial_mode_t partial_mode; //!< Частичный режим.
    tft9341_sleep_mode_t sleep_mode; //!< Режим сна.
    tft9341_normal_mode_t normal_mode; //!< Нормальный режим.
    tft9341_display_mode_t display_mode; //!< Состояние экрана.
} tft9341_power_mode_t;

/**
 * Структура настроек доступа к памяти.
 */
typedef struct _Tft9341_Mad_Ctl {
    tft9341_row_address_order_t row_address_order; //!< Порядок строк.
    tft9341_col_address_order_t col_address_order; //!< Порядок столбцов.
    tft9341_row_col_exchange_t row_col_exchange; //!< Реверсное отображение.
    tft9341_vertical_refresh_t vertical_refresh; //!< Вертикальное обновление.
    tft9341_color_order_t color_order; //!< Порядок цвета.
    tft9341_horizontal_refresh_t horizontal_refresh; //!< Горизонтальное обновление.
} tft9341_madctl_t;

/**
 * Структура формата пиксела экрана.
 */
typedef struct _Tft9341_Display_Pixel_Format {
    bool parallel_iface_6bit; //!< Шина 6 бит, 3 передачи.
    tft9341_pixel_format_t parallel_iface_format; //!< Формат пиксела параллельного интерфейса.
    tft9341_pixel_format_t mcu_iface_format; //!< Формат пиксела интерфейса микроконтроллера.
} tft9341_display_pixel_format_t;

/**
 * Структура контроля экрана.
 */
typedef struct _Tft9341_Display_Control {
    tft9341_brightness_control_t brightness_control; //!< Контроль яркости.
    tft9341_display_dimming_t display_dimming; //!< Затемнение экрана.
    tft9341_backlight_mode_t backlight_mode; //!< Состояние подсветки.
}tft9341_display_control_t;

/**
 * Каллбэк SPI.
 * @return true, если событие обработано, иначе false.
 */
extern bool tft9341_spi_callback(tft9341_t* tft);

/**
 * Инициализирует TFT.
 * @param tft TFT.
 * @param tft_init Структура инициализации.
 * @return Код ошибки.
 */
extern err_t tft9341_init(tft9341_t* tft, tft9341_init_t* tft_init);

/**
 * Получает флаг занятости TFT.
 * @param tft TFT.
 * @return Флаг занятости TFT.
 */
extern bool tft9341_busy(tft9341_t* tft);

/**
 * Получает код ошибки асинхронно выполненой операции.
 * @param tft TFT.
 * @return Код ошибки асинхронно выполненой операции.
 */
extern err_t tft9341_error(tft9341_t* tft);

/**
 * Ожидает завершения операции с TFT.
 * @param tft TFT.
 * @return Код ошибки операции.
 */
extern err_t tft9341_wait(tft9341_t* tft);

/**
 * Сбрасывает TFT.
 * @param tft TFT.
 */
extern void tft9341_reset(tft9341_t* tft);

/**
 * Ничего не делает.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_nop(tft9341_t* tft);

/**
 * Сбрасывает TFT командой.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_soft_reset(tft9341_t* tft);

/**
 * Читает идентификатор TFT.
 * @param tft TFT.
 * @param id Адрес переменной для записи идентификатора.
 * @return Код ошибки.
 */
extern err_t tft9341_read_id(tft9341_t* tft, tft9341_id_t* id);

/**
 * Читает статус TFT.
 * @param tft TFT.
 * @param status Статус.
 * @return Код ошибки.
 */
extern err_t tft9341_read_status(tft9341_t* tft, tft9341_status_t* status);

/**
 * Читает режим питания TFT.
 * @param tft TFT.
 * @param mode Режим питания.
 * @return Код ошибки.
 */
extern err_t tft9341_read_power_mode(tft9341_t* tft, tft9341_power_mode_t* mode);

/**
 * Читает настройки доступа к памяти.
 * @param tft Экран.
 * @param madctl Настройки доступа к памяти.
 * @return Код ошибки.
 */
extern err_t tft9341_read_madctl(tft9341_t* tft, tft9341_madctl_t* madctl);

/**
 * Читает формат пиксела экрана.
 * @param tft Экран.
 * @param format Формат пиксела.
 * @return Код ошибки.
 */
extern err_t tft9341_read_pixel_format(tft9341_t* tft, tft9341_display_pixel_format_t* format);

/**
 * Читает формат изображения экрана.
 * @param tft Экран.
 * @param format Формат изображения.
 * @return Код ошибки.
 */
extern err_t tft9341_read_image_format(tft9341_t* tft, tft9341_gamma_curve_t* format);

/**
 * Засыпает.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_sleep_in(tft9341_t* tft);

/**
 * Просыпается.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_sleep_out(tft9341_t* tft);

/**
 * Частичный режим.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_partial_mode(tft9341_t* tft);

/**
 * Нормальный режим.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_normal_mode(tft9341_t* tft);

/**
 * Выключает инверсию.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_inversion_off(tft9341_t* tft);

/**
 * Включает инверсию.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_inversion_on(tft9341_t* tft);

/**
 * Устанавливает гамму экрана.
 * @param tft Экран.
 * @param gamma Гамма.
 * @return Код ошибки.
 */
extern err_t tft9341_set_gamma(tft9341_t* tft, tft9341_gamma_curve_t gamma);

/**
 * Выключает дисплей.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_display_off(tft9341_t* tft);

/**
 * Включает дисплей.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_display_on(tft9341_t* tft);

/**
 * Устанавливает столбцы области памяти, к которой есть доступ.
 * @param tft TFT.
 * @param start Начальный столбец.
 * @param end Конечный столбец.
 * @return Код ошибки.
 */
extern err_t tft9341_set_column_address(tft9341_t* tft, uint16_t start, uint16_t end);

/**
 * Устанавливает страницы области памяти, к которой есть доступ.
 * @param tft TFT.
 * @param start Начальная страница.
 * @param end Конечная страница.
 * @return Код ошибки.
 */
extern err_t tft9341_set_page_address(tft9341_t* tft, uint16_t start, uint16_t end);

/**
 * Записывает данные.
 * Отправка данных асинхронна.
 * @param tft TFT.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
extern err_t tft9341_write(tft9341_t* tft, const void* data, size_t size);

/**
 * Команда начала записи данных.
 * Отправка асинхронна.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_begin_write(tft9341_t* tft);

/**
 * Читает данные.
 * Чтение данных асинхронно.
 * @param tft TFT.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
extern err_t tft9341_read(tft9341_t* tft, void* data, size_t size);

/**
 * Устанавливает устанавливает область строк частичного режима.
 * @param tft TFT.
 * @param start Начальная строка.
 * @param end Конечная строка.
 * @return Код ошибки.
 */
extern err_t tft9341_set_partial_area(tft9341_t* tft, uint16_t start, uint16_t end);

/**
 * Устанавливает настройки доступа к памяти.
 * @param tft Экран.
 * @param madctl Настройки доступа к памяти.
 * @return Код ошибки.
 */
extern err_t tft9341_set_madctl(tft9341_t* tft, const tft9341_madctl_t* madctl);

/**
 * Выключает режим ожидания.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_idle_off(tft9341_t* tft);

/**
 * Включает режим ожидания.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_idle_on(tft9341_t* tft);

/**
 * Устанавливает формат пиксела экрана.
 * @param tft Экран.
 * @param rgb_iface_format Формат пиксела RGB-интерфейса.
 * @param mcu_iface_format Формат пиксела MCU-интерфейса.
 * @return Код ошибки.
 */
extern err_t tft9341_set_pixel_format(tft9341_t* tft, tft9341_pixel_format_t rgb_iface_format, tft9341_pixel_format_t mcu_iface_format);

/**
 * Записывает данные с адреса последней записи.
 * Отправка данных асинхронна.
 * @param tft TFT.
 * @param data Данные.
 * @param size Размер данных.
 */
extern err_t tft9341_write_continue(tft9341_t* tft, const void* data, size_t size);

/**
 * Команда начала записи данных с адреса последней записи.
 * Отправка асинхронна.
 * @param tft TFT.
 * @return Код ошибки.
 */
extern err_t tft9341_begin_write_continue(tft9341_t* tft);

/**
 * Читает данные с адреса последнего чтения.
 * Чтение данных асинхронно.
 * @param tft TFT.
 * @param data Данные.
 * @param size Размер данных.
 */
extern err_t tft9341_read_continue(tft9341_t* tft, void* data, size_t size);

/**
 * Устанавливает яркость экрана.
 * @param tft Экран.
 * @param brightness Яркость.
 * @return Код ошибки.
 */
extern err_t tft9341_set_brightness(tft9341_t* tft, uint8_t brightness);

/**
 * Читает яркость экрана.
 * @param tft Экран.
 * @param brightness Яркость.
 * @return Код ошибки.
 */
extern err_t tft9341_read_brightness(tft9341_t* tft, uint8_t* brightness);

/**
 * Устанавливает настройки экрана.
 * @param tft TFT.
 * @param control Настройки экрана.
 * @return Код ошибки.
 */
extern err_t tft9341_set_display_control(tft9341_t* tft, const tft9341_display_control_t* control);

/**
 * Читает настройки экрана.
 * @param tft TFT.
 * @param control Настройки экрана.
 * @return Код ошибки.
 */
extern err_t tft9341_read_display_control(tft9341_t* tft, tft9341_display_control_t* control);

/**
 * Устанавливает адаптивный контроль яркости.
 * @param tft TFT.
 * @param cabc Адаптивный контроль яркости.
 * @return Код ошибки.
 */
extern err_t tft9341_set_cabc(tft9341_t* tft, tft9341_cabc_t cabc);

/**
 * Получает адаптивный контроль яркости.
 * @param tft TFT.
 * @param cabc Адаптивный контроль яркости.
 * @return Код ошибки.
 */
extern err_t tft9341_read_cabc(tft9341_t* tft, tft9341_cabc_t* cabc);

/**
 * Устанавливает минимум яркости для CABC.
 * @param tft TFT.
 * @param cabc_min Минимум яркости.
 * @return Код ошибки.
 */
extern err_t tft9341_set_cabc_min(tft9341_t* tft, uint8_t cabc_min);

/**
 * Читает минимум яркости для CABC.
 * @param tft TFT.
 * @param cabc_min Минимум яркости.
 * @return Код ошибки.
 */
extern err_t tft9341_read_cabc_min(tft9341_t* tft, uint8_t* cabc_min);

/**
 * Читает идентификатор ID1 TFT.
 * @param tft TFT.
 * @param id Адрес переменной для записи идентификатора.
 * @return Код ошибки.
 */
extern err_t tft9341_read_id1(tft9341_t* tft, uint8_t* id);

/**
 * Читает идентификатор ID2 TFT.
 * @param tft TFT.
 * @param id Адрес переменной для записи идентификатора.
 * @return Код ошибки.
 */
extern err_t tft9341_read_id2(tft9341_t* tft, uint8_t* id);

/**
 * Читает идентификатор ID3 TFT.
 * @param tft TFT.
 * @param id Адрес переменной для записи идентификатора.
 * @return Код ошибки.
 */
extern err_t tft9341_read_id3(tft9341_t* tft, uint8_t* id);

/**
 * Записывает данные в TFT без посыла команды.
 * Асинхронная операция.
 * @param tft TFT.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
extern err_t tft9341_data(tft9341_t* tft, const void* data, size_t size);

/**
 * Устанавливает пиксел на экране.
 * Асинхронная операция.
 * Память пиксела может быть освобождена или
 * использована сразу после возврата из функции.
 * @param tft TFT.
 * @param x Координата X.
 * @param y Координата Y.
 * @param pixel Данные пиксела.
 * @param size Размер пиксела в байтах.
 * @return Код ошибки.
 */
extern err_t tft9341_set_pixel(tft9341_t* tft, uint16_t x, uint16_t y, const void* pixel, size_t size);

/**
 * Записывает данные в регион на экране.
 * Асинхронная операция.
 * @param tft TFT.
 * @param x0 Координата X верхнего левого угла региона.
 * @param y0 Координата Y верхнего левого угла региона.
 * @param x1 Координата X правого нижнего угла региона.
 * @param y1 Координата Y правого нижнего угла региона.
 * @param data Данные.
 * @param size Размер данных в байтах.
 * @return Код ошибки.
 */
extern err_t tft9341_write_region(tft9341_t* tft, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const void* data, size_t size);

//147
//209

#endif	/* TFT9341_H */

