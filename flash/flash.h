/**
 * @file flash.h Библиотека работы со встроенной FLASH-памятью.
 */

#ifndef FLASH_H
#define FLASH_H

#include "defs/defs.h"
#include "errors/errors.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * Получает размер флеш-памяти в килобайтах.
 * @return Размер флеш-памяти.
 */
EXTERN uint32_t flash_size(void);

/**
 * Получение размера страницы флеш-памяти в килобайтах.
 * @return Размер страницы флеш-памяти.
 */
EXTERN uint32_t flash_page_size(void);

/**
 * Получение размера страницы флеш-памяти в байтах.
 * @return Размер страницы флеш-памяти.
 */
EXTERN uint32_t flash_page_size_bytes(void);

/**
 * Получает количество страниц флеш-памяти.
 * @return Количество страниц.
 */
EXTERN uint32_t flash_pages(void);

/**
 * Получает первую страницу флеш-памяти.
 * @return Первую страница флеш-памяти.
 */
EXTERN uint32_t flash_first_page(void);

/**
 * Получает последнюю страницу флеш-памяти.
 * @return Последняя страница флеш-памяти.
 */
EXTERN uint32_t flash_last_page(void);

/**
 * Получает адрес начала флеш-памяти.
 * @return Адрес начала флеш-памяти.
 */
EXTERN uint32_t flash_begin_address(void);

/**
 * Получает адрес конца флеш-памяти.
 * @return Адрес конца флеш-памяти.
 */
EXTERN uint32_t flash_end_address(void);

/**
 * Получает адрес заданной страницы флеш-памяти.
 * @param page Номер страницы флеш-памяти.
 * @return Адрес страницы флеш-памяти.
 */
EXTERN uint32_t flash_page_address(uint32_t page);

/**
 * Получает флаг блокировки флеш-памяти.
 * @return Флаг блокировки флеш-памяти.
 */
EXTERN bool flash_locked(void);

/**
 * Разблокирует флеш-память.
 * @return Флаг успеха.
 */
EXTERN bool flash_unlock(void);

/**
 * Блокирует флеш-память.
 */
EXTERN void flash_lock(void);

/**
 * Стирает страницу памяти.
 * @param address Адрес страницы памяти.
 * @return Флаг успеха.
 */
EXTERN bool flash_page_erase(uint32_t address);

/**
 * Стирает все страницы памяти.
 * @return Флаг успеха.
 */
EXTERN bool flash_mass_erase(void);

/**
 * Записывает полуслово в память.
 * @param address Адрес.
 * @param data Данные.
 * @return Флаг успеха.
 */
EXTERN bool flash_program(uint32_t address, uint16_t data);

/**
 * Читает значение во флеш-памяти по заданному адресу.
 * @param address Адрес.
 * @return Значение.
 */
EXTERN uint16_t flash_read(uint32_t address);

#endif /* FLASH_H */

