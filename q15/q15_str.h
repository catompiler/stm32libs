/**
 * @file q15_str.h Библиотека строковых функций для чисел с фиксированной запятой.
 */

#ifndef Q15_STR_H_
#define Q15_STR_H_

#include "q15.h"
#include <stddef.h>


/**
 * Преобразует строку в число с фиксированной запятой с целой частью.
 * Устанавливает ERRNO.
 * @param str Строка.
 * @param endp Окончание числа в строке.
 * @return Число с фиксированной запятой.
 */
extern iq15_t iq15_fromstr(const char* str, const char** endp);

/**
 * Преобразует число с фиксированной запятой с целой частью в строку.
 * @param str Строка.
 * @param size Размер строки.
 * @param q Число с фиксированной запятой.
 * @return Число записанных символов.
 */
extern int iq15_tostr(char* str, size_t size, iq15_t q);

#endif /* Q15_STR_H_ */
