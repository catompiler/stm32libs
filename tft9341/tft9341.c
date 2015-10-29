#include "tft9341.h"
#include "utils/utils.h"
#include "utils/delay.h"
#include "bits/bits.h"
#include "defs/defs.h"
#include <string.h>
//#include <stdio.h>

//extern void led_on(uint16_t led_pin);
//#define TFT9341_GET_MEM_DEBUG

//! Продолжительность ресета.
#define TFT9341_RESET_TIME_US  15
#define TFT9341_RESET_WAIT_TIME_MS  150
//#define TFT9341_SLEEP_OUT_WAIT_TIME_MS  10


//! Максимальные значения параметров
#define TFT9341__MAX     TFT9341_

//! Команды и их параметры.
//! Размер команды.
#define TFT9341_CMD_SIZE    0x1
//! Размер мусорных данных при приёме.
#define TFT9341_TRASH_SIZE    0x1
//! NOP, он и в Африке NOP.
#define TFT9341_CMD_NOP                     0x0
//! Программный сброс.
#define TFT9341_CMD_SOFT_RESET              0x1
//! Чтение идентификатора.
#define TFT9341_CMD_READ_ID                 0x4
#define TFT9341_RD_ID_DATA_SIZE             4
#define TFT9341_RD_ID_ID1_BYTE              1
#define TFT9341_RD_ID_ID2_BYTE              2
#define TFT9341_RD_ID_ID3_BYTE              3
//! Чтение состояния.
#define TFT9341_CMD_READ_DISPLAY_STATUS         0x9
#define TFT9341_RD_DS_DATA_SIZE                 5
#define TFT9341_RD_DS_BOOSTER_STATUS_OFFSET     31
#define TFT9341_RD_DS_BOOSTER_STATUS_MASK       1
#define TFT9341_RD_DS_ROW_ADDR_ORDER_OFFSET     30
#define TFT9341_RD_DS_ROW_ADDR_ORDER_MASK       1
#define TFT9341_RD_DS_COL_ADDR_ORDER_OFFSET     29
#define TFT9341_RD_DS_COL_ADDR_ORDER_MASK       1
#define TFT9341_RD_DS_ROW_COL_EXCH_OFFSET       28
#define TFT9341_RD_DS_ROW_COL_EXCH_MASK         1
#define TFT9341_RD_DS_VERTICAL_REFRESH_OFFSET   27
#define TFT9341_RD_DS_VERTICAL_REFRESH_MASK     1
#define TFT9341_RD_DS_COLOR_ORDER_OFFSET        26
#define TFT9341_RD_DS_COLOR_ORDER_MASK          1
#define TFT9341_RD_DS_HORIZONTAL_REFRESH_OFFSET 25
#define TFT9341_RD_DS_HORIZONTAL_REFRESH_MASK   1
#define TFT9341_RD_DS_PIXEL_FORMAT_OFFSET       20
#define TFT9341_RD_DS_PIXEL_FORMAT_MASK         7
#define TFT9341_RD_DS_IDLE_MODE_OFFSET          19
#define TFT9341_RD_DS_IDLE_MODE_MASK            1
#define TFT9341_RD_DS_PARTIAL_MODE_OFFSET       18
#define TFT9341_RD_DS_PARTIAL_MODE_MASK         1
#define TFT9341_RD_DS_SLEEP_MODE_OFFSET         17
#define TFT9341_RD_DS_SLEEP_MODE_MASK           1
#define TFT9341_RD_DS_NORMAL_MODE_OFFSET        16
#define TFT9341_RD_DS_NORMAL_MODE_MASK          1
#define TFT9341_RD_DS_DISPLAY_MODE_OFFSET       10
#define TFT9341_RD_DS_DISPLAY_MODE_MASK         1
#define TFT9341_RD_DS_TEARING_EFFECT_OFFSET     9
#define TFT9341_RD_DS_TEARING_EFFECT_MASK       1
#define TFT9341_RD_DS_GAMMA_CURVE_OFFSET        6
#define TFT9341_RD_DS_GAMMA_CURVE_MASK          7
#define TFT9341_RD_DS_TEARING_MODE_OFFSET       5
#define TFT9341_RD_DS_TEARING_MODE_MASK         1

//! Чтение состояния питания.
#define TFT9341_CMD_READ_POWER_MODE             0xa
#define TFT9341_RD_PM_DATA_SIZE                 2
#define TFT9341_RD_PM_BOOSTER_STATUS_OFFSET     7
#define TFT9341_RD_PM_BOOSTER_STATUS_MASK       1
#define TFT9341_RD_PM_IDLE_MODE_OFFSET          6
#define TFT9341_RD_PM_IDLE_MODE_MASK            1
#define TFT9341_RD_PM_PARTIAL_MODE_OFFSET       5
#define TFT9341_RD_PM_PARTIAL_MODE_MASK         1
#define TFT9341_RD_PM_SLEEP_MODE_OFFSET         4
#define TFT9341_RD_PM_SLEEP_MODE_MASK           1
#define TFT9341_RD_PM_NORMAL_MODE_OFFSET        3
#define TFT9341_RD_PM_NORMAL_MODE_MASK          1
#define TFT9341_RD_PM_DISPLAY_MODE_OFFSET       2
#define TFT9341_RD_PM_DISPLAY_MODE_MASK         1

//! Чтение настройки доступа к памяти.
#define TFT9341_CMD_READ_MADCTL                 0xb
#define TFT9341_RD_MC_DATA_SIZE                 2
#define TFT9341_RD_MC_ROW_ADDR_ORDER_OFFSET     7
#define TFT9341_RD_MC_ROW_ADDR_ORDER_MASK       1
#define TFT9341_RD_MC_COL_ADDR_ORDER_OFFSET     6
#define TFT9341_RD_MC_COL_ADDR_ORDER_MASK       1
#define TFT9341_RD_MC_ROW_COL_EXCH_OFFSET       5
#define TFT9341_RD_MC_ROW_COL_EXCH_MASK         1
#define TFT9341_RD_MC_VERTICAL_REFRESH_OFFSET   4
#define TFT9341_RD_MC_VERTICAL_REFRESH_MASK     1
#define TFT9341_RD_MC_COLOR_ORDER_OFFSET        3
#define TFT9341_RD_MC_COLOR_ORDER_MASK          1
#define TFT9341_RD_MC_HORIZONTAL_REFRESH_OFFSET 2
#define TFT9341_RD_MC_HORIZONTAL_REFRESH_MASK   1

//! Чтение формата пиксела экрана.
#define TFT9341_CMD_READ_DISPLAY_PIXEL_FORMAT   0xc
#define TFT9341_RD_PF_DATA_SIZE                 2
#define TFT9341_RD_PF_RIM_OFFSET                7
#define TFT9341_RD_PF_RIM_MASK                  1
#define TFT9341_RD_PF_DPI_OFFSET                4
#define TFT9341_RD_PF_DPI_MASK                  7
#define TFT9341_RD_PF_DBI_OFFSET                0
#define TFT9341_RD_PF_DBI_MASK                  7

//! Чтение формата изображения экрана.
#define TFT9341_CMD_READ_DISPLAY_IMAGE_FORMAT   0xd
#define TFT9341_RD_IF_DATA_SIZE                 2
#define TFT9341_RD_IF_D_OFFSET                  0
#define TFT9341_RD_IF_D_MASK                    7

//! Вход в сон.
#define TFT9341_CMD_SLEEP_IN                    0x10

//! Выход из сна.
#define TFT9341_CMD_SLEEP_OUT                   0x11

//! Частичный режим вкл.
#define TFT9341_CMD_PARTIAL_MODE_ON             0x12

//! Нормальный режим вкл.
#define TFT9341_CMD_NORMAL_MODE_ON              0x13

//! Инверсия выкл.
#define TFT9341_CMD_INVERSION_OFF               0x20

//! Инверсия вкл.
#define TFT9341_CMD_INVERSION_ON                0x21

//! Установка гаммы экрана.
#define TFT9341_CMD_WRITE_DISPLAY_GAMMA         0x26
#define TFT9341_WR_DG_DATA_SIZE                 1

//! Дисплей выкл.
#define TFT9341_CMD_DISPLAY_OFF                 0x28

//! Дисплей вкл.
#define TFT9341_CMD_DISPLAY_ON                  0x29

//! Установка столбцов разрешённой области.
#define TFT9341_CMD_WRITE_COL_ADDRESS           0x2a
#define TFT9341_WR_CA_DATA_SIZE                 4

//! Установка строк разрешённой области.
#define TFT9341_CMD_WRITE_PAGE_ADDRESS          0x2b
#define TFT9341_WR_PGA_DATA_SIZE                 4

//! Запись памяти.
#define TFT9341_CMD_WRITE_MEMORY                0x2c

//! Запись памяти.
#define TFT9341_CMD_READ_MEMORY                 0x2e

//! Установка области частичного режима.
#define TFT9341_CMD_WRITE_PARTIAL_AREA          0x30
#define TFT9341_WR_PLA_DATA_SIZE                4

//! Запись настроек доступа к памяти.
#define TFT9341_CMD_WRITE_MADCTL                0x36
#define TFT9341_WR_MC_DATA_SIZE                 1
#define TFT9341_WR_MC_ROW_ADDR_ORDER_OFFSET     7
#define TFT9341_WR_MC_ROW_ADDR_ORDER_MASK       1
#define TFT9341_WR_MC_COL_ADDR_ORDER_OFFSET     6
#define TFT9341_WR_MC_COL_ADDR_ORDER_MASK       1
#define TFT9341_WR_MC_ROW_COL_EXCH_OFFSET       5
#define TFT9341_WR_MC_ROW_COL_EXCH_MASK         1
#define TFT9341_WR_MC_VERTICAL_REFRESH_OFFSET   4
#define TFT9341_WR_MC_VERTICAL_REFRESH_MASK     1
#define TFT9341_WR_MC_COLOR_ORDER_OFFSET        3
#define TFT9341_WR_MC_COLOR_ORDER_MASK          1
#define TFT9341_WR_MC_HORIZONTAL_REFRESH_OFFSET 2
#define TFT9341_WR_MC_HORIZONTAL_REFRESH_MASK   1

//! IDLE выкл.
#define TFT9341_CMD_IDLE_OFF                    0x38

//! IDLE вкл.
#define TFT9341_CMD_IDLE_ON                     0x39

//! Чтение формата пиксела экрана.
#define TFT9341_CMD_WRITE_DISPLAY_PIXEL_FORMAT  0x3a
#define TFT9341_WR_PF_DATA_SIZE                 1
#define TFT9341_WR_PF_DPI_OFFSET                4
#define TFT9341_WR_PF_DPI_MASK                  7
#define TFT9341_WR_PF_DBI_OFFSET                0
#define TFT9341_WR_PF_DBI_MASK                  7

//! Запись памяти.
#define TFT9341_CMD_WRITE_MEMORY_CONTINUE       0x3c

//! Запись памяти.
#define TFT9341_CMD_READ_MEMORY_CONTINUE        0x3e

//! Установка яркости.
#define TFT9341_CMD_WRITE_BRIGHTNESS            0x51
#define TFT9341_WR_BN_DATA_SIZE                 1

//! Чтение яркости.
#define TFT9341_CMD_READ_BRIGHTNESS             0x52
#define TFT9341_RD_BN_DATA_SIZE                 2

//! Запись настроек экрана.
#define TFT9341_CMD_WRITE_DISPLAY_CONTROL       0x53
#define TFT9341_WR_DC_DATA_SIZE                 1
#define TFT9341_WR_DC_BRIGHTNESS_CONTROL_OFFSET 5
#define TFT9341_WR_DC_BRIGHTNESS_CONTROL_MASK   1
#define TFT9341_WR_DC_DISPLAY_DIMMING_OFFSET    3
#define TFT9341_WR_DC_DISPLAY_DIMMING_MASK      1
#define TFT9341_WR_DC_BACKLIGHT_OFFSET          2
#define TFT9341_WR_DC_BACKLIGHT_MASK            1

//! Чтение настроек экрана.
#define TFT9341_CMD_READ_DISPLAY_CONTROL        0x54
#define TFT9341_RD_DC_DATA_SIZE                 2
#define TFT9341_RD_DC_BRIGHTNESS_CONTROL_OFFSET 5
#define TFT9341_RD_DC_BRIGHTNESS_CONTROL_MASK   1
#define TFT9341_RD_DC_DISPLAY_DIMMING_OFFSET    3
#define TFT9341_RD_DC_DISPLAY_DIMMING_MASK      1
#define TFT9341_RD_DC_BACKLIGHT_OFFSET          2
#define TFT9341_RD_DC_BACKLIGHT_MASK            1

//! Установка гаммы экрана.
#define TFT9341_CMD_WRITE_CABC                  0x55
#define TFT9341_WR_CABC_DATA_SIZE               1
#define TFT9341_WR_CABC_C_OFFSET                0
#define TFT9341_WR_CABC_C_MASK                  3

//! Чтение формата изображения экрана.
#define TFT9341_CMD_READ_CABC                   0x56
#define TFT9341_RD_CABC_DATA_SIZE               2
#define TFT9341_RD_CABC_C_OFFSET                0
#define TFT9341_RD_CABC_C_MASK                  3

//! Установка минимума яркости CABC.
#define TFT9341_CMD_WRITE_CABC_MIN              0x5e
#define TFT9341_WR_CMB_DATA_SIZE                1

//! Чтение минимума яркости CABC.
#define TFT9341_CMD_READ_CABC_MIN               0x5f
#define TFT9341_RD_CMB_DATA_SIZE                2

//! Чтение ID1.
#define TFT9341_CMD_READ_ID1                    0xda
#define TFT9341_RD_ID1_DATA_SIZE                2

//! Чтение ID2.
#define TFT9341_CMD_READ_ID2                    0xdb
#define TFT9341_RD_ID2_DATA_SIZE                2

//! Чтение ID3.
#define TFT9341_CMD_READ_ID3                    0xdc
#define TFT9341_RD_ID3_DATA_SIZE                2



/**
 * Выставляет значения управляющих пинов для начала обмена данными с TFT.
 * @param tft TFT.
 * @param is_data Флаг данные/команда.
 */
ALWAYS_INLINE static void tft9341_pins_begin(tft9341_t* tft, bool is_data)
{
    if(is_data){
        GPIO_SetBits(tft->dc_gpio, tft->dc_pin);
    }else{
        GPIO_ResetBits(tft->dc_gpio, tft->dc_pin);
    }
    GPIO_ResetBits(tft->ce_gpio, tft->ce_pin);
}

/**
 * Выставлят значения управляющих пинов для окончания обмена данными с TFT.
 * @param tft TFT.
 */
ALWAYS_INLINE static void tft9341_pins_end(tft9341_t* tft)
{
    GPIO_SetBits(tft->ce_gpio, tft->ce_pin);
    GPIO_SetBits(tft->dc_gpio, tft->dc_pin);
}

/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool tft9341_wait_current_op(tft9341_t* tft)
{
    // Если шина занята, да ещё и не нами - возврат ошибки занятости.
    if(spi_bus_busy(tft->spi) && spi_bus_transfer_id(tft->spi) != tft->transfer_id) return false;
    
    future_wait(&tft->future);
    
    return true;
}

/**
 * Начинает обмен данными с TFT.
 * @param tft TFT.
 * @param is_data Флаг данные/команда.
 */
static void tft9341_begin(tft9341_t* tft, bool is_data)
{
    future_set_result(&tft->future, int_to_pvoid(E_NO_ERROR));
    
    future_start(&tft->future);
    
    tft9341_pins_begin(tft, is_data);
}

/**
 * Завершает обмен данными с TFT.
 * @param tft TFT.
 * @param err Код ошибки.
 */
static void tft9341_end(tft9341_t* tft, err_t err)
{
    future_finish(&tft->future, int_to_pvoid(err));
    
    tft9341_pins_end(tft);
}

/**
 * Каллбэк сообщения команды.
 * Устанавливает пин D/C на команду.
 * @param message Сообщение SPI.
 */
static void tft9341_cmd_message_start(spi_message_t* message)
{
    //printf("tft9341_cmd_message_start\r\n");
    tft9341_t* tft = (tft9341_t*)message->sender_data;
    if(tft != NULL){
        //GPIO_SetBits(tft->ce_gpio, tft->ce_pin);
        GPIO_ResetBits(tft->dc_gpio, tft->dc_pin);
        //GPIO_ResetBits(tft->ce_gpio, tft->ce_pin);
    }
}

/**
 * Каллбэк сообщения команды.
 * Устанавливает пин D/C на данные.
 * @param message Сообщение SPI.
 */
static void tft9341_cmd_message_end(spi_message_t* message)
{
    //printf("tft9341_cmd_message_end\r\n");
    tft9341_t* tft = (tft9341_t*)message->sender_data;
    if(tft != NULL){
        //GPIO_SetBits(tft->ce_gpio, tft->ce_pin);
        GPIO_SetBits(tft->dc_gpio, tft->dc_pin);
        //GPIO_ResetBits(tft->ce_gpio, tft->ce_pin);
    }
}

/**
 * Отправляет данные в TFT.
 * Асинхронно.
 * @param tft TFT.
 * @param is_data Флаг данные/команда.
 * @param messages_count Число сообщений.
 * @return Код ошибки.
 */
static err_t tft9341_transfer(tft9341_t* tft, bool is_data, size_t messages_count)
{
    if(!spi_bus_set_transfer_id(tft->spi, tft->transfer_id)) return E_BUSY;
    
    tft9341_begin(tft, is_data);
    
    err_t err = spi_bus_transfer(tft->spi, tft->messages, messages_count);
    
    if(err != E_NO_ERROR){
        tft9341_end(tft, err);
        return err;
    }
    
    return E_NO_ERROR;
}

bool tft9341_spi_callback(tft9341_t* tft)
{
    if(spi_bus_transfer_id(tft->spi) != tft->transfer_id) return false;
    
    tft9341_end(tft, spi_bus_status(tft->spi) != SPI_STATUS_TRANSFERED ? E_IO_ERROR : E_NO_ERROR);
    
    return true;
}

err_t tft9341_init(tft9341_t* tft, tft9341_init_t* tft_init)
{
    // Clear.
    memset(tft, 0x0, sizeof(tft9341_t));
    
    // SPI.
    tft->spi = tft_init->spi;
    
    // CE.
    tft->ce_gpio = tft_init->ce_gpio;
    tft->ce_pin = tft_init->ce_pin;
    
    // D/C.
    tft->dc_gpio = tft_init->dc_gpio;
    tft->dc_pin = tft_init->dc_pin;
    
    // RESET.
    tft->rst_gpio = tft_init->reset_gpio;
    tft->rst_pin = tft_init->reset_pin;
    
    future_init(&tft->future);
    
    // Значения по-умолчанию для этих пинов.
    tft9341_pins_end(tft);
    
    // Высокий уровень для пина сброса.
    GPIO_SetBits(tft->rst_gpio, tft->rst_pin);
    // Сбросим дисплей.
    //tft9341_reset(tft);
    
    // Идентификатор передачи.
    tft->transfer_id = tft_init->transfer_id;
    
    return E_NO_ERROR;
}

bool tft9341_busy(tft9341_t* tft)
{
    return future_running(&tft->future);
}

err_t tft9341_error(tft9341_t* tft)
{
    return pvoid_to_int(err_t, future_result(&tft->future));
}

err_t tft9341_wait(tft9341_t* tft)
{
    future_wait(&tft->future);
    return pvoid_to_int(err_t, future_result(&tft->future));
}

void tft9341_reset(tft9341_t* tft)
{
    //wait
    future_wait(&tft->future);
    
    GPIO_ResetBits(tft->rst_gpio, tft->rst_pin);
    delay_us(TFT9341_RESET_TIME_US);
    GPIO_SetBits(tft->rst_gpio, tft->rst_pin);
    
    delay_ms(TFT9341_RESET_WAIT_TIME_MS);
}

/**
 * Получает указатель на данные в буфере нужного размера.
 * @param tft TFT.
 * @param size Необходимый размер.
 * @param index Значение предыдущего индекса.
 * @return Указатель на данные в бефере.
 */
ALWAYS_INLINE static uint8_t* tft9341_get_buffer(tft9341_t* tft, size_t size, size_t* index)
{
#ifdef TFT9341_GET_MEM_DEBUG
    if(index == NULL) return NULL;
#endif
    
    size_t i = *index;
    
#ifdef TFT9341_GET_MEM_DEBUG
    if(i + size > TFT9341_BUFFER_SIZE) return NULL;
#endif
    
    (*index) += size;
    
    return &tft->buffer[i];
}

/**
 * Получает указатель на сообщение.
 * @param tft TFT.
 * @param index Значение предыдущего индекса.
 * @return Указатель на данные в бефере.
 */
ALWAYS_INLINE static spi_message_t* tft9341_get_message(tft9341_t* tft, size_t* index)
{
#ifdef TFT9341_GET_MEM_DEBUG
    if(index == NULL) return NULL;
#endif
    
    size_t i = *index;
    
#ifdef TFT9341_GET_MEM_DEBUG
    if(i >= TFT9341_MESSAGES_COUNT) return NULL;
#endif
    
    (*index) ++;
    
    return &tft->messages[i];
}

err_t tft9341_nop(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_NOP;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_soft_reset(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_SOFT_RESET;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}


/*err_t tft9341_soft_reset(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    tft->buffer[0] = TFT9341_CMD_SOFT_RESET;
    err = spi_message_init(&tft->messages[0], SPI_WRITE, &tft->buffer[0], NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}*/

err_t tft9341_read_id(tft9341_t* tft, tft9341_id_t* id)
{
    if(id == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_ID_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_ID;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_ID_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    id->id1 = data_buf[1];
    id->id2 = data_buf[2];
    id->id3 = data_buf[3];
    
    return E_NO_ERROR;
}

err_t tft9341_read_status(tft9341_t* tft, tft9341_status_t* status)
{
    if(status == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_DS_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_DISPLAY_STATUS;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_DS_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    uint32_t data = *((uint32_t*)&data_buf[1]);
    
    status->booster_status = BIT_VALUE_MASK(data, TFT9341_RD_DS_BOOSTER_STATUS_OFFSET, TFT9341_RD_DS_BOOSTER_STATUS_MASK);
    status->row_address_order = BIT_VALUE_MASK(data, TFT9341_RD_DS_ROW_ADDR_ORDER_OFFSET, TFT9341_RD_DS_ROW_ADDR_ORDER_MASK);
    status->col_address_order = BIT_VALUE_MASK(data, TFT9341_RD_DS_COL_ADDR_ORDER_OFFSET, TFT9341_RD_DS_COL_ADDR_ORDER_MASK);
    status->row_col_exchange = BIT_VALUE_MASK(data, TFT9341_RD_DS_ROW_COL_EXCH_OFFSET, TFT9341_RD_DS_ROW_COL_EXCH_MASK);
    status->vertical_refresh = BIT_VALUE_MASK(data, TFT9341_RD_DS_VERTICAL_REFRESH_OFFSET, TFT9341_RD_DS_VERTICAL_REFRESH_MASK);
    status->color_order = BIT_VALUE_MASK(data, TFT9341_RD_DS_COLOR_ORDER_OFFSET, TFT9341_RD_DS_COLOR_ORDER_MASK);
    status->horizontal_order = BIT_VALUE_MASK(data, TFT9341_RD_DS_HORIZONTAL_REFRESH_OFFSET, TFT9341_RD_DS_HORIZONTAL_REFRESH_MASK);
    status->pixel_format = BIT_VALUE_MASK(data, TFT9341_RD_DS_PIXEL_FORMAT_OFFSET, TFT9341_RD_DS_PIXEL_FORMAT_MASK);
    status->idle_mode = BIT_VALUE_MASK(data, TFT9341_RD_DS_IDLE_MODE_OFFSET, TFT9341_RD_DS_IDLE_MODE_MASK);
    status->partial_mode = BIT_VALUE_MASK(data, TFT9341_RD_DS_PARTIAL_MODE_OFFSET, TFT9341_RD_DS_PARTIAL_MODE_MASK);
    status->sleep_mode = BIT_VALUE_MASK(data, TFT9341_RD_DS_SLEEP_MODE_OFFSET, TFT9341_RD_DS_SLEEP_MODE_MASK);
    status->normal_mode = BIT_VALUE_MASK(data, TFT9341_RD_DS_NORMAL_MODE_OFFSET, TFT9341_RD_DS_NORMAL_MODE_MASK);
    status->display_mode = BIT_VALUE_MASK(data, TFT9341_RD_DS_DISPLAY_MODE_OFFSET, TFT9341_RD_DS_DISPLAY_MODE_MASK);
    status->tearing_effect_line = BIT_VALUE_MASK(data, TFT9341_RD_DS_TEARING_EFFECT_OFFSET, TFT9341_RD_DS_TEARING_EFFECT_MASK);
    status->gamma_curve = BIT_VALUE_MASK(data, TFT9341_RD_DS_GAMMA_CURVE_OFFSET, TFT9341_RD_DS_GAMMA_CURVE_MASK);
    status->tearing_effect_mode = BIT_VALUE_MASK(data, TFT9341_RD_DS_TEARING_MODE_OFFSET, TFT9341_RD_DS_TEARING_MODE_MASK);
    
    return E_NO_ERROR;
}

err_t tft9341_read_power_mode(tft9341_t* tft, tft9341_power_mode_t* mode)
{
    if(mode == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_PM_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_POWER_MODE;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_PM_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    uint8_t data = data_buf[1];
    
    mode->booster_status = BIT_VALUE_MASK(data, TFT9341_RD_PM_BOOSTER_STATUS_OFFSET, TFT9341_RD_PM_BOOSTER_STATUS_MASK);
    mode->idle_mode = BIT_VALUE_MASK(data, TFT9341_RD_PM_IDLE_MODE_OFFSET, TFT9341_RD_PM_IDLE_MODE_MASK);
    mode->partial_mode = BIT_VALUE_MASK(data, TFT9341_RD_PM_PARTIAL_MODE_OFFSET, TFT9341_RD_PM_PARTIAL_MODE_MASK);
    mode->sleep_mode = BIT_VALUE_MASK(data, TFT9341_RD_PM_SLEEP_MODE_OFFSET, TFT9341_RD_PM_SLEEP_MODE_MASK);
    mode->normal_mode = BIT_VALUE_MASK(data, TFT9341_RD_PM_NORMAL_MODE_OFFSET, TFT9341_RD_PM_NORMAL_MODE_MASK);
    mode->display_mode = BIT_VALUE_MASK(data, TFT9341_RD_PM_DISPLAY_MODE_OFFSET, TFT9341_RD_PM_DISPLAY_MODE_MASK);

    return E_NO_ERROR;
}

err_t tft9341_read_madctl(tft9341_t* tft, tft9341_madctl_t* madctl)
{
    if(madctl == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_MC_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_MADCTL;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_MC_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    uint8_t data = data_buf[1];
    
    madctl->row_address_order = BIT_VALUE_MASK(data, TFT9341_RD_MC_ROW_ADDR_ORDER_OFFSET, TFT9341_RD_MC_ROW_ADDR_ORDER_MASK);
    madctl->col_address_order = BIT_VALUE_MASK(data, TFT9341_RD_MC_COL_ADDR_ORDER_OFFSET, TFT9341_RD_MC_COL_ADDR_ORDER_MASK);
    madctl->row_col_exchange = BIT_VALUE_MASK(data, TFT9341_RD_MC_ROW_COL_EXCH_OFFSET, TFT9341_RD_MC_ROW_COL_EXCH_MASK);
    madctl->vertical_refresh = BIT_VALUE_MASK(data, TFT9341_RD_MC_VERTICAL_REFRESH_OFFSET, TFT9341_RD_MC_VERTICAL_REFRESH_MASK);
    madctl->color_order = BIT_VALUE_MASK(data, TFT9341_RD_MC_COLOR_ORDER_OFFSET, TFT9341_RD_MC_COLOR_ORDER_MASK);
    madctl->horizontal_refresh = BIT_VALUE_MASK(data, TFT9341_RD_MC_HORIZONTAL_REFRESH_OFFSET, TFT9341_RD_MC_HORIZONTAL_REFRESH_MASK);
    
    return E_NO_ERROR;
}

err_t tft9341_read_pixel_format(tft9341_t* tft, tft9341_display_pixel_format_t* format)
{
    if(format == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_PF_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_DISPLAY_PIXEL_FORMAT;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_PF_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    uint8_t data = data_buf[1];
    
    format->parallel_iface_6bit = BIT_VALUE_MASK(data, TFT9341_RD_PF_RIM_OFFSET, TFT9341_RD_PF_RIM_MASK);
    format->parallel_iface_format = BIT_VALUE_MASK(data, TFT9341_RD_PF_DPI_OFFSET, TFT9341_RD_PF_DPI_MASK);
    format->mcu_iface_format = BIT_VALUE_MASK(data, TFT9341_RD_PF_DBI_OFFSET, TFT9341_RD_PF_DBI_MASK);

    return E_NO_ERROR;
}

err_t tft9341_read_image_format(tft9341_t* tft, tft9341_gamma_curve_t* format)
{
    if(format == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_IF_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_DISPLAY_IMAGE_FORMAT;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_IF_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    uint8_t data = data_buf[1];
    
    *format = BIT_VALUE_MASK(data, TFT9341_RD_IF_D_OFFSET, TFT9341_RD_IF_D_MASK);

    return E_NO_ERROR;
}

err_t tft9341_sleep_in(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_SLEEP_IN;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_sleep_out(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_SLEEP_OUT;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    //delay_ms(TFT9341_SLEEP_OUT_WAIT_TIME_MS);
    
    return E_NO_ERROR;
}

err_t tft9341_partial_mode(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_PARTIAL_MODE_ON;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_normal_mode(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_NORMAL_MODE_ON;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_inversion_off(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_INVERSION_OFF;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_inversion_on(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_INVERSION_ON;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_set_gamma(tft9341_t* tft, tft9341_gamma_curve_t gamma)
{
    //if(gamma == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_WR_DG_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_DISPLAY_GAMMA;
    *data_buf = gamma;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, TFT9341_WR_DG_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t tft9341_display_off(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_DISPLAY_OFF;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_display_on(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_DISPLAY_ON;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_set_column_address(tft9341_t* tft, uint16_t start, uint16_t end)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint16_t* data_buf = (uint16_t*)tft9341_get_buffer(tft, TFT9341_WR_CA_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_COL_ADDRESS;
    data_buf[0] = __REV16(start);
    data_buf[1] = __REV16(end);
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, TFT9341_WR_CA_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t tft9341_set_page_address(tft9341_t* tft, uint16_t start, uint16_t end)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint16_t* data_buf = (uint16_t*)tft9341_get_buffer(tft, TFT9341_WR_PGA_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_PAGE_ADDRESS;
    data_buf[0] = __REV16(start);
    data_buf[1] = __REV16(end);
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, TFT9341_WR_PGA_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t tft9341_write(tft9341_t* tft, const void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_MEMORY;
    
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    spi_message_setup(data_msg, SPI_WRITE, data, NULL, size);

    return tft9341_transfer(tft, false, 2);
}

err_t tft9341_begin_write(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_MEMORY;
    
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);

    return tft9341_transfer(tft, false, 1);
}

err_t tft9341_read(tft9341_t* tft, void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* trash_buf = tft9341_get_buffer(tft, TFT9341_TRASH_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(trash_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* trash_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_MEMORY;
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    spi_message_setup(trash_msg, SPI_READ, NULL, trash_buf, TFT9341_TRASH_SIZE);
    
    spi_message_setup(data_msg, SPI_READ, NULL, data, size);

    return tft9341_transfer(tft, false, 3);
}

err_t tft9341_set_partial_area(tft9341_t* tft, uint16_t start, uint16_t end)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint16_t* data_buf = (uint16_t*)tft9341_get_buffer(tft, TFT9341_WR_PLA_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_PARTIAL_AREA;
    data_buf[0] = __REV16(start);
    data_buf[1] = __REV16(end);
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, TFT9341_WR_PLA_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t tft9341_set_madctl(tft9341_t* tft, const tft9341_madctl_t* madctl)
{
    if(madctl == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_WR_MC_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_MADCTL;
    
    uint8_t data = 0;
    
    // Да здравствует ARM!
    // orr r3, r3, r2, lsl #N
    data |= madctl->row_address_order << TFT9341_WR_MC_ROW_ADDR_ORDER_OFFSET;
    data |= madctl->col_address_order << TFT9341_WR_MC_COL_ADDR_ORDER_OFFSET;
    data |= madctl->row_col_exchange << TFT9341_WR_MC_ROW_COL_EXCH_OFFSET;
    data |= madctl->vertical_refresh << TFT9341_WR_MC_VERTICAL_REFRESH_OFFSET;
    data |= madctl->color_order << TFT9341_WR_MC_COLOR_ORDER_OFFSET;
    data |= madctl->horizontal_refresh << TFT9341_WR_MC_HORIZONTAL_REFRESH_OFFSET;
    
    *data_buf = data;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, TFT9341_WR_MC_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_idle_off(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_IDLE_OFF;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_idle_on(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_IDLE_ON;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 1);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_set_pixel_format(tft9341_t* tft, tft9341_pixel_format_t rgb_iface_format, tft9341_pixel_format_t mcu_iface_format)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_WR_PF_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_DISPLAY_PIXEL_FORMAT;
    
    uint8_t data = 0;
    
    data |= rgb_iface_format << TFT9341_WR_PF_DPI_OFFSET;
    data |= mcu_iface_format << TFT9341_WR_PF_DBI_OFFSET;
    
    *data_buf = data;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, TFT9341_WR_PF_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t tft9341_write_continue(tft9341_t* tft, const void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_MEMORY_CONTINUE;
    
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    spi_message_setup(data_msg, SPI_WRITE, data, NULL, size);
    
    return tft9341_transfer(tft, false, 2);
}

err_t tft9341_begin_write_continue(tft9341_t* tft)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_MEMORY_CONTINUE;
    
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);

    return tft9341_transfer(tft, false, 1);
}

err_t tft9341_read_continue(tft9341_t* tft, void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* trash_buf = tft9341_get_buffer(tft, TFT9341_TRASH_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(trash_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* trash_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_MEMORY_CONTINUE;
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    spi_message_setup(trash_msg, SPI_READ, NULL, trash_buf, TFT9341_TRASH_SIZE);
    
    spi_message_setup(data_msg, SPI_READ, NULL, data, size);

    return tft9341_transfer(tft, false, 3);
}

err_t tft9341_set_brightness(tft9341_t* tft, uint8_t brightness)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_WR_BN_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_BRIGHTNESS;
    *data_buf = brightness;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, TFT9341_WR_BN_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t tft9341_read_brightness(tft9341_t* tft, uint8_t* brightness)
{
    if(brightness == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_BN_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_BRIGHTNESS;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_BN_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    *brightness = data_buf[1];

    return E_NO_ERROR;
}

err_t tft9341_set_display_control(tft9341_t* tft, const tft9341_display_control_t* control)
{
    if(control == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_WR_DC_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_DISPLAY_CONTROL;
    
    uint8_t data = 0;
    
    // Да здравствует ARM!
    // orr r3, r3, r2, lsl #N
    data |= control->brightness_control << TFT9341_WR_DC_BRIGHTNESS_CONTROL_OFFSET;
    data |= control->display_dimming << TFT9341_WR_DC_DISPLAY_DIMMING_OFFSET;
    data |= control->backlight_mode << TFT9341_WR_DC_BACKLIGHT_OFFSET;
    
    *data_buf = data;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, TFT9341_WR_DC_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t tft9341_read_display_control(tft9341_t* tft, tft9341_display_control_t* control)
{
    if(control == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_DC_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_DISPLAY_CONTROL;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_DC_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    uint8_t data = data_buf[1];
    
    control->brightness_control = BIT_VALUE_MASK(data, TFT9341_RD_DC_BRIGHTNESS_CONTROL_OFFSET, TFT9341_RD_DC_BRIGHTNESS_CONTROL_MASK);
    control->display_dimming = BIT_VALUE_MASK(data, TFT9341_RD_DC_DISPLAY_DIMMING_OFFSET, TFT9341_RD_DC_DISPLAY_DIMMING_MASK);
    control->backlight_mode = BIT_VALUE_MASK(data, TFT9341_RD_DC_BACKLIGHT_OFFSET, TFT9341_RD_DC_BACKLIGHT_MASK);
    
    return E_NO_ERROR;
}

err_t tft9341_set_cabc(tft9341_t* tft, tft9341_cabc_t cabc)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_WR_CABC_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_CABC;
    *data_buf = cabc;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, TFT9341_WR_CABC_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t tft9341_read_cabc(tft9341_t* tft, tft9341_cabc_t* cabc)
{
    if(cabc == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_CABC_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_CABC;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_CABC_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    uint8_t data = data_buf[1];
    
    *cabc = BIT_VALUE_MASK(data, TFT9341_RD_CABC_C_OFFSET, TFT9341_RD_CABC_C_MASK);

    return E_NO_ERROR;
}

err_t tft9341_set_cabc_min(tft9341_t* tft, uint8_t cabc_min)
{
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_WR_CMB_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_WRITE_CABC_MIN;
    *data_buf = cabc_min;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_WRITE, data_buf, NULL, TFT9341_WR_CMB_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;

    return E_NO_ERROR;
}

err_t tft9341_read_cabc_min(tft9341_t* tft, uint8_t* cabc_min)
{
    if(cabc_min == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_CMB_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_CABC_MIN;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_CMB_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    *cabc_min = data_buf[1];

    return E_NO_ERROR;
}

err_t tft9341_read_id1(tft9341_t* tft, uint8_t* id)
{
    if(id == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_ID1_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_ID1;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_ID1_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    *id = data_buf[1];

    return E_NO_ERROR;
}

err_t tft9341_read_id2(tft9341_t* tft, uint8_t* id)
{
    if(id == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_ID2_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_ID2;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_ID2_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    *id = data_buf[1];

    return E_NO_ERROR;
}

err_t tft9341_read_id3(tft9341_t* tft, uint8_t* id)
{
    if(id == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = tft9341_get_buffer(tft, TFT9341_RD_ID3_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = TFT9341_CMD_READ_ID3;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, TFT9341_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_sender_data(cmd_msg, tft);
    spi_message_set_callback(cmd_msg, tft9341_cmd_message_end);
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, TFT9341_RD_ID3_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_transfer(tft, false, 2);
    if(err != E_NO_ERROR) return err;
    
    err = tft9341_wait(tft);
    if(err != E_NO_ERROR) return err;
    
    *id = data_buf[1];

    return E_NO_ERROR;
}

err_t tft9341_data(tft9341_t* tft, const void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t message_index = 0;

    spi_message_t* data_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_setup(data_msg, SPI_WRITE, data, NULL, size);
    
    return tft9341_transfer(tft, true, 1);
}

err_t tft9341_set_pixel(tft9341_t* tft, uint16_t x, uint16_t y, const void* pixel, size_t size)
{
    if(pixel == NULL) return E_NULL_POINTER;
    if(size < TFT9341_PIXEL_SIZE_MIN || size > TFT9341_PIXEL_SIZE_MAX) return E_INVALID_VALUE;
    
    if(!tft9341_wait_current_op(tft)) return E_BUSY;
    
    //err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    //
    // Начальный и конечный столбец.
    //
    uint8_t* cmd_col_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_col_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_col_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_col_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint16_t* data_col_buf = (uint16_t*)tft9341_get_buffer(tft, TFT9341_WR_CA_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_col_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_col_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_col_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    //
    // Начальная и конечная страница.
    //
    uint8_t* cmd_page_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_page_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_page_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_page_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint16_t* data_page_buf = (uint16_t*)tft9341_get_buffer(tft, TFT9341_WR_PGA_DATA_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_page_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_page_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_page_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    //
    // Данные.
    //
    uint8_t* cmd_pixel_buf = tft9341_get_buffer(tft, TFT9341_CMD_SIZE, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_pixel_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_pixel_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(cmd_pixel_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_pixel_buf = tft9341_get_buffer(tft, size, &buffer_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_pixel_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_pixel_msg = tft9341_get_message(tft, &message_index);
#ifdef TFT9341_GET_MEM_DEBUG
    if(data_pixel_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_col_buf = TFT9341_CMD_WRITE_COL_ADDRESS;
    data_col_buf[0] = __REV16(x);
    data_col_buf[1] = __REV16(x);
    
    *cmd_page_buf = TFT9341_CMD_WRITE_PAGE_ADDRESS;
    data_page_buf[0] = __REV16(y);
    data_page_buf[1] = __REV16(y);
    
    *cmd_pixel_buf = TFT9341_CMD_WRITE_MEMORY;
    data_pixel_buf[0] = ((uint8_t*)pixel)[0];
    data_pixel_buf[1] = ((uint8_t*)pixel)[1];
    if(size == TFT9341_PIXEL_SIZE_MAX){
        data_pixel_buf[2] = ((uint8_t*)pixel)[2];
    }
    
    spi_message_setup(cmd_col_msg, SPI_WRITE, cmd_col_buf, NULL, TFT9341_CMD_SIZE);
    spi_message_set_sender_data(cmd_col_msg, tft);
    spi_message_set_callback(cmd_col_msg, tft9341_cmd_message_end);
    
    spi_message_setup(data_col_msg, SPI_WRITE, data_col_buf, NULL, TFT9341_WR_CA_DATA_SIZE);
    spi_message_set_sender_data(data_col_msg, tft);
    spi_message_set_callback(data_col_msg, tft9341_cmd_message_start);
    
    spi_message_setup(cmd_page_msg, SPI_WRITE, cmd_page_buf, NULL, TFT9341_CMD_SIZE);
    spi_message_set_sender_data(cmd_page_msg, tft);
    spi_message_set_callback(cmd_page_msg, tft9341_cmd_message_end);
    
    spi_message_setup(data_page_msg, SPI_WRITE, data_page_buf, NULL, TFT9341_WR_PGA_DATA_SIZE);
    spi_message_set_sender_data(data_page_msg, tft);
    spi_message_set_callback(data_page_msg, tft9341_cmd_message_start);
    
    spi_message_setup(cmd_pixel_msg, SPI_WRITE, cmd_pixel_buf, NULL, TFT9341_CMD_SIZE);
    spi_message_set_sender_data(cmd_pixel_msg, tft);
    spi_message_set_callback(cmd_pixel_msg, tft9341_cmd_message_end);
    
    spi_message_setup(data_pixel_msg, SPI_WRITE, data_pixel_buf, NULL, size);
    
    return tft9341_transfer(tft, false, 6);
}

