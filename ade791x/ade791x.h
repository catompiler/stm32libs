/**
 * @file ade791x.h Библиотека работы с измерителями ade791x.
 */

#ifndef ADE791X_H
#define ADE791X_H

#include <stm32f10x.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "errors/errors.h"
#include "future/future.h"
#include "spi/spi.h"
#include "defs/defs.h"

//! Идентификатор передачи по-умолчанию.
#define ADE791X_DEFAULT_TRANSFER_ID 79

//! Тип адреса памяти измерителя.
typedef uint8_t ade791x_address_t;


//! Размер буфера данных.
#define ADE791X_BUFFER_SIZE 4

//! Число доступных сообщений SPI.
#define ADE791X_MESSAGES_COUNT 2

//! Тип структуры памяти ade791x.
typedef struct _ADE791X {
    //! Шина SPI.
    spi_bus_t* spi;
    //! Идентификатор передачи.
    spi_transfer_id_t transfer_id;
    //! Порт выбора ведомого.
    GPIO_TypeDef* ce_gpio;
    //! Пин выбора ведомого.
    uint16_t ce_pin;
    //! Будущее.
    future_t future;
    //! Буфер данных.
    uint8_t buffer[ADE791X_BUFFER_SIZE];
    //! Сообщения SPI.
    spi_message_t messages[ADE791X_MESSAGES_COUNT];
} ade791x_t;

/**
 * Тип структуры инициализации дисплея.
 */
typedef struct _ADE791X_Init {
    spi_bus_t*          spi; //!< Шина SPI.
    spi_transfer_id_t   transfer_id; //!< Идентификатор передачи.
    GPIO_TypeDef*       ce_gpio; //!< Порт выбора ведомого.
    uint16_t            ce_pin; //!< Пин выбора ведомого.
} ade791x_init_t;


//! Перечисление частоты АЦП измерителя.
typedef enum _Ade791x_Adc_Freq {
    ADE791X_ADC_FREQ_8KHZ = 0, //!< 8 kHz.
    ADE791X_ADC_FREQ_4KHZ = 1, //!< 4 kHz.
    ADE791X_ADC_FREQ_2KHZ = 2, //!< 2 kHz.
    ADE791X_ADC_FREQ_1KHZ = 3, //!< 1 kHz.
} ade791x_adc_freq_t;

//! Перечисление полосы пропускания ФНЧ АЦП.
typedef enum _Ade791x_Adc_Lpf {
    ADE791X_ADC_LPF_3_3KHZ = 0, //!< 3.3 kHz.
    ADE791X_ADC_LPF_2KHZ = 1, //!< 2 kHz.
} ade791x_adc_lpf_t;

//! Тип конфигурации измерителя.
typedef struct _Ade791x_Config {
    bool clock_out_enabled; //!< Разрешение вывода тактиравания.
    bool power_down_enabled; //!< Разрешение выключения DC-DC.
    bool temp_enabled; //!< Выбор температуры как второго канала напряжения.
    ade791x_adc_freq_t adc_freq; //!< Частота АЦП.
    bool soft_reset; //!< Программный сброс.
    ade791x_adc_lpf_t adc_lpf; //!< ФНЧ АЦП.
} ade791x_config_t;

//! Тип первого состояния измерителя.
typedef struct _Ade791x_Status0 {
    bool reset_in_progress; //!< Флаг процесса сброса.
    bool crc_changed; //!< Флаг изменения CRC.
    bool config_protected; //!< Флаг состояния защиты конфигурации.
} ade791x_status0_t;

//! Sync/Snap.
typedef struct _Ade791x_Sync_Snap {
    bool sync; //!< Синхронизация нескольких измерителей.
    bool snap; //!< Защёлкивание внутренних счётчиков.
} ade791x_sync_snap_t;

//! Тип второго состояния измерителя.
typedef struct _Ade791x_Status1 {
    uint8_t version; //!< Версия измерителя.
    bool adc_not_accessed; //!< Флаг отсутствия доступа к регистрам АЦП.
} ade791x_status1_t;

//! Структура данных АЦП измерителя.
typedef struct _Ade791x_Data {
    int32_t current; //!< Ток.
    int32_t voltage1; //!< Напряжение 1.
    int32_t voltage2; //!< Напряжение 2.
} ade791x_data_t;


/**
 * Каллбэк SPI.
 * @return true, если событие обработано, иначе false.
 */
EXTERN bool ade791x_spi_callback(ade791x_t* ade791x);

/**
 * Инициализирует измеритель.
 * @param ade791x Измеритель.
 * @param ade791x_init Структура инициализации.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_init(ade791x_t* ade791x, ade791x_init_t* ade791x_init);

/**
 * Получает флаг занятости измерителя.
 * @param ade791x Измеритель.
 * @return Флаг занятости измерителя.
 */
EXTERN bool ade791x_busy(ade791x_t* ade791x);

/**
 * Получает код ошибки асинхронно выполненой операции.
 * @param ade791x Измеритель.
 * @return Код ошибки асинхронно выполненой операции.
 */
EXTERN err_t ade791x_error(ade791x_t* ade791x);

/**
 * Ожидает завершения операции с измерителем.
 * @param ade791x Измеритель.
 * @return Код ошибки операции.
 */
EXTERN err_t ade791x_wait(ade791x_t* ade791x);

/**
 * Начинает широковещательную рассылку.
 * Вызов функции требуется только для
 * ведомых измерителей.
 * @param ade791x Измеритель.
 */
EXTERN void ade791x_broadcast_begin(ade791x_t* ade791x);

/**
 * Завершает широковещательную рассылку.
 * Вызов функции требуется только для
 * ведомых измерителей.
 * @param ade791x Измеритель.
 */
EXTERN void ade791x_broadcast_end(ade791x_t* ade791x);

/**
 * Считывает данные.
 * Асинхронная операция.
 * @param ade791x Измеритель.
 * @param addr Адрес.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read(ade791x_t* ade791x, ade791x_address_t addr, void* data, size_t size);

/**
 * Записывает данные.
 * Асинхронная операция.
 * @param ade791x Измеритель.
 * @param addr Адрес регистра.
 * @param data Данные.
 * @param size Размер данных.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_write(ade791x_t* ade791x, ade791x_address_t addr, const void* data, size_t size);

/**
 * Читает данные измерителя.
 * @param ade791x Измеритель.
 * @param data Данные.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_data(ade791x_t* ade791x, ade791x_data_t* data);

/**
 * Читает значение тока.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param value Значение.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_current(ade791x_t* ade791x, int32_t* value);

/**
 * Читает значение напряжения 1.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param value Значение.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_voltage1(ade791x_t* ade791x, int32_t* value);

/**
 * Читает значение напряжения 2.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param value Значение.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_voltage2(ade791x_t* ade791x, int32_t* value);

/**
 * Читает значение контрольной суммы значений АЦП.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param crc Контрольная сумма.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_adc_crc(ade791x_t* ade791x, uint16_t* crc);

/**
 * Читает значение контрольной суммы регистров конфигурации.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param crc Контрольная сумма.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_conf_crc(ade791x_t* ade791x, uint16_t* crc);

/**
 * Читает значение снимка счётчика.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param cnt_snapshot Снимок счётчика.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_counter_snapshot(ade791x_t* ade791x, uint16_t* cnt_snapshot);

/**
 * Читает значение счётчика 0.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param counter Значение счётчика.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_counter0(ade791x_t* ade791x, uint8_t* counter);

/**
 * Читает значение счётчика 1.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param counter Значение счётчика.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_counter1(ade791x_t* ade791x, uint8_t* counter);

/**
 * Читает конфигурацию измерителя.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param config Конфигурация.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_config(ade791x_t* ade791x, ade791x_config_t* config);

/**
 * Записывает конфигурацию измерителя.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param config Конфигурация.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_write_config(ade791x_t* ade791x, const ade791x_config_t* config);

/**
 * Читает первое состояние измерителя.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param status Состояние.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_status0(ade791x_t* ade791x, ade791x_status0_t* status);

/**
 * Блокирует изменение конфигурации измерителя.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_config_lock(ade791x_t* ade791x);

/**
 * Разблокирует изменение конфигурации измерителя.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_config_unlock(ade791x_t* ade791x);

/**
 * Читает Sync/Snap измерителя.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param sync_snap Sync/Snap.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_sync_snap(ade791x_t* ade791x, ade791x_sync_snap_t* sync_snap);

/**
 * Записывает Sync/Snap измерителя.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param sync_snap Sync/Snap.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_write_sync_snap(ade791x_t* ade791x, const ade791x_sync_snap_t* sync_snap);

/**
 * Читает EMI Control измерителя.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param emi_control EMI Control.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_emi_control(ade791x_t* ade791x, uint8_t* emi_control);

/**
 * Записывает EMI Control измерителя.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param emi_control EMI Control.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_write_emi_control(ade791x_t* ade791x, uint8_t emi_control);

/**
 * Читает второе состояние измерителя.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param status Состояние.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_status1(ade791x_t* ade791x, ade791x_status1_t* status);

/**
 * Читает значение смещения датчика температуры.
 * Синхронная операция.
 * @param ade791x Измеритель.
 * @param tempos Значение смещения датчика температуры.
 * @return Код ошибки.
 */
EXTERN err_t ade791x_read_tempos(ade791x_t* ade791x, int8_t* tempos);

/**
 * Расширяет 24 битное значение в 32 бита.
 * @param value Значение 24 бит.
 * @return Значение 32 бит.
 */
EXTERN int32_t ade791x_expand(const uint8_t* value);

#endif	// ADE791X_H

