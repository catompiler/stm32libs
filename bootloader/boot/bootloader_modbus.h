/**
 * @file bootloader_modbus.h Интерфейс Modbus загрузчика.
 */

#ifndef BOOTLOADER_MODBUS_H
#define BOOTLOADER_MODBUS_H

#include "defs/defs.h"
#include "errors/errors.h"
#include "modbus/modbus_rtu.h"
#include <stdint.h>


//! Функция обратного вызова запуска приложения.
typedef void (*bootloader_run_app_callback_t)(void);


/**
 * Инициализирует интерфейс Modbus загрузчика.
 * @return Код ошибки.
 */
EXTERN err_t bootloader_modbus_init(void);

/**
 * Устанавливает функцию обратного вызова запуска приложения.
 * @param callback Функция обратного вызова запуска приложения.
 */
EXTERN void bootloader_modbus_set_run_app_callback(bootloader_run_app_callback_t callback);

/**
 * Настраивает Modbus для работы с загрузчиком.
 * @param modbus Модбас.
 * @return Код ошибки.
 */
EXTERN err_t bootloader_modbus_setup_modbus(modbus_rtu_t* modbus);

#endif /* BOOTLOADER_MODBUS_H */

