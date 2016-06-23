/**
 * @file utf.h Библиотека работы с utf8.
 */

#ifndef UTF8_H
#define	UTF8_H

#include <stddef.h>
#include <stdbool.h>
#include "defs/defs.h"


/**
 * Получает размер символа строки utf8.
 * @param str_c Символ utf8.
 * @return Размер символа строки utf8.
 */
EXTERN size_t utf8_str_char_size(const char* str_c);

/**
 * Получает размер символа utf8.
 * @param c Код символа utf8.
 * @return Размер символа utf8.
 */
EXTERN size_t utf8_char_size(wchar_t c);

/**
 * Декодирует символ строки utf8.
 * @param str_c Символ строки utf8.
 * @return Код символа utf8.
 */
EXTERN wchar_t utf8_char_decode(const char* str_c);

/**
 * Кодирует символ в строку utf8.
 * @param str_c Строка utf8.
 * @param c Код символа utf8.
 * @return Размер символа utf8.
 */
EXTERN size_t utf8_char_encode(char* str_c, wchar_t c);

/**
 * Верифицирует символа строки utf8.
 * @param str_c Символ utf8.
 * @return true если символ валиден, иначе false.
 */
EXTERN bool utf8_char_validate(const char* str_c);

#endif	/* UTF8_H */
