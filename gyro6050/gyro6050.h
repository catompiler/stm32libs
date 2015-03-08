/**
 * @file gyro6050.h
 * Библиотека для работы с гироскопом MPU-6050.
 */

#ifndef GYRO6050_H
#define	GYRO6050_H

#include "errors/errors.h"
#include "i2c/i2c.h"
#include "fixed/fixed16.h"
#include "fixed/fixed32.h"
#include "future/future.h"

//! Адреса i2c гироскопа.
//! Пин AD0 подтянут к земле.
#define GYRO6050_I2C_ADDRESS0 0x68
//! Пин AD0 подтянут к питанию.
#define GYRO6050_I2C_ADDRESS1 0x69

//! Идентификатор передачи i2c по умолчанию.
#define GYRO6050_DEFAULT_I2C_TRANSFER_ID GYRO6050_I2C_ADDRESS0

//! Частота вывода данных.
//! Частота вывода данных акселерометра.
#define GYRO6050_ACCEL_OUTPUT_RATE_KHZ      1
//! Частота вывода данных гироскопа.
#define GYRO6050_GYRO_OUTPUT_RATE_KHZ       8
//! Частота вывода данных гироскопа с DLPF.
#define GYRO6050_GYRO_DLPF_OUTPUT_RATE_KHZ  1


//! Фильтр низких частот.
typedef enum _Gyro5050_Dlpf {
    //! Акселерометр 260 Hz, гироскоп 256 Hz.
    GYRO6050_DLPF_A260HZ_G256HZ = 0,
    //! Акселерометр 184 Hz, гироскоп 188 Hz.
    GYRO6050_DLPF_A184HZ_G188HZ = 1,
    //! Акселерометр 94 Hz, гироскоп 98 Hz.
    GYRO6050_DLPF_A94HZ_G98HZ = 2,
    //! Акселерометр 44 Hz, гироскоп 42 Hz.
    GYRO6050_DLPF_A44HZ_G42HZ = 3,
    //! Акселерометр 21 Hz, гироскоп 20 Hz.
    GYRO6050_DLPF_A21HZ_G20HZ = 4,
    //! Акселерометр 10 Hz, гироскоп 10 Hz.
    GYRO6050_DLPF_A10HZ_G10HZ = 5,
    //! Акселерометр 5 Hz, гироскоп 5 Hz.
    GYRO6050_DLPF_A5HZ_G5HZ = 6
} gyro6050_dlpf_t;


//! Диапазон данных гироскопа.
typedef enum _Gyro6050_Gyro_Scale_Range {
    //! +-250 degrees per second
    GYRO6050_GYRO_SCALE_RANGE_250_DPS = 0,
    //! +-500 degrees per second
    GYRO6050_GYRO_SCALE_RANGE_500_DPS = 1,
    //! +-1000 degrees per second
    GYRO6050_GYRO_SCALE_RANGE_1000_DPS = 2,
    //! +-2000 degrees per second
    GYRO6050_GYRO_SCALE_RANGE_2000_DPS = 3
} gyro6050_gyro_scale_range_t;

//! Диапазон данных акселерометра.
typedef enum _Gyro6050_Accel_Scale_Range {
    //! +-2g
    GYRO6050_ACCEL_SCALE_RANGE_2G = 0,
    //! +-4g
    GYRO6050_ACCEL_SCALE_RANGE_4G = 1,
    //! +-8g
    GYRO6050_ACCEL_SCALE_RANGE_8G = 2,
    //! +-16g
    GYRO6050_ACCEL_SCALE_RANGE_16G = 3
} gyro6050_accel_scale_range_t;

//! Конфигурация пина сигнала о прерываниях.
//! Уровень активного сигнала - высокий.
#define GYRO6050_INT_PIN_PIN_LEVEL_HI               0
//! Уровень активного сигнала - низкий.
#define GYRO6050_INT_PIN_LEVEL_LO                   128
//! Тип выхода - push-pull.
#define GYRO6050_INT_PIN_OUT_PUSH_PULL              0
//! Тип выхода - открытый коллектор.
#define GYRO6050_INT_PIN_OUT_OPEN_DRAIN             64
//! Продолжительность сигнала - 50 us.
#define GYRO6050_INT_PIN_PULSE_50US                 0
//! Продолжительность сигнала - пока не очищен флаг прерывания.
#define GYRO6050_INT_PIN_PULSE_UNTIL_CLEARED        32
//! Очистка флага прерывания - только при чтении регистра статуса.
#define GYRO6050_INT_PIN_CLEAN_ON_READ_STATUS       0
//! Очистка флага прерывания - при любом чтении.
#define GYRO6050_INT_PIN_CLEAN_ON_ANY_READ_OP       16
//! Уровень активного сигнала FSYNC - высокий.
#define GYRO6050_INT_PIN_FSYNC_LEVEL_HI             0
//! Уровень активного сигнала FSYNC - низкий.
#define GYRO6050_INT_PIN_FSYNC_LEVEL_LO             8
//! Генерация сигнала на пине FSYNC - выключена.
#define GYRO6050_INT_PIN_FSYNC_DISABLED             0
//! Генерация сигнала на пине FSYNC - включена.
#define GYRO6050_INT_PIN_FSYNC_ENABLED              4
//! Получение доступа к AUX i2c - запрещено.
#define GYRO6050_INT_PIN_I2C_BYPASS_DISABLED        0
//! Получение доступа к AUX i2c - разрешено.
#define GYRO6050_INT_PIN_I2C_BYPASS_ENABLED         2
//! Тип конфигурации пина сигнала прерываниях.
typedef uint8_t gyro6050_int_pin_conf_t;

//! Конфигурация сигнала о прерываниях.
typedef enum _Gyro6050_Int_Conf {
    //! Прерывание от детектора движения.
    GYRO6050_INT_ON_MOTION_DETECTION = 64,
    //! Прерывание по переполнению буфера FIFO.
    GYRO6050_INT_ON_FIFO_OVERFLOW = 16,
    //! Прерывание при событии i2c.
    GYRO6050_INT_ON_I2C_EVENT = 8,
    //! Прерывание по готовности данных.
    GYRO6050_INT_ON_DATA_READY = 1
} gyro6050_int_conf_t;

//! Флаги питания.
//! Сброс устройства.
#define GYRO6050_POWER_DEVICE_RESET     0x80
//! Сон.
#define GYRO6050_POWER_SLEEP            0x40
//! Цикличность.
#define GYRO6050_POWER_CYCLE            0x20
//! Отключение температуры.
#define GYRO6050_POWER_TEMP_DISABLE     0x8
//! Значение по-умолчанию.
#define GYRO6050_POWER_WAKEUP           0x0
//! Тип флагов питания.
typedef uint8_t gyro6050_power_t;

//! Источник тактирования.
typedef enum _Gyro6050_Clock {
    //! Внетренний RC-осциллятор 8 МГц.
    GYRO6050_CLOCK_INTERNAL_RC_8_MHz = 0,
    //! Гироскоп, ось X.
    GYRO6050_CLOCK_GYRO_AXIS_X = 1,
    //! Гироскоп, ось Y.
    GYRO6050_CLOCK_GYRO_AXIS_Y = 2,
    //! Гироскоп, ось Z.
    GYRO6050_CLOCK_GYRO_AXIS_Z = 3,
    //! Внешний осциллятор 32.768 kHz.
    GYRO6050_CLOCK_EXTERNAL_32_768_kHz = 4,
    //! Внешний осциллятор 19.2 MHz.
    GYRO6050_CLOCK_EXTERNAL_19_2_MHz = 5,
    //! Зарезервировано.
    GYRO6050_CLOCK_RESERVED = 6,
    //! Остановка часов.
    GYRO6050_CLOCK_STOP = 7
} gyro6050_clock_t;

//! Сырые данные датчика.
#pragma pack(push, 1)
typedef struct _Gyro6050RawData{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    
    int16_t temp;
    
    int16_t gyro_w_x;
    int16_t gyro_w_y;
    int16_t gyro_w_z;
}gyro6050_raw_data_t;
#pragma pack(pop)

//! Преобразованные данные гироскопа, акселерометра и температуры.
typedef struct _Gyro6050Data{
    fixed32_t accel_x;
    fixed32_t accel_y;
    fixed32_t accel_z;
    
    fixed16_t temp;
    
    fixed32_t gyro_w_x;
    fixed32_t gyro_w_y;
    fixed32_t gyro_w_z;
}gyro6050_data_t;

//! Кэшированные конфигурационные данные.
typedef struct _Gyro6050CachedData{
    //! Диапазон данных гироскопа.
    gyro6050_gyro_scale_range_t gyro_scale_range;
    //! Диапазон данных акселерометра.
    gyro6050_accel_scale_range_t accel_scale_range;
    //! Делитель частоты вывода.
    uint8_t rate_divisor;
    //! dlpf.
    uint8_t dlpf;
    //! Конфигурация пина сигнала о прерываниях.
    gyro6050_int_pin_conf_t int_pin_config;
    //! Конфигурация сигнала о прерываниях.
    gyro6050_int_conf_t int_config;
}gyro6050_cached_data_t;

//! Число сообщений i2c.
#define GYRO6050_I2C_MESSAGES_COUNT 2
//! Номер сообщения с адресом памяти.
#define GYRO6050_I2C_MESSAGE_ROM_ADDRESS 0
//! Номер сообщения с данными.
#define GYRO6050_I2C_MESSAGE_DATA 1

//! Структура состояния гироскопа.
typedef struct _Gyro5060{
    //! Шина i2c.
    i2c_bus_t* i2c;
    //! Адрес i2c.
    //i2c_address_t i2c_address;
    //! Идентификато передачи i2c.
    i2c_transfer_id_t i2c_transfer_id;
    //! Адрес страницы i2c.
    uint8_t rom_address;
    //! Байт данных для обмена.
    uint8_t data_byte;
    //! Сообщения i2c.
    i2c_message_t i2c_messages[GYRO6050_I2C_MESSAGES_COUNT];
    //! Будущее.
    future_t future;
    //! Сырые данные.
    gyro6050_raw_data_t raw_data;
    //! Вычисленные данные.
    gyro6050_data_t data;
    //! Кэшированные данные.
    gyro6050_cached_data_t cached_data;
    //! Флаг новых данных.
    bool new_data_avail;
    //! Состояние (текущее действие).
    uint8_t state;
}gyro6050_t;



/**
 * Каллбэк i2c гироскопа.
 * @return Флаг обработки события.
 */
extern bool gyro6050_i2c_callback(gyro6050_t* gyro);

/**
 * Инициализирует данные гироскопа.
 * Идентификатор пердачи по умолчанию.
 * @param gyro Гироскоп.
 * @param i2c Шина i2c.
 * @param address Адрес устройства на шине i2c.
 * @return Код ошибки.
 */
extern err_t gyro6050_init(gyro6050_t* gyro, i2c_bus_t* i2c, i2c_address_t address);

/**
 * Получает флаг завершения текущего действия.
 * @param gyro Гироскоп.
 * @return Флаг завершения текущего действия.
 */
extern bool gyro6050_done(const gyro6050_t* gyro);

/**
 * Ждёт завершения текущей операции.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
extern err_t gyro6050_wait(const gyro6050_t* gyro);

/**
 * Получает флаг наличия выполняемой операции.
 * @param gyro Гироскоп.
 * @return Флаг наличия выполняемой операции.
 */
extern bool gyro6050_busy(const gyro6050_t* gyro);

/**
 * Получает код ошибки последней операции.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
extern err_t gyro6050_error(const gyro6050_t* gyro);

/**
 * Получает идентификатор передачи i2c.
 * @param gyro Гироскоп.
 * @return Идентификатор передачи i2c.
 */
extern i2c_transfer_id_t gyro6050_i2c_transfer_id(const gyro6050_t* gyro);

/**
 * Устанавливает идентификатор передачи i2c.
 * @param gyro Гироскоп.
 * @param transfer_id Идентификатор передачи i2c.
 */
extern void gyro6050_set_i2c_transfer_id(gyro6050_t* gyro, i2c_transfer_id_t transfer_id);

/**
 * Считывает делитель частоты вывода из гироскопа.
 * Данные действительны до следующей операции с гироскопом.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
extern err_t gyro6050_read_rate_divisor(gyro6050_t* gyro);

/**
 * Получает считанное значение делителя частоты вывода гироскопа.
 * @param gyro Гироскоп.
 * @return Считанное значение делителя частоты вывода гироскопа.
 */
extern uint8_t gyro6050_rate_divisor(const gyro6050_t* gyro);

/**
 * Устанавливает делитель частоты вывода гироскопа.
 * @param gyro Гироскоп.
 * @param divisor Делитель.
 * @return Код ошибки.
 */
extern err_t gyro6050_set_rate_divisor(gyro6050_t* gyro, uint8_t divisor);

/**
 * Считывает значение фильтра низких частот..
 * Данные действительны до следующей операции с гироскопом.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
extern err_t gyro6050_read_dlpf(gyro6050_t* gyro);

/**
 * Получает считанное значение фильтра низких частот.
 * @param gyro Гироскоп.
 * @return Считанное значение фильтра низких частот.
 */
extern gyro6050_dlpf_t gyro6050_dlpf(const gyro6050_t* gyro);

/**
 * Устанавливает значение фильтра низких частот.
 * @param gyro Гироскоп.
 * @param dlpf Значение фильтра низких частот.
 * @return Код ошибки.
 */
extern err_t gyro6050_set_dlpf(gyro6050_t* gyro, gyro6050_dlpf_t dlpf);

/**
 * Считывает значение диапазона данных гироскопа.
 * Данные действительны до следующей операции с гироскопом.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
extern err_t gyro6050_read_gyro_scale_range(gyro6050_t* gyro);

/**
 * Получает считанное значение диапазона данных гироскопа.
 * @param gyro Гироскоп.
 * @return Считанное значение диапазона данных гироскопа.
 */
extern gyro6050_gyro_scale_range_t gyro6050_gyro_scale_range(const gyro6050_t* gyro);

/**
 * Устанавливает значение диапазона данных гироскопа.
 * @param gyro Гироскоп.
 * @param range Значение диапазона данных гироскопа.
 * @return Код ошибки.
 */
extern err_t gyro6050_set_gyro_scale_range(gyro6050_t* gyro, gyro6050_gyro_scale_range_t range);

/**
 * Считывает значение диапазона данных акселерометра.
 * Данные действительны до следующей операции с гироскопом.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
extern err_t gyro6050_read_accel_scale_range(gyro6050_t* gyro);

/**
 * Получает считанное значение диапазона данных акселерометра.
 * @param gyro Гироскоп.
 * @return Считанное значение диапазона данных акселерометра.
 */
extern gyro6050_accel_scale_range_t gyro6050_accel_scale_range(const gyro6050_t* gyro);

/**
 * Устанавливает значение диапазона данных акселерометра.
 * @param gyro Гироскоп.
 * @param range Значение диапазона данных акселерометра.
 * @return Код ошибки.
 */
extern err_t gyro6050_set_accel_scale_range(gyro6050_t* gyro, gyro6050_accel_scale_range_t range);

/**
 * Считывает .
 * Данные действительны до следующей операции с гироскопом.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
//extern err_t gyro6050_read_(gyro5060_t* gyro);

/**
 * Получает считанное значение .
 * @param gyro Гироскоп.
 * @return Считанное значение .
 */
//extern uint8_t gyro6050_(gyro5060_t* gyro);

/**
 * Устанавливает .
 * @param gyro Гироскоп.
 * @param data.
 * @return Код ошибки.
 */
//extern err_t gyro6050_set_(gyro5060_t* gyro, uint8_t data);

/**
 * Конфигурирует пин сигнала о прерываниях.
 * @param gyro Гироскоп.
 * @param conf Конфигурация пина.
 * @return Код ошибки.
 */
extern err_t gyro6050_int_pin_configure(gyro6050_t* gyro, gyro6050_int_pin_conf_t conf);

/**
 * Считывает конфигурацию пина сигнала о прерываниях.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
extern err_t gyro6050_read_int_pin_config(gyro6050_t* gyro);

/**
 * Получает считанную конфигурацию пина сигнала о прерываниях.
 * @param gyro Гироскоп.
 * @return Конфигурацию пина сигнала о прерываниях.
 */
extern gyro6050_int_pin_conf_t gyro6050_int_pin_config(const gyro6050_t* gyro);

/**
 * Конфигурирует сигнал о прерываниях.
 * @param gyro Гироскоп.
 * @param conf Конфигурация сигнала.
 * @return Код ошибки.
 */
extern err_t gyro6050_int_configure(gyro6050_t* gyro, gyro6050_int_conf_t conf);

/**
 * Считывает конфигурацию сигнала о прерываниях.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
extern err_t gyro6050_read_int_config(gyro6050_t* gyro);

/**
 * Получает считанную конфигурацию сигнала о прерываниях.
 * @param gyro Гироскоп.
 * @return Конфигурацию сигнала о прерываниях.
 */
extern gyro6050_int_conf_t gyro6050_int_config(const gyro6050_t* gyro);

/**
 * Пробуждает гироскоп.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
extern err_t gyro6050_power_control(gyro6050_t* gyro, gyro6050_power_t power_flags, gyro6050_clock_t clock);

/**
 * Считывает данные из гироскопа.
 * @param gyro Гироскоп.
 * @return Код ошибки.
 */
extern err_t gyro6050_read(gyro6050_t* gyro);

/**
 * Вычисляет ориентацию по полученным данным.
 * @param gyro Гироскоп.
 */
extern void gyro6050_calculate(gyro6050_t* gyro);

/**
 * Получает температуру гироскопа.
 * @param gyro Гироскоп.
 * @return Температура гироскопа.
 */
extern fixed16_t gyro6050_temp(const gyro6050_t* gyro);

/**
 * Получает ускорение по оси X.
 * @param gyro Гироскоп.
 * @return Ускорение по оси X.
 */
extern fixed32_t gyro6050_accel_x(const gyro6050_t* gyro);

/**
 * Получает ускорение по оси Y.
 * @param gyro Гироскоп.
 * @return Ускорение по оси Y.
 */
extern fixed32_t gyro6050_accel_y(const gyro6050_t* gyro);

/**
 * Получает ускорение по оси Z.
 * @param gyro Гироскоп.
 * @return Ускорение по оси Z.
 */
extern fixed32_t gyro6050_accel_z(const gyro6050_t* gyro);

/**
 * Получает угловую скорость по оси X.
 * @param gyro Гироскоп.
 * @return Угловую скорость по оси X.
 */
extern fixed32_t gyro6050_gyro_w_x(const gyro6050_t* gyro);

/**
 * Получает угловую скорость по оси Y.
 * @param gyro Гироскоп.
 * @return Угловую скорость по оси Y.
 */
extern fixed32_t gyro6050_gyro_w_y(const gyro6050_t* gyro);

/**
 * Получает угловую скорость по оси Z.
 * @param gyro Гироскоп.
 * @return Угловую скорость по оси Z.
 */
extern fixed32_t gyro6050_gyro_w_z(const gyro6050_t* gyro);

#endif	/* GYRO6050_H */

