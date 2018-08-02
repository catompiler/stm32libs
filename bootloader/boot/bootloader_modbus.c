#include "bootloader_modbus.h"
#include "flash/flash.h"
#include <string.h>


// Регистры ввода.
//! Базовый адрес регистров ввода.
#define BOOT_MODBUS_INPUT_REG_BASE 0x1
//! Регистр с размером FLASH-памяти.
#define BOOT_MODBUS_INPUT_REG_FLASH_SIZE (BOOT_MODBUS_INPUT_REG_BASE + 0)
//! Регистр с размером страницы FLASH-памяти.
#define BOOT_MODBUS_INPUT_REG_FLASH_PAGE_SIZE (BOOT_MODBUS_INPUT_REG_BASE + 1)
// Регистры хранения.
//! Базовый адрес регистров хранения.
#define BOOT_MODBUS_HOLD_REG_BASE 0x1
//! Регистр номера страницы.
#define BOOT_MODBUS_HOLD_REG_PAGE_NUMBER (BOOT_MODBUS_HOLD_REG_BASE + 0)
// Флаги.
//! Базовый адрес флагов.
#define BOOT_MODBUS_COIL_BASE 0x1
//! Флаг стирания страницы по адресу в регистре BOOT_MODBUS_HOLD_REG_PAGE_ADDRESS.
#define BOOT_MODBUS_COIL_PAGE_ERASE (BOOT_MODBUS_COIL_BASE + 0)
//! Флаг запуска приложения.
#define BOOT_MODBUS_COIL_RUN_APP (BOOT_MODBUS_COIL_BASE + 1)
// Файлы.
//! Базовый адрес файлов.
#define BOOT_MODBUS_FILE_BASE 0x1
//! Файл текущей страницы памяти.
#define BOOT_MODBUS_FILE_PAGE (BOOT_MODBUS_FILE_BASE + 0)



//! Структура интерфейса Modbus загрузчика.
typedef struct _Bootloader_Modbus {
    modbus_rtu_t* modbus; //!< Интерфейс Modbus RTU.
    uint16_t page_number; //!< Номер страницы флеш-памяти.
    bootloader_run_app_callback_t run_app_callback; //!< Функция обратного вызова запуска приложения.
} bootloader_modbus_t;

//! Интерфейс Modbus загрузчика.
static bootloader_modbus_t boot_modbus;


/*
 * Общие функции.
 */

// Установка номера страницы.
//static void boot_modbus_set_page(uint16_t page)
//{
//}


/*
 * Каллбэки Modbus.
 */

// Чтение флага.
modbus_rtu_error_t boot_modbus_on_read_coil(uint16_t address, modbus_rtu_coil_value_t* value)
{
    switch(address){
        default:
            return MODBUS_RTU_ERROR_INVALID_ADDRESS;
        case BOOT_MODBUS_COIL_PAGE_ERASE:
            *value = MODBUS_RTU_COIL_OFF;
            break;
    }
    
    return MODBUS_RTU_ERROR_NONE;
}

// Запись флага.
modbus_rtu_error_t boot_modbus_on_write_coil(uint16_t address, modbus_rtu_coil_value_t value)
{
    switch(address){
        default:
            return MODBUS_RTU_ERROR_INVALID_ADDRESS;
        case BOOT_MODBUS_COIL_PAGE_ERASE:
            if(value == MODBUS_RTU_COIL_ON){
                if(!flash_unlock()) return MODBUS_RTU_ERROR_NONRECOVERABLE;
                if(!flash_page_erase(flash_page_address(boot_modbus.page_number))){
                    flash_lock();
                    return MODBUS_RTU_ERROR_NONRECOVERABLE;
                }
                flash_lock();
            }
            break;
        case BOOT_MODBUS_COIL_RUN_APP:
            if(value == MODBUS_RTU_COIL_ON){
                if(!boot_modbus.run_app_callback) return MODBUS_RTU_ERROR_NONRECOVERABLE;
                boot_modbus.run_app_callback();
            }
            break;
    }
    
    return MODBUS_RTU_ERROR_NONE;
}

// Чтение регистра ввода.
modbus_rtu_error_t boot_modbus_on_read_input_reg(uint16_t address, uint16_t* value)
{
    switch(address){
        default:
            return MODBUS_RTU_ERROR_INVALID_ADDRESS;
        case BOOT_MODBUS_INPUT_REG_FLASH_SIZE:
            *value = flash_size();
            break;
        case BOOT_MODBUS_INPUT_REG_FLASH_PAGE_SIZE:
            *value = flash_page_size_bytes();
            break;
    }
    
    return MODBUS_RTU_ERROR_NONE;
}

// Чтение регистра хранения.
modbus_rtu_error_t boot_modbus_on_read_hold_reg(uint16_t address, uint16_t* value)
{
    switch(address){
        default:
            return MODBUS_RTU_ERROR_INVALID_ADDRESS;
        case BOOT_MODBUS_HOLD_REG_PAGE_NUMBER:
            *value = boot_modbus.page_number;
            break;
    }
    
    return MODBUS_RTU_ERROR_NONE;
}

// Запись регистра хранения.
modbus_rtu_error_t boot_modbus_on_write_hold_reg(uint16_t address, uint16_t value)
{
    switch(address){
        default:
            return MODBUS_RTU_ERROR_INVALID_ADDRESS;
        case BOOT_MODBUS_HOLD_REG_PAGE_NUMBER:
            boot_modbus.page_number = value;
            break;
    }
    
    return MODBUS_RTU_ERROR_NONE;
}

//! Чтение файла.
modbus_rtu_error_t boot_modbus_rtu_on_read_file_record(uint16_t file, uint16_t record, uint16_t count, uint16_t* values)
{
    switch(file){
        default:
            return MODBUS_RTU_ERROR_INVALID_ADDRESS;
        case BOOT_MODBUS_FILE_PAGE:{
            if((record + count) > (flash_page_size_bytes() / 2)) return MODBUS_RTU_ERROR_INVALID_ADDRESS;
            uint32_t address = flash_page_address(boot_modbus.page_number) + (uint32_t)record * sizeof(uint16_t);
            uint16_t i;
            for(i = 0; i < count; i ++, address += 2){
                values[i] = flash_read(address);
            }
            }break;
    }
    
    return MODBUS_RTU_ERROR_NONE;
}

//! Запись файла.
modbus_rtu_error_t boot_modbus_rtu_on_write_file_record(uint16_t file, uint16_t record, uint16_t count, const uint16_t* values)
{
    switch(file){
        default:
            return MODBUS_RTU_ERROR_INVALID_ADDRESS;
        case BOOT_MODBUS_FILE_PAGE:{
            if((record + count) > (flash_page_size_bytes() / 2)) return MODBUS_RTU_ERROR_INVALID_ADDRESS;
            if(!flash_unlock()) return MODBUS_RTU_ERROR_NONRECOVERABLE;
            uint32_t address = flash_page_address(boot_modbus.page_number) + (uint32_t)record * sizeof(uint16_t);
            uint16_t i;
            for(i = 0; i < count; i ++, address += 2){
                if(!flash_program(address, values[i])){
                    flash_lock();
                    return MODBUS_RTU_ERROR_NONRECOVERABLE;
                }
            }
            flash_lock();
            }break;
    }
    
    return MODBUS_RTU_ERROR_NONE;
}


err_t bootloader_modbus_init(void)
{
    memset(&boot_modbus, 0x0, sizeof(bootloader_modbus_t));
    
    return E_NO_ERROR;
}

void bootloader_modbus_set_run_app_callback(bootloader_run_app_callback_t callback)
{
    boot_modbus.run_app_callback = callback;
}

err_t bootloader_modbus_setup_modbus(modbus_rtu_t* modbus)
{
    if(modbus == NULL) return E_NULL_POINTER;
    
    modbus_rtu_set_read_coil_callback(modbus, boot_modbus_on_read_coil);
    modbus_rtu_set_write_coil_callback(modbus, boot_modbus_on_write_coil);
    modbus_rtu_set_read_input_reg_callback(modbus, boot_modbus_on_read_input_reg);
    modbus_rtu_set_read_holding_reg_callback(modbus, boot_modbus_on_read_hold_reg);
    modbus_rtu_set_write_holding_reg_callback(modbus, boot_modbus_on_write_hold_reg);
    modbus_rtu_set_read_file_record_callback(modbus, boot_modbus_rtu_on_read_file_record);
    modbus_rtu_set_write_file_record_callback(modbus, boot_modbus_rtu_on_write_file_record);
    
    return E_NO_ERROR;
}
