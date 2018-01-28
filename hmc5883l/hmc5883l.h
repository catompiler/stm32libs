/**
 * @file hmc5883l.h
 * Библиотека для работы с компасом HMC5883L.
 */

#ifndef HMC5883L_H
#define	HMC5883L_H

#include "errors/errors.h"
#include "i2c/i2c.h"
#include "fixed/fixed16.h"
#include "fixed/fixed32.h"
#include "future/future.h"
#include <stdbool.h>
#include <stdint.h>

//! Адреса i2c компаса.
//! Пин AD0 подтянут к земле.
#define HMC5883L_I2C_ADDRESS 0x1e

//! Идентификатор передачи i2c по умолчанию.
#define HMC5883L_DEFAULT_I2C_TRANSFER_ID HMC5883L_I2C_ADDRESS

//! Число выборок на измерение.
typedef enum _Hmc5883l_Samples {
    //! 1.
    HMC5883L_SAMPLES_1 = 0,
    //! 2.
    HMC5883L_SAMPLES_2 = 1,
    //! 4.
    HMC5883L_SAMPLES_4 = 2,
    //! 8.
    HMC5883L_SAMPLES_8 = 3
} hmc5883l_samples_t;

//! Частота измерений.
typedef enum _Hmc5883l_Output_Rate {
    //! 0.75 кГц.
    HMC5883L_OUTPUT_RATE_0_75_HZ = 0,
    //! 1.5 кГц.
    HMC5883L_OUTPUT_RATE_1_5_HZ = 1,
    //! 3 кГц.
    HMC5883L_OUTPUT_RATE_3_HZ = 2,
    //! 7.5 кГц.
    HMC5883L_OUTPUT_RATE_7_5_HZ = 3,
    //! 15 кГц.
    HMC5883L_OUTPUT_RATE_15_HZ = 4,
    //! 30 кГц.
    HMC5883L_OUTPUT_RATE_30_HZ = 5,
    //! 75 кГц.
    HMC5883L_OUTPUT_RATE_75_HZ = 6
} hmc5883l_output_rate_t;

//! Режим измерений.
typedef enum _Hmc5883l_Measurement_Mode {
    //! Normal.
    HMC5883L_MEASUREMENT_MODE_NORMAL = 0,
    //! Positive bias configuration.
    HMC5883L_MEASUREMENT_MODE_POSITIVE_BIAS = 1,
    //! Negative bias configuration.
    HMC5883L_MEASUREMENT_MODE_NEGATIVE_BIAS = 2
} hmc5883l_measurement_mode_t;

//! Разрешение.
typedef enum _Hmc5883l_Resoulution {
    // 0.73 mG/LSB.
    HMC5883L_RESOULUTION_0_73_mG_PER_LSB = 0,
    // 0.92 mG/LSB.
    HMC5883L_RESOULUTION_0_92_mG_PER_LSB = 1,
    // 1.22 mG/LSB.
    HMC5883L_RESOULUTION_1_22_mG_PER_LSB = 2,
    // 1.52 mG/LSB.
    HMC5883L_RESOULUTION_1_52_mG_PER_LSB = 3,
    // 2.27 mG/LSB.
    HMC5883L_RESOULUTION_2_27_mG_PER_LSB = 4,
    // 2.56 mG/LSB.
    HMC5883L_RESOULUTION_2_56_mG_PER_LSB = 5,
    // 3.03 mG/LSB.
    HMC5883L_RESOULUTION_3_03_mG_PER_LSB = 6,
    // 4.35 mG/LSB.
    HMC5883L_RESOULUTION_4_35_mG_PER_LSB = 7
} hmc5883l_resolution_t;

//! Флаги режима компаса.
//! Режим высокой скорости i2c.
#define HMC5883L_MODE_I2C_HIGH_SPEED_400_KHZ 0x80

//! Режим компаса.
typedef enum _Hmc5883l_Operating_Mode {
    //! Режим непрерывного измерения.
    HMC5883L_OPERATING_MODE_CONTINUOUS = 0,
    //! Режим одиночного измерения.
    HMC5883L_OPERATING_MODE_SINGLE = 1,
    //! Режим ожидания.
    HMC5883L_OPERATING_MODE_IDLE = 2
} hmc5883l_operating_mode_t;

//! Сырые данные датчика.
#pragma pack(push, 1)
typedef struct _Hmc5883lRawData{
    int16_t compass_x;
    int16_t compass_z;
    int16_t compass_y;
}hmc5883l_raw_data_t;
#pragma pack(pop)

//! Преобразованные данные компаса, акселерометра и температуры.
typedef struct _Hmc5883lData{
    fixed32_t compass_x;
    fixed32_t compass_y;
    fixed32_t compass_z;
}hmc5883l_data_t;

//! Кэшированные конфигурационные данные.
typedef struct _Hmc5883lConfdData{
    //! Разрешение.
    hmc5883l_resolution_t resolution;
}hmc5883l_conf_data_t;

//! Число сообщений i2c.
#define HMC5883L_I2C_MESSAGES_COUNT 2
//! Номер сообщения с адресом памяти.
#define HMC5883L_I2C_MESSAGE_ROM_ADDRESS 0
//! Номер сообщения с данными.
#define HMC5883L_I2C_MESSAGE_DATA 1

//! Структура состояния компаса.
typedef struct _Hmc5883l{
    //! Шина i2c.
    i2c_bus_t* i2c;
    //! Адрес i2c.
    //i2c_address_t i2c_address;
    //! Идентификато передачи i2c.
    i2c_transfer_id_t i2c_transfer_id;
    //! Адрес страницы i2c.
    uint8_t rom_address;
    //! Сообщения i2c.
    i2c_message_t i2c_messages[HMC5883L_I2C_MESSAGES_COUNT];
    //! Будущее.
    future_t future;
    //! Сырые данные.
    hmc5883l_raw_data_t raw_data;
    //! Вычисленные данные.
    hmc5883l_data_t data;
    //! Кэшированные данные.
    hmc5883l_conf_data_t conf_data;
}hmc5883l_t;



/**
 * Каллбэк i2c компаса.
 * @return Флаг обработки события.
 */
extern bool hmc5883l_i2c_callback(hmc5883l_t* compass);

/**
 * Инициализирует данные компаса.
 * Идентификатор пердачи по умолчанию.
 * @param compass Компас.
 * @param i2c Шина i2c.
 * @param address Адрес устройства на шине i2c.
 * @return Код ошибки.
 */
extern err_t hmc5883l_init(hmc5883l_t* compass, i2c_bus_t* i2c, i2c_address_t address);

/**
 * Сбрасывает состояние.
 * @param compass Компас.
 */
EXTERN void hmc5883l_reset(hmc5883l_t* compass);

/**
 * Завершает операцию
 * с ошибкой тайм-аута.
 * @param compass Компас.
 */
EXTERN void hmc5883l_timeout(hmc5883l_t* compass);

/**
 * Получает флаг завершения текущего действия.
 * @param compass Компас.
 * @return Флаг завершения текущего действия.
 */
extern bool hmc5883l_done(const hmc5883l_t* compass);

/**
 * Ждёт завершения текущей операции.
 * @param compass Компас.
 * @return Код ошибки.
 */
extern err_t hmc5883l_wait(const hmc5883l_t* compass);

/**
 * Получает флаг наличия выполняемой операции.
 * @param compass Компас.
 * @return Флаг наличия выполняемой операции.
 */
extern bool hmc5883l_busy(const hmc5883l_t* compass);

/**
 * Получает код ошибки последней операции.
 * @param compass Компас.
 * @return Код ошибки.
 */
extern err_t hmc5883l_error(const hmc5883l_t* compass);

/**
 * Получает идентификатор передачи i2c.
 * @param compass Компас.
 * @return Идентификатор передачи i2c.
 */
extern i2c_transfer_id_t hmc5883l_i2c_transfer_id(const hmc5883l_t* compass);

/**
 * Устанавливает идентификатор передачи i2c.
 * @param compass Компас.
 * @param transfer_id Идентификатор передачи i2c.
 */
extern void hmc5883l_set_i2c_transfer_id(hmc5883l_t* compass, i2c_transfer_id_t transfer_id);

/**
 * Считывает конфигурацию компаса.
 * @param compass Компас.
 * @param samples Число выборок.
 * @param output_rate Частота измерения.
 * @param measurement_mode режим измерения.
 * @return Код ошибки.
 */
extern err_t hmc5883l_read_configuration(hmc5883l_t* compass, hmc5883l_samples_t* samples,
                                         hmc5883l_output_rate_t* output_rate, hmc5883l_measurement_mode_t* measurement_mode);

/**
 * Устанавливает конфигурацию компаса.
 * @param compass Компас.
 * @param samples Число выборок.
 * @param output_rate Частота измерения.
 * @param measurement_mode режим измерения.
 * @return Код ошибки.
 */
extern err_t hmc5883l_configure(hmc5883l_t* compass, hmc5883l_samples_t samples,
                                hmc5883l_output_rate_t output_rate, hmc5883l_measurement_mode_t measurement_mode);

/**
 * Считывает разрешение.
 * @param compass Компас.
 * @param resolution Разрешение.
 * @return Код ошибки.
 */
extern err_t hmc5883l_read_resolution(hmc5883l_t* compass, hmc5883l_resolution_t* resolution);

/**
 * Устанавливает разрешение.
 * @param compass Компас.
 * @param resolution Разрешение.
 * @return Код ошибки.
 */
extern err_t hmc5883l_set_resolution(hmc5883l_t* compass, hmc5883l_resolution_t resolution);

/**
 * Считывает режим работы компаса.
 * @param compass Компас.
 * @param op_mode Режим работы компаса.
 * @param mode_flags Флаги режима работы компаса.
 * @return Код ошибки.
 */
extern err_t hmc5883l_read_mode(hmc5883l_t* compass, hmc5883l_operating_mode_t* op_mode, uint8_t* mode_flags);

/**
 * Устанавливает режим работы компаса.
 * @param compass Компас.
 * @param op_mode Режим работы компаса.
 * @param mode_flags Флаги режима работы компаса.
 * @return Код ошибки.
 */
extern err_t hmc5883l_set_mode(hmc5883l_t* compass, hmc5883l_operating_mode_t op_mode, uint8_t mode_flags);

/**
 * Считывает данные из компаса.
 * Асинхронная операция.
 * @param compass Компас.
 * @return Код ошибки.
 */
extern err_t hmc5883l_read(hmc5883l_t* compass);

/**
 * Вычисляет ориентацию по полученным данным.
 * @param compass Компас.
 */
extern void hmc5883l_calculate(hmc5883l_t* compass);

/**
 * Получает копию вычисленных данных компаса.
 * @param compass Компас.
 * @param data Данные компаса.
 * @return Код ошибки.
 */
extern err_t hmc5883l_data_get(hmc5883l_t* compass, hmc5883l_data_t* data);

/**
 * Получает значение магнитной индукции по оси X.
 * @param compass Компас.
 * @return Значение магнитной индукции по оси X.
 */
extern fixed32_t hmc5883l_compass_x(const hmc5883l_t* compass);

/**
 * Получает значение магнитной индукции по оси Y.
 * @param compass Компас.
 * @return Значение магнитной индукции по оси Y.
 */
extern fixed32_t hmc5883l_compass_y(const hmc5883l_t* compass);

/**
 * Получает значение магнитной индукции по оси Z.
 * @param compass Компас.
 * @return Значение магнитной индукции по оси Z.
 */
extern fixed32_t hmc5883l_compass_z(const hmc5883l_t* compass);

#endif	/* HMC5883L_H */

