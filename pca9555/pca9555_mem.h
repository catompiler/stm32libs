/**
 * @file pca9555_mem.h Структура памяти pca9555.
 */

#ifndef PCA9555_MEM_H
#define	PCA9555_MEM_H

#include <stdint.h>

//Структура памяти в pca9555.

//Полная память.
#pragma pack(push, 1)
typedef struct _Pca9555_mem {
    // Входа 0-7.
    uint8_t idr0;
    // Входа 8-15.
    uint8_t idr1;
    // Выхода 0-7.
    uint8_t odr0;
    // Выхода 8-15.
    uint8_t odr1;
    // Инверсия 0-7.
    uint8_t pir0;
    // Инверсия 8-15.
    uint8_t pir1;
    // Конфигурация 0-7.
    uint8_t cr0;
    // Конфигурация 8-15.
    uint8_t cr1;
} pca9555_mem_t;
#pragma pack(pop)

#endif	/* PCA9555_MEM_H */

