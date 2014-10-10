/**
 * @file ds18x20.h
 * Библиотека для работы с датчиками температуры DS18x20.
 */

#ifndef DS18X20_H
#define	DS18X20_H

#include <stdbool.h>
#include "one_wire/one_wire.h"
#include "errors/errors.h"
#include "fixed/fixed16.h"
#include "bits/bits.h"

#ifndef DS18X20_READ_SCRATCHPAD_FULLY
#define DS18X20_READ_SCRATCHPAD_FULLY 1
#endif

//Команды сенсора.
#define DS18X20_CMD_SCRATCHPAD_READ     0xbe
#define DS18X20_CMD_SCRATCHPAD_WRITE    0x4e
#define DS18X20_CMD_TEMP_START_CONVERT  0x44

//Сигнализируемые значения температуры.
#define DS18X20_ALARM_TEMP_MAX (127)
#define DS18X20_ALARM_TEMP_MIN (-128)

//Разрешение.
//Смещение битов разрешения в регистре конфигурации.
#define DS18X20_RESOLUTION_BITS_OFFSET 5
//Значения разрешения.
#define DS18X20_RESOLUTION_12BIT (3 << DS18X20_RESOLUTION_BITS_OFFSET)
#define DS18X20_RESOLUTION_11BIT (2 << DS18X20_RESOLUTION_BITS_OFFSET)
#define DS18X20_RESOLUTION_10BIT (1 << DS18X20_RESOLUTION_BITS_OFFSET)
#define DS18X20_RESOLUTION_9BIT  (0 << DS18X20_RESOLUTION_BITS_OFFSET)
#define DS18X20_RESOLUTION_MAX  DS18X20_RESOLUTION_12BIT
#define DS18X20_RESOLUTION_MIN  DS18X20_RESOLUTION_9BIT

//Структура сенсора.
typedef struct _Ds18x20 {
    one_wire_t* one_wire;
    one_wire_rom_id_t* rom;
} ds18x20_t;

/**
 * Инициализирует структура сенсора.
 * @param sensor Структура сенсора.
 * @param ow Шина 1-wire.
 * @param rom Идентификатор устройства, NULL если устройство
 * на шине одно и можно игнорировать идентификатор.
 * @return Код ошибки.
 */
extern err_t ds18x20_init(ds18x20_t* sensor, one_wire_t* ow, one_wire_rom_id_t* rom);

/**
 * Выбирает сенсор.
 * @param sensor Сенсор.
 * @return Код ошибки.
 */
extern err_t ds18x20_select(ds18x20_t* sensor);

/**
 * Конфигурирует сенсор.
 * @param resolution Разрешение конвертирования температуры.
 * @param alarm_temp_lo Нижнее сигнализируемое значение температуры.
 * @param alarm_temp_hi Верхнее сигнализируемое значение температуры.
 * @return Код ошибки.
 */
extern err_t ds18x20_configure(ds18x20_t* sensor, uint8_t resolution,
                              int8_t alarm_temp_lo, int8_t alarm_temp_hi);

/**
 * Инициирует конвертирование температуры.
 * @param sensor Сенсор.
 * @return Код ошибки.
 */
extern err_t ds18x20_start_conversion(ds18x20_t* sensor);

/**
 * Проверяет статус конвертирования температуры.
 * @param sensor Сенсор.
 * @return true если конвертирование окончено или не начиналось, иначе false.
 */
extern bool ds18x20_conversion_done(ds18x20_t* sensor);

/**
 * Считывает температуру.
 * @param sensor Сенсор.
 * @param temp указатель на переменную для возвращения температуры.
 * @return Код ошибки.
 */
extern err_t ds18x20_read_temp(ds18x20_t* sensor, fixed16_t* temp);

#endif	/* DS18X20_H */

