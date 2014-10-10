/**
 * @file one_wire.h
 * Библиотека для работы с шиной 1-wire.
 */

#ifndef ONE_WIRE_H
#define	ONE_WIRE_H

#include "stm32f10x.h"
#include "errors/errors.h"
#include <stdint.h>
#include <stddef.h>

//Коды ошибок.
#define E_ONE_WIRE                      (E_USER + 10)
#define E_ONE_WIRE_INVALID_CRC          (E_ONE_WIRE + 1)
#define E_ONE_WIRE_SEARCH_LOGIC_ERROR   (E_ONE_WIRE + 2)
#define E_ONE_WIRE_DEVICES_NOT_FOUND    (E_ONE_WIRE + 3)


//Коды команд
//Поиск устройств.
#define ONE_WIRE_CMD_SEARCH_ROM         0xf0
//Чтения идентификатора ROM.
#define ONE_WIRE_CMD_READ_ROM           0x33
//Выбор устройства.
#define ONE_WIRE_CMD_MATCH_ROM          0x55
//Игнорирование идентификаторов.
#define ONE_WIRE_CMD_SKIP_ROM           0xcc


/**
 * Структура шины 1-wire.
 */
typedef struct _One_Wire{
    //Порт шины.
    GPIO_TypeDef* GPIO;
    //Пин порта шины.
    uint16_t GPIO_Pin;
}one_wire_t;

/**
 * Структура идентификатора ROM устройства на шине 1-wire.
 */
#define ONE_WIRE_SERIAL_LEN 6
#pragma pack(push, 1)
typedef struct _Rom_Id{
    //Код типа устройства.
    uint8_t family_code;
    //48-битный серийный номер.
    uint8_t serial[ONE_WIRE_SERIAL_LEN];
    //Контрольная сумма.
    uint8_t crc;
}one_wire_rom_id_t;
#pragma pack(pop)

/**
 * Инициализирует структуру шины 1-wire.
 * @param ow Структура шины 1-wire.
 * @param port_n Номер порта.
 * @param pin_n Номер пина порта.
 * @return Код ошибки.
 */
extern err_t one_wire_init(one_wire_t* ow, GPIO_TypeDef* GPIO, uint16_t GPIO_Pin);

/**
 * Сбрасывает устройства на шине 1-wire.
 * @param ow Шина 1-wire.
 * @return 1 в случае наличия устройств на шине, иначе 0.
 */
extern uint8_t one_wire_reset(one_wire_t* ow);

/**
 * Записывает бит в шину 1-wire.
 * @param ow Шина 1-wire.
 * @param bit Бит, имеет значение только нулевой бит.
 */
extern void one_wire_write_bit(one_wire_t* ow, uint8_t bit);

/**
 * Считывает бит из шины 1-wire.
 * @param ow Шина 1-wire.
 * @return Бит, в нулевом бите.
 */
extern uint8_t one_wire_read_bit(one_wire_t* ow);

/**
 * Записывает байт в шину 1-wire.
 * @param ow Шина 1-wire.
 * @param byte Байт.
 */
extern void one_wire_write_byte(one_wire_t* ow, uint8_t byte);

/**
 * Считывает байт из шины 1-wire.
 * @param ow Шина 1-wire.
 * @return Байт.
 */
extern uint8_t one_wire_read_byte(one_wire_t* ow);

/**
 * Записывает данные в шину 1-wire.
 * @param ow Шина 1-wire.
 * @param data Данные.
 * @param size Размер данных.
 */
extern void one_wire_write(one_wire_t* ow, const void* data, size_t size);

/**
 * Считывает данные из шины 1-wire.
 * @param ow Шина 1-wire.
 * @param data Данные.
 * @param size Размер данных.
 */
extern void one_wire_read(one_wire_t* ow, void* data, size_t size);

/**
 * Вычисляет контрольную сумму данных.
 * @param data Данные.
 * @param size Размер данных.
 * @return Контрольная сумма.
 */
extern uint8_t one_wire_calc_crc(const void* data, size_t size);

/**
 * Алиас для one_wire_write_byte().
 */
#define one_wire_send_cmd(ow, cmd) one_wire_write_byte(ow, cmd)

/**
 * Читает идентификатор устройства на шине 1-wire.
 * @param ow Шина 1-wire.
 * @param rom Идентификатор устройства.
 * @return Код ошибки.
 */
extern err_t one_wire_read_rom(one_wire_t* ow, one_wire_rom_id_t* rom);

/**
 * Выбирает устройство с заданным идентификатором на шине 1-wire.
 * @param ow Шина 1-wire.
 * @param rom Идентификатор устройства.
 * @return Код ошибки.
 */
extern err_t one_wire_match_rom(one_wire_t* ow, one_wire_rom_id_t* rom);

/**
 * Заставляет устройства на шине 1-wire игнорировать адресацию.
 * @param ow Шина 1-wire.
 * @return Код ошибки.
 */
extern err_t one_wire_skip_rom(one_wire_t* ow);

/**
 * Ищет устройства на шине 1-wire.
 * @param ow Шина 1-wire.
 * @param roms Идентификаторы устройств.
 * @param roms_count Число идентификаторов.
 * @param roms_found Число найденных устройств.
 * @param max_attempts Число повторов при ошибке для каждого устройства.
 * @return Код ошибки.
 */
extern err_t one_wire_search_roms(one_wire_t* ow, one_wire_rom_id_t* roms,
                                  uint8_t roms_count, uint8_t* roms_found,
                                  uint8_t max_attempts);

#endif	/* ONE_WIRE_H */
