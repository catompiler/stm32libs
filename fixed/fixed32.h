/**
 * @file fixed32.h
 * Библиотека для работы с 32 битными числами с фиксированной запятой.
 */

#ifndef FIXED32_H
#define	FIXED32_H

#include "fixed.h"

/*
 * fixed32.
 * 32 бит.
 * 16 бит + 16 бит.
 */
#define FIXED32_SIZE_BITS  32
#define FIXED32_INT_BITS   16
#define FIXED32_FRACT_BITS 16

//! Тип fixed_int32_t.
typedef fixed_make_stdint_type(FIXED32_SIZE_BITS)       fixed_make_int_type(  FIXED32_SIZE_BITS);
//! Тип fixed32_t.
typedef fixed_make_stdint_type(FIXED32_SIZE_BITS)       fixed_make_type(      FIXED32_SIZE_BITS);

/*
 * Макросы для fixed32.
 * Описание см. fixed.h (@see fixed.h).
 */
#define fixed32_make_from_int(int_n)                    fixed_make_from_int_bits(int_n,                 FIXED32_FRACT_BITS)
#define fixed32_make_from_fract(dividend, divider)      fixed_make_from_fract_bits(dividend, divider,   FIXED32_FRACT_BITS)
#define fixed32_round(f)                                fixed_round_bits(f,                             FIXED32_INT_BITS, FIXED32_FRACT_BITS)
#define fixed32_get_int(f)                              fixed_get_int_bits(f,                           FIXED32_FRACT_BITS)
#define fixed32_get_fract_denom()                       fixed_get_fract_denom_bits(                     FIXED32_FRACT_BITS)
#define fixed32_get_fract(f)                            fixed_get_fract_bits(f,                         FIXED32_FRACT_BITS)
#define fixed32_get_fract_by_denom(f, base)             fixed_get_fract_by_denom_bits(f, base,          FIXED32_FRACT_BITS)
#define fixed32_mul(f1, f2)                             fixed_mul_bits(f1, f2,                          FIXED32_FRACT_BITS)
#define fixed32_div(f1, f2)                             fixed_div_bits(f1, f2,                          FIXED32_FRACT_BITS)

/*
 * Макросы по-умолчанию.
 */
#if defined(FIXED32_IS_DEFAULT) || defined(FIXED32_DEFAULT)
typedef fixed32_t       fixed_t;
typedef fixed_int32_t   fixed_int_t;
#define fixed_make_from_int(int_n)                    fixed32_make_from_int(int_n)
#define fixed_make_from_fract(dividend, divider)      fixed32_make_from_fract(dividend, divider)
#define fixed_round(f)                                fixed32_round(f)
#define fixed_get_int(f)                              fixed32_get_int(f)
#define fixed_get_fract_denom()                       fixed32_get_fract_denom()
#define fixed_get_fract(f)                            fixed32_get_fract(f)
#define fixed_get_fract_by_denom(f, base)             fixed32_get_fract_by_denom(f, base)
#define fixed_mul(f1, f2)                             fixed32_mul(f1, f2)
#define fixed_div(f1, f2)                             fixed32_div(f1, f2)
#endif


/*
 * fixed24_8.
 * 32 бит.
 * 24 бит + 8 бит.
 */
#define FIXED24_8_SIZE_BITS  32
#define FIXED24_8_INT_BITS   24
#define FIXED24_8_FRACT_BITS 8

//! Тип fixed_int24_8_t.
typedef fixed_make_stdint_type(FIXED24_8_SIZE_BITS)       fixed_make_int_type2( FIXED24_8_INT_BITS, FIXED24_8_FRACT_BITS);
//! Тип fixed24_8_t.
typedef fixed_make_stdint_type(FIXED24_8_SIZE_BITS)       fixed_make_type2(     FIXED24_8_INT_BITS, FIXED24_8_FRACT_BITS);

/*
 * Макросы для fixed24_8.
 * Описание см. fixed.h (@see fixed.h).
 */
#define fixed24_8_make_from_int(int_n)                    fixed_make_from_int_bits(int_n,                 FIXED24_8_FRACT_BITS)
#define fixed24_8_make_from_fract(dividend, divider)      fixed_make_from_fract_bits(dividend, divider,   FIXED24_8_FRACT_BITS)
#define fixed24_8_round(f)                                fixed_round_bits(f,                             FIXED24_8_INT_BITS, FIXED24_8_FRACT_BITS)
#define fixed24_8_get_int(f)                              fixed_get_int_bits(f,                           FIXED24_8_FRACT_BITS)
#define fixed24_8_get_fract_denom()                       fixed_get_fract_denom_bits(                     FIXED24_8_FRACT_BITS)
#define fixed24_8_get_fract(f)                            fixed_get_fract_bits(f,                         FIXED24_8_FRACT_BITS)
#define fixed24_8_get_fract_by_denom(f, base)             fixed_get_fract_by_denom_bits(f, base,          FIXED24_8_FRACT_BITS)
#define fixed24_8_mul(f1, f2)                             fixed_mul_bits(f1, f2,                          FIXED24_8_FRACT_BITS)
#define fixed24_8_div(f1, f2)                             fixed_div_bits(f1, f2,                          FIXED24_8_FRACT_BITS)

/*
 * Макросы по-умолчанию.
 */
#if defined(FIXED24_8_IS_DEFAULT) || defined(FIXED24_8_DEFAULT)
typedef fixed24_8_t       fixed_t;
typedef fixed_int24_8_t   fixed_int_t;
#define fixed_make_from_int(int_n)                    fixed24_8_make_from_int(int_n)
#define fixed_make_from_fract(dividend, divider)      fixed24_8_make_from_fract(dividend, divider)
#define fixed_round(f)                                fixed24_8_round(f)
#define fixed_get_int(f)                              fixed24_8_get_int(f)
#define fixed_get_fract_denom()                       fixed24_8_get_fract_denom()
#define fixed_get_fract(f)                            fixed24_8_get_fract(f)
#define fixed_get_fract_by_denom(f, base)             fixed24_8_get_fract_by_denom(f, base)
#define fixed_mul(f1, f2)                             fixed24_8_mul(f1, f2)
#define fixed_div(f1, f2)                             fixed24_8_div(f1, f2)
#endif


/*
 * fixed8_24.
 * 32 бит.
 * 8 бит + 24 бит.
 */
#define FIXED8_24_SIZE_BITS  32
#define FIXED8_24_INT_BITS   8
#define FIXED8_24_FRACT_BITS 24

//! Тип fixed_int8_24_t.
typedef fixed_make_stdint_type(FIXED8_24_SIZE_BITS)       fixed_make_int_type2( FIXED8_24_INT_BITS, FIXED8_24_FRACT_BITS);
//! Тип fixed8_24_t.
typedef fixed_make_stdint_type(FIXED8_24_SIZE_BITS)       fixed_make_type2(     FIXED8_24_INT_BITS, FIXED8_24_FRACT_BITS);

/*
 * Макросы для fixed8_24.
 * Описание см. fixed.h (@see fixed.h).
 */
#define fixed8_24_make_from_int(int_n)                    fixed_make_from_int_bits(int_n,                 FIXED8_24_FRACT_BITS)
#define fixed8_24_make_from_fract(dividend, divider)      fixed_make_from_fract_bits(dividend, divider,   FIXED8_24_FRACT_BITS)
#define fixed8_24_round(f)                                fixed_round_bits(f,                             FIXED8_24_INT_BITS, FIXED8_24_FRACT_BITS)
#define fixed8_24_get_int(f)                              fixed_get_int_bits(f,                           FIXED8_24_FRACT_BITS)
#define fixed8_24_get_fract_denom()                       fixed_get_fract_denom_bits(                     FIXED8_24_FRACT_BITS)
#define fixed8_24_get_fract(f)                            fixed_get_fract_bits(f,                         FIXED8_24_FRACT_BITS)
#define fixed8_24_get_fract_by_denom(f, base)             fixed_get_fract_by_denom_bits(f, base,          FIXED8_24_FRACT_BITS)
#define fixed8_24_mul(f1, f2)                             fixed_mul_bits(f1, f2,                          FIXED8_24_FRACT_BITS)
#define fixed8_24_div(f1, f2)                             fixed_div_bits(f1, f2,                          FIXED8_24_FRACT_BITS)

/*
 * Макросы по-умолчанию.
 */
#if defined(FIXED8_24_IS_DEFAULT) || defined(FIXED8_24_DEFAULT)
typedef fixed8_24_t       fixed_t;
typedef fixed_int8_24_t   fixed_int_t;
#define fixed_make_from_int(int_n)                    fixed8_24_make_from_int(int_n)
#define fixed_make_from_fract(dividend, divider)      fixed8_24_make_from_fract(dividend, divider)
#define fixed_round(f)                                fixed8_24_round(f)
#define fixed_get_int(f)                              fixed8_24_get_int(f)
#define fixed_get_fract_denom()                       fixed8_24_get_fract_denom()
#define fixed_get_fract(f)                            fixed8_24_get_fract(f)
#define fixed_get_fract_by_denom(f, base)             fixed8_24_get_fract_by_denom(f, base)
#define fixed_mul(f1, f2)                             fixed8_24_mul(f1, f2)
#define fixed_div(f1, f2)                             fixed8_24_div(f1, f2)
#endif

#endif	/* FIXED32_H */
