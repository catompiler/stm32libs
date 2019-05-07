#include "q15_str.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>
#include <ctype.h>


#define STRTOL_DEFAULT 0
#define STRTOL_BASE 10
#define IQ15_STR_DP '.'
#define IQ15_STR_e 'e'
#define IQ15_STR_E 'E'
#define IQ15_STR_PLUS '+'
#define IQ15_STR_MINUS '-'
#define IQ15_TOSTR_DIVIDER 100000


//! Пропускает пробельные символы.
static char* iq15_str_skip_spaces(char* str)
{
    while(isspace((unsigned char)*str)) str ++;
    return str;
}

//! Преобразует целое число дробной части в фиксированную запятую.
static q15_t iq15_str_fract_to_q(int32_t fract)
{
    int32_t dec_div = STRTOL_BASE;
    while(fract >= dec_div) dec_div *= STRTOL_BASE;
    return (q15_t)LQ15F(fract, dec_div);
}

iq15_t iq15_fromstr(const char* str, const char** endp)
{
    if(str == NULL){
        if(endp) *endp = str;
        return STRTOL_DEFAULT;
    }

    char* ptr = (char*)str;

    lq15_t res = STRTOL_DEFAULT;

    bool is_neg = false;

    long int_part = 0;
    long fract_part = 0;
    long exp_part = 0;

    ptr = iq15_str_skip_spaces(ptr);

    // Знак.
    if(*ptr == IQ15_STR_MINUS){
        is_neg = true;
    }

    // Целая часть.
    int_part = strtol(ptr, &ptr, STRTOL_BASE);
    if(errno != 0){
        if(endp) *endp = ptr;
        return STRTOL_DEFAULT;
    }

    // Если после целой части следует точка.
    if(*ptr == IQ15_STR_DP){
        // Пропуск разделителя дробной части.
        ptr ++;
        // Если следующий символ - цифра.
        if(isdigit((unsigned char)*ptr)){
            // Дробная часть.
            fract_part = strtol(ptr, &ptr, STRTOL_BASE);
            if(errno != 0){
                if(endp) *endp = ptr;
                return STRTOL_DEFAULT;
            }
        }
    }

    // Если после целой и/или дробной части следует экспонента.
    if(*ptr == IQ15_STR_e || *ptr == IQ15_STR_E){
        // Пропуск экспоненты.
        ptr ++;
        // Экспонента.
        exp_part = strtol(ptr, &ptr, STRTOL_BASE);
        if(errno != 0){
            if(endp) *endp = ptr;
            return STRTOL_DEFAULT;
        }
    }

    // Разбор строки закончен.
    if(endp) *endp = ptr;

    // Переведём целое число дробной части в фиксированную запятую.
    fract_part = iq15_str_fract_to_q(fract_part);

    // Целая часть.
    res = LQ15I(int_part);
    // Прибавим согласно знаку дробную часть.
    if(is_neg == false) res += fract_part;
    else                res -= fract_part;
    // Обработаем экспоненту.
    while(exp_part > 0) { res *= 10; exp_part --; }
    while(exp_part < 0) { res /= 10; exp_part ++; }

    // Пределы.
    if(res > INT32_MAX || res < INT32_MIN){
        errno = ERANGE;
        return 0;
    }

    errno = 0;

    return (iq15_t)res;
}

int iq15_tostr(char* str, size_t size, iq15_t q)
{
    if(str == NULL) return -1;
    if(size == 0) return -1;

    const char* fmt = (q >= 0) ? "%u.%05u" : "-%u.%05u";

    // Абсолютное значение.
    q = iq15_abs(q);

    // Целая часть.
    uint32_t int_part = (uint32_t)iq15_int(q);
    // Фиксированная дробная часть.
    uint32_t fract_part = (uint32_t)iq15_fract(q);
    // Преобразуем в числовое представление.
    // fract_part = fract_part * 100000 / 32768
    fract_part = fract_part * IQ15_TOSTR_DIVIDER / Q15_BASE;

    int cnt = snprintf(str, size, fmt, (unsigned int)int_part, (unsigned int)fract_part);

    return cnt;
}

