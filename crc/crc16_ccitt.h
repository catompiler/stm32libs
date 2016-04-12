#ifndef CRC16_CCITT_H
#define CRC16_CCITT_H

#include <stdint.h>
#include <stddef.h>

/**
 * Вычисляет CRC16 CCITT переданных данных.
 * Полином x^16 + x^12 + x^5 + 1 (0x1021).
 * @param data Данные.
 * @param size Размер данных.
 * @return CRC16.
 */
extern uint16_t crc16_ccitt(const void* data, size_t size);

/**
 * Получает начальное значение CRC16 CCITT.
 * @return Начальное значение CRC16.
 */
extern uint16_t crc16_ccitt_first(void);

/**
 * Вычисляет CRC16 CCITT для очередного байта данных.
 * @param crc Предыдущее значение CRC16.
 * @param data Указатель на очередной байт данных.
 * @return CRC16.
 */
extern uint16_t crc16_ccitt_next(uint16_t crc, const void* data);

#endif  //CRC16_CCITT_H
