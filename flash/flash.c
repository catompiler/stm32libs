#include "flash.h"
#include <stm32f10x.h>

//! Размер страницы флеш-памяти.
// Если семейство Low/Medium.
#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL) ||\
    defined(STM32F10X_MD) || defined(STM32F10X_MD_VL)\
    // Размер страницы - 1 кб.
    #define PAGE_SIZE_KB 1
    #define PAGE_SIZE_B  1024
#else
// Если семейство High / Connectivity.
#if defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) ||\
    defined(STM32F10X_XL) || defined(STM32F10X_CL)\
    // Размер страницы - 2 кб.
    #define PAGE_SIZE_KB 2
    #define PAGE_SIZE_B  2048
#else
    #error Unknown device family!
#endif
#endif

//! Адрес регистра размера флеш-памяти.
#define F_SIZE_ADDRESS 0x1ffff7e0

// Ключи для работы с флеш-памятью.
//! Первый ключ разблокировки.
#define FLASH_KEY1 0x45670123
//! Второй ключ разблокировки.
#define FLASH_KEY2 0xCDEF89AB
//! RDPRT.
#define FLASH_RDPRT_KEY 0x00a5



uint32_t flash_size(void)
{
    return *((uint16_t*)F_SIZE_ADDRESS);
}

uint32_t flash_page_size(void)
{
    return PAGE_SIZE_KB;
}

uint32_t flash_page_size_bytes(void)
{
    return PAGE_SIZE_B;
}

uint32_t flash_pages(void)
{
    return flash_size() / flash_page_size();
}

uint32_t flash_first_page(void)
{
    return 0;
}

uint32_t flash_last_page(void)
{
    uint32_t pages = flash_pages();
    
    return (pages == 0) ? 0 : (pages - 1);
}

uint32_t flash_begin_address(void)
{
    return FLASH_BASE;
}

uint32_t flash_end_address(void)
{
    return FLASH_BASE + flash_size() * 1024;
}

uint32_t flash_page_address(uint32_t page)
{
    return FLASH_BASE + page * flash_page_size_bytes();
}

bool flash_locked(void)
{
    return (FLASH->CR & FLASH_CR_LOCK) != 0;
}

bool flash_unlock(void)
{
    // Запись последовательно двух ключей.
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    
    return (FLASH->CR & FLASH_CR_LOCK) == 0;
}

void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

bool flash_page_erase(uint32_t address)
{
    // Убеждаемся что нет операций с флешем.
    while(FLASH->SR & FLASH_SR_BSY);
    
    // Бит стирания страницы.
    FLASH->CR |= FLASH_CR_PER;
    
    // Адрес страницы.
    FLASH->AR = address;
    
    // Запуск операции.
    FLASH->CR |= FLASH_CR_STRT;
    
    // Подождём завершения операции.
    while(FLASH->SR & FLASH_SR_BSY);
    
    // Снятие бита стирания страницы.
    FLASH->CR &= ~FLASH_CR_PER;
    
    // Проверить содержимое страницы.
    // Адрес конца страницы.
    uint32_t page_end = address + PAGE_SIZE_B;
    // Пока мы не достигли конца страницы.
    while(address < page_end){
        // Если текущие 16 бит памяти не стёрты - возврат ошибки.
        if(*((uint16_t*)address) != 0xffff) return false;
        // Увеличиваем адрес на 2 байта.
        address += sizeof(uint16_t);
    }
    
    // Страница успешно стёрта.
    return true;
}

bool flash_mass_erase(void)
{
    // Убеждаемся что нет операций с флешем.
    while(FLASH->SR & FLASH_SR_BSY);
    
    // Бит стирания страниц.
    FLASH->CR |= FLASH_CR_MER;
    
    // Запуск операции.
    FLASH->CR |= FLASH_CR_STRT;
    
    // Подождём завершения операции.
    while(FLASH->SR & FLASH_SR_BSY);
    
    // Снятие бита стирания страниц.
    FLASH->CR &= ~FLASH_CR_MER;
    
    // Проверить содержимое флеш-памяти.
    // Начало флеш-памяти.
    uint32_t address = FLASH_BASE;
    // Адрес конца флеш-памяти.
    uint32_t page_end = address + flash_size() * 1024;
    // Пока мы не достигли конца памяти.
    while(address < page_end){
        // Если текущие 16 бит памяти не стёрты - возврат ошибки.
        if(*((uint16_t*)address) != 0xffff) return false;
        // Увеличиваем адрес на 2 байта.
        address += sizeof(uint16_t);
    }
    
    // Страница успешно стёрта.
    return true;
}

bool flash_program(uint32_t address, uint16_t data)
{
    // Убеждаемся что нет операций с флешем.
    while(FLASH->SR & FLASH_SR_BSY);
    
    // Бит программирования памяти.
    FLASH->CR |= FLASH_CR_PG;
    
    // Запись данных.
    *((uint16_t*)address) = data;
    
    // Подождём завершения операции.
    while(FLASH->SR & FLASH_SR_BSY);
    
    // Сброс бита программирования.
    FLASH->CR &= ~FLASH_CR_PG;
    
    // Если данные записались некорректно - возврат ошибки.
    if(*((uint16_t*)address) != data) return false;
    
    // Данные успешно записаны.
    return true;
}

uint16_t flash_read(uint32_t address)
{
    return *((uint16_t*)address);
}
