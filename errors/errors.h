/**
 * @file errors.h
 * Тип и общие коды ошибок.
 */

#ifndef ERRORS_H
#define	ERRORS_H

#include <stdint.h>

//! Тип ошибки.
typedef uint8_t err_t;

//! Отсутствие ошибки.
#define E_NO_ERROR 0
//! Нулевой указатель.
#define E_NULL_POINTER 1
//! Неправильное значение.
#define E_INVALID_VALUE 2
//! Выход за допустимые пределы.
#define E_OUT_OF_RANGE 3
//! Занято.
#define E_BUSY 4
//! Ошибка ввода-вывода.
#define E_IO_ERROR 5
//! Нехватка памяти.
#define E_OUT_OF_MEMORY 6
//! Нереализованный функционал.
#define E_NOT_IMPLEMENTED 7
//! Ошибка контрольной суммы.
#define E_CRC 8
//! Ошибка состояния.
#define E_STATE 9
//! Ошибка тайм-аута.
#define E_TIME_OUT 10

//! Начальный код для пользовательских ошибок.
#define E_USER 100

#endif	/* ERRORS_H */

