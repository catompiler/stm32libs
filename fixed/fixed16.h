/**
 * @file fixed16.h
 * Библиотека для работы с 16 битными числами с фиксированной запятой.
 */

#ifndef FIXED16_H
#define	FIXED16_H

#include "fixed.h"

/*
 * fixed16.
 * 16 бит.
 * 8 бит + 8 бит.
 */
#define FIXED16_SIZE_BITS  16
#define FIXED16_INT_BITS   8
#define FIXED16_FRACT_BITS 8

//! Тип fixed_int16_t.
typedef fixed_make_stdint_type(FIXED16_SIZE_BITS)       fixed_make_int_type(  FIXED16_SIZE_BITS);
//! Тип fixed16_t.
typedef fixed_make_stdint_type(FIXED16_SIZE_BITS)       fixed_make_type(      FIXED16_SIZE_BITS);

/*
 * Макросы для fixed16.
 * Описание см. fixed.h (@see fixed.h).
 */
#define fixed16_make_from_int(int_n)                    fixed_make_from_int_bits(int_n,                 FIXED16_FRACT_BITS)
#define fixed16_make_from_fract(dividend, divider)      fixed_make_from_fract_bits(dividend, divider,   FIXED16_FRACT_BITS)
#define fixed16_round(f)                                fixed_round_bits(f,                             FIXED16_INT_BITS, FIXED16_FRACT_BITS)
#define fixed16_get_int(f)                              fixed_get_int_bits(f,                           FIXED16_FRACT_BITS)
#define fixed16_get_fract_denom()                       fixed_get_fract_denom_bits(                     FIXED16_FRACT_BITS)
#define fixed16_get_fract(f)                            fixed_get_fract_bits(f,                         FIXED16_FRACT_BITS)
#define fixed16_get_fract_by_denom(f, base)             fixed_get_fract_by_denom_bits(f, base,          FIXED16_FRACT_BITS)
#define fixed16_mul(f1, f2)                             fixed_mul_bits(f1, f2,                          FIXED16_FRACT_BITS)
#define fixed16_div(f1, f2)                             fixed_div_bits(f1, f2,                          FIXED16_FRACT_BITS)

/*
 * Макросы по-умолчанию.
 */
#if defined(FIXED16_IS_DEFAULT) || defined(FIXED16_DEFAULT)
typedef fixed16_t       fixed_t;
typedef fixed_int16_t   fixed_int_t;
#define fixed_make_from_int(int_n)                    fixed16_make_from_int(int_n)
#define fixed_make_from_fract(dividend, divider)      fixed16_make_from_fract(dividend, divider)
#define fixed_round(f)                                fixed16_round(f)
#define fixed_get_int(f)                              fixed16_get_int(f)
#define fixed_get_fract_denom()                       fixed16_get_fract_denom()
#define fixed_get_fract(f)                            fixed16_get_fract(f)
#define fixed_get_fract_by_denom(f, base)             fixed16_get_fract_by_denom(f, base)
#define fixed_mul(f1, f2)                             fixed16_mul(f1, f2)
#define fixed_div(f1, f2)                             fixed16_div(f1, f2)
#endif


/*
 * fixed10_6.
 * 16 бит.
 * 10 бит + 6 бит.
 */
#define FIXED10_6_SIZE_BITS  16
#define FIXED10_6_INT_BITS   10
#define FIXED10_6_FRACT_BITS 6

//! Тип fixed_int10_6_t.
typedef fixed_make_stdint_type(FIXED10_6_SIZE_BITS)       fixed_make_int_type2( FIXED10_6_INT_BITS, FIXED10_6_FRACT_BITS);
//! Тип fixed10_6_t.
typedef fixed_make_stdint_type(FIXED10_6_SIZE_BITS)       fixed_make_type2(     FIXED10_6_INT_BITS, FIXED10_6_FRACT_BITS);

/*
 * Макросы для fixed10_6.
 * Описание см. fixed.h (@see fixed.h).
 */
#define fixed10_6_make_from_int(int_n)                    fixed_make_from_int_bits(int_n,                 FIXED10_6_FRACT_BITS)
#define fixed10_6_make_from_fract(dividend, divider)      fixed_make_from_fract_bits(dividend, divider,   FIXED10_6_FRACT_BITS)
#define fixed10_6_round(f)                                fixed_round_bits(f,                             FIXED10_6_INT_BITS, FIXED10_6_FRACT_BITS)
#define fixed10_6_get_int(f)                              fixed_get_int_bits(f,                           FIXED10_6_FRACT_BITS)
#define fixed10_6_get_fract_denom()                       fixed_get_fract_denom_bits(                     FIXED10_6_FRACT_BITS)
#define fixed10_6_get_fract(f)                            fixed_get_fract_bits(f,                         FIXED10_6_FRACT_BITS)
#define fixed10_6_get_fract_by_denom(f, base)             fixed_get_fract_by_denom_bits(f, base,          FIXED10_6_FRACT_BITS)
#define fixed10_6_mul(f1, f2)                             fixed_mul_bits(f1, f2,                          FIXED10_6_FRACT_BITS)
#define fixed10_6_div(f1, f2)                             fixed_div_bits(f1, f2,                          FIXED10_6_FRACT_BITS)

/*
 * Макросы по-умолчанию.
 */
#if defined(FIXED10_6_IS_DEFAULT) || defined(FIXED10_6_DEFAULT)
typedef fixed10_6_t       fixed_t;
typedef fixed_int10_6_t   fixed_int_t;
#define fixed_make_from_int(int_n)                    fixed10_6_make_from_int(int_n)
#define fixed_make_from_fract(dividend, divider)      fixed10_6_make_from_fract(dividend, divider)
#define fixed_round(f)                                fixed10_6_round(f)
#define fixed_get_int(f)                              fixed10_6_get_int(f)
#define fixed_get_fract_denom()                       fixed10_6_get_fract_denom()
#define fixed_get_fract(f)                            fixed10_6_get_fract(f)
#define fixed_get_fract_by_denom(f, base)             fixed10_6_get_fract_by_denom(f, base)
#define fixed_mul(f1, f2)                             fixed10_6_mul(f1, f2)
#define fixed_div(f1, f2)                             fixed10_6_div(f1, f2)
#endif


/*
 * fixed6_10.
 * 16 бит.
 * 6 бит + 10 бит.
 */
#define FIXED6_10_SIZE_BITS  16
#define FIXED6_10_INT_BITS   6
#define FIXED6_10_FRACT_BITS 10

//! Тип fixed_int6_10_t.
typedef fixed_make_stdint_type(FIXED6_10_SIZE_BITS)       fixed_make_int_type2( FIXED6_10_INT_BITS, FIXED6_10_FRACT_BITS);
//! Тип fixed6_10_t.
typedef fixed_make_stdint_type(FIXED6_10_SIZE_BITS)       fixed_make_type2(     FIXED6_10_INT_BITS, FIXED6_10_FRACT_BITS);

/*
 * Макросы для fixed6_10.
 * Описание см. fixed.h (@see fixed.h).
 */
#define fixed6_10_make_from_int(int_n)                    fixed_make_from_int_bits(int_n,                 FIXED6_10_FRACT_BITS)
#define fixed6_10_make_from_fract(dividend, divider)      fixed_make_from_fract_bits(dividend, divider,   FIXED6_10_FRACT_BITS)
#define fixed6_10_round(f)                                fixed_round_bits(f,                             FIXED6_10_INT_BITS, FIXED6_10_FRACT_BITS)
#define fixed6_10_get_int(f)                              fixed_get_int_bits(f,                           FIXED6_10_FRACT_BITS)
#define fixed6_10_get_fract_denom()                       fixed_get_fract_denom_bits(                     FIXED6_10_FRACT_BITS)
#define fixed6_10_get_fract(f)                            fixed_get_fract_bits(f,                         FIXED6_10_FRACT_BITS)
#define fixed6_10_get_fract_by_denom(f, base)             fixed_get_fract_by_denom_bits(f, base,          FIXED6_10_FRACT_BITS)
#define fixed6_10_mul(f1, f2)                             fixed_mul_bits(f1, f2,                          FIXED6_10_FRACT_BITS)
#define fixed6_10_div(f1, f2)                             fixed_div_bits(f1, f2,                          FIXED6_10_FRACT_BITS)

/*
 * Макросы по-умолчанию.
 */
#if defined(FIXED6_10_IS_DEFAULT) || defined(FIXED6_10_DEFAULT)
typedef fixed6_10_t       fixed_t;
typedef fixed_int6_10_t   fixed_int_t;
#define fixed_make_from_int(int_n)                    fixed6_10_make_from_int(int_n)
#define fixed_make_from_fract(dividend, divider)      fixed6_10_make_from_fract(dividend, divider)
#define fixed_round(f)                                fixed6_10_round(f)
#define fixed_get_int(f)                              fixed6_10_get_int(f)
#define fixed_get_fract_denom()                       fixed6_10_get_fract_denom()
#define fixed_get_fract(f)                            fixed6_10_get_fract(f)
#define fixed_get_fract_by_denom(f, base)             fixed6_10_get_fract_by_denom(f, base)
#define fixed_mul(f1, f2)                             fixed6_10_mul(f1, f2)
#define fixed_div(f1, f2)                             fixed6_10_div(f1, f2)
#endif


/*
 * fixed12_4.
 * 16 бит.
 * 12 бит + 4 бит.
 */
#define FIXED12_4_SIZE_BITS  16
#define FIXED12_4_INT_BITS   12
#define FIXED12_4_FRACT_BITS 4

//! Тип fixed_int12_4_t.
typedef fixed_make_stdint_type(FIXED12_4_SIZE_BITS)       fixed_make_int_type2( FIXED12_4_INT_BITS, FIXED12_4_FRACT_BITS);
//! Тип fixed12_4_t.
typedef fixed_make_stdint_type(FIXED12_4_SIZE_BITS)       fixed_make_type2(     FIXED12_4_INT_BITS, FIXED12_4_FRACT_BITS);

/*
 * Макросы для fixed12_4.
 * Описание см. fixed.h (@see fixed.h).
 */
#define fixed12_4_make_from_int(int_n)                    fixed_make_from_int_bits(int_n,                 FIXED12_4_FRACT_BITS)
#define fixed12_4_make_from_fract(dividend, divider)      fixed_make_from_fract_bits(dividend, divider,   FIXED12_4_FRACT_BITS)
#define fixed12_4_round(f)                                fixed_round_bits(f,                             FIXED12_4_INT_BITS, FIXED12_4_FRACT_BITS)
#define fixed12_4_get_int(f)                              fixed_get_int_bits(f,                           FIXED12_4_FRACT_BITS)
#define fixed12_4_get_fract_denom()                       fixed_get_fract_denom_bits(                     FIXED12_4_FRACT_BITS)
#define fixed12_4_get_fract(f)                            fixed_get_fract_bits(f,                         FIXED12_4_FRACT_BITS)
#define fixed12_4_get_fract_by_denom(f, base)             fixed_get_fract_by_denom_bits(f, base,          FIXED12_4_FRACT_BITS)
#define fixed12_4_mul(f1, f2)                             fixed_mul_bits(f1, f2,                          FIXED12_4_FRACT_BITS)
#define fixed12_4_div(f1, f2)                             fixed_div_bits(f1, f2,                          FIXED12_4_FRACT_BITS)

/*
 * Макросы по-умолчанию.
 */
#if defined(FIXED12_4_IS_DEFAULT) || defined(FIXED12_4_DEFAULT)
typedef fixed12_4_t       fixed_t;
typedef fixed_int12_4_t   fixed_int_t;
#define fixed_make_from_int(int_n)                    fixed12_4_make_from_int(int_n)
#define fixed_make_from_fract(dividend, divider)      fixed12_4_make_from_fract(dividend, divider)
#define fixed_round(f)                                fixed12_4_round(f)
#define fixed_get_int(f)                              fixed12_4_get_int(f)
#define fixed_get_fract_denom()                       fixed12_4_get_fract_denom()
#define fixed_get_fract(f)                            fixed12_4_get_fract(f)
#define fixed_get_fract_by_denom(f, base)             fixed12_4_get_fract_by_denom(f, base)
#define fixed_mul(f1, f2)                             fixed12_4_mul(f1, f2)
#define fixed_div(f1, f2)                             fixed12_4_div(f1, f2)
#endif


/*
 * fixed4_12.
 * 16 бит.
 * 4 бит + 12 бит.
 */
#define FIXED4_12_SIZE_BITS  16
#define FIXED4_12_INT_BITS   4
#define FIXED4_12_FRACT_BITS 12

//! Тип fixed_int4_12_t.
typedef fixed_make_stdint_type(FIXED4_12_SIZE_BITS)       fixed_make_int_type2( FIXED4_12_INT_BITS, FIXED4_12_FRACT_BITS);
//! Тип fixed4_12_t.
typedef fixed_make_stdint_type(FIXED4_12_SIZE_BITS)       fixed_make_type2(     FIXED4_12_INT_BITS, FIXED4_12_FRACT_BITS);

/*
 * Макросы для fixed4_12.
 * Описание см. fixed.h (@see fixed.h).
 */
#define fixed4_12_make_from_int(int_n)                    fixed_make_from_int_bits(int_n,                 FIXED4_12_FRACT_BITS)
#define fixed4_12_make_from_fract(dividend, divider)      fixed_make_from_fract_bits(dividend, divider,   FIXED4_12_FRACT_BITS)
#define fixed4_12_round(f)                                fixed_round_bits(f,                             FIXED4_12_INT_BITS, FIXED4_12_FRACT_BITS)
#define fixed4_12_get_int(f)                              fixed_get_int_bits(f,                           FIXED4_12_FRACT_BITS)
#define fixed4_12_get_fract_denom()                       fixed_get_fract_denom_bits(                     FIXED4_12_FRACT_BITS)
#define fixed4_12_get_fract(f)                            fixed_get_fract_bits(f,                         FIXED4_12_FRACT_BITS)
#define fixed4_12_get_fract_by_denom(f, base)             fixed_get_fract_by_denom_bits(f, base,          FIXED4_12_FRACT_BITS)
#define fixed4_12_mul(f1, f2)                             fixed_mul_bits(f1, f2,                          FIXED4_12_FRACT_BITS)
#define fixed4_12_div(f1, f2)                             fixed_div_bits(f1, f2,                          FIXED4_12_FRACT_BITS)

/*
 * Макросы по-умолчанию.
 */
#if defined(FIXED4_12_IS_DEFAULT) || defined(FIXED4_12_DEFAULT)
typedef fixed4_12_t       fixed_t;
typedef fixed_int4_12_t   fixed_int_t;
#define fixed_make_from_int(int_n)                    fixed4_12_make_from_int(int_n)
#define fixed_make_from_fract(dividend, divider)      fixed4_12_make_from_fract(dividend, divider)
#define fixed_round(f)                                fixed4_12_round(f)
#define fixed_get_int(f)                              fixed4_12_get_int(f)
#define fixed_get_fract_denom()                       fixed4_12_get_fract_denom()
#define fixed_get_fract(f)                            fixed4_12_get_fract(f)
#define fixed_get_fract_by_denom(f, base)             fixed4_12_get_fract_by_denom(f, base)
#define fixed_mul(f1, f2)                             fixed4_12_mul(f1, f2)
#define fixed_div(f1, f2)                             fixed4_12_div(f1, f2)
#endif

#endif	/* FIXED16_H */

