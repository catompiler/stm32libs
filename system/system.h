/**
 * @file system.h Библиотека инициализации дерева тактирования.
 */

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <stdint.h>

//! Значение частоты ядра.
extern uint32_t SystemCoreClock;

/**
 * Инициализирует дерево тактирования.
 */
extern void SystemInit(void);

/**
 * Сбрасывает дерево тактирования.
 */
extern void SystemDeInit(void);

#endif  //_SYSTEM_H_
