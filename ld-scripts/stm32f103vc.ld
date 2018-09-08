/*
 * Скрипт компоновщика для stm32.
 * Часть задания адресов и областей памяти.
 */

/* Точка входа. */
ENTRY(Reset_Handler)

/* Память. */
MEMORY
{
    BOOT (rx) : ORIGIN = 0x08000000,                LENGTH =   0K                /* Загрузчик. */
    APP  (rx) : ORIGIN = 0x08000000 + LENGTH(BOOT), LENGTH = 256K - LENGTH(BOOT) /* Флеш-память. */
    RAM (rwx) : ORIGIN = 0x20000000,                LENGTH =  48K                /* ОЗУ */
}

/* Синоним региона. */
REGION_ALIAS("FLASH", APP);

/* Минимальный размер стека. */
_min_stack_size = 0x200;

/* Конец стека. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Адрес начала сегмента основного приложения. */
_app_origin = ORIGIN(APP);

/* Декларация секций. */
INCLUDE "stm32f10x_sections.ld"
