/**
 * @file bits.h
 * Макросы для работы с битами.
 */

#ifndef BITS_H
#define	BITS_H

#include "defs/defs.h"

#ifndef BIT_SET_TEST_0BIT
#define BIT_SET_TEST_0BIT 1
#endif

/**
 * Формирует число с установленным битом
 * в заданной позиции B.
 */
#define BIT(B) (1 << (B))
/**
 * Формирует число с битом, равным V
 * в заданной позиции B.
 */
#if BIT_SET_TEST_0BIT == 1
#define BIT_BYVAL(B, V) ((V & 0x1) << B)
#else
#define BIT_BYVAL(B, V) ((V      ) << B)
#endif
/**
 * Устанавливает бит номер B в переменной P.
 */
#define BIT_ON(P, B) do{(P) |= BIT(B);}while(0)
/**
 * Сбрасывает бит номер B в переменной P.
 */
#define BIT_OFF(P, B) do{(P) &= ~BIT(B);}while(0)
/**
 * Изменяет бит номер B в переменной P.
 */
#define BIT_TOGGLE(P, B) do{(P) ^= BIT(B);}while(0)
/**
 * Устанавливает бит номер B в переменной P в значение V.
 */
#if BIT_SET_TEST_0BIT == 1
    #define BIT_SET(P, B, V) do{ if( (V) & 0x1 ){ BIT_ON(P, B); }else{ BIT_OFF(P, B); } }while(0)
#else
    #define BIT_SET(P, B, V) do{ if( (V)       ){ BIT_ON(P, B); }else{ BIT_OFF(P, B); } }while(0)
#endif
/**
 * Инвертирует нулевой бит в числе P.
 */
#define BIT0_NOT(P) ((P) ^ 1)
/**
 * Проверяет бит номер B в переменной P.
 */
#define BIT_TEST(P, B) ((P) & BIT(B))
/**
 * Возвращает сырое значение бита B в переменной P.
 */
#define BIT_RAW_VALUE(P, B) ((P) & BIT(B))
/**
 * Ждёт установки бита B в переменной P.
 */
#define BIT_WAIT_ON(P, B) do{}while(!BIT_TEST(P, B))
/**
 * Ждёт снятия бита B в переменной P.
 */
#define BIT_WAIT_OFF(P, B) do{}while(BIT_TEST(P, B))
/**
 * Возвращает значение бита B в переменной P.
 */
#define BIT_VALUE(P, B) ((BIT_TEST(P, B)) ? 0x1 : 0x0)
/**
 * Устанавливает биты, соответствущие маске.
 */
#define BIT_ON_MASK(P, M) do{(P) |= (M);}while(0)
/**
 * Сбрасывает биты, соответствующие маске.
 */
#define BIT_OFF_MASK(P, M) do{(P) &= ~(M);}while(0)
/**
 * Изменяет биты, соответствующие маске.
 */
#define BIT_TOGGLE_MASK(P, M) do{(P) ^= (M);}while(0)
/**
 * Устанавливает биты, соответсвующие маске M, в переменной P в значение V.
 */
#if BIT_SET_TEST_0BIT == 1
    #define BIT_SET_MASK(P, M, V) do{ if( (V) & 0x1 ){ BIT_ON_MASK(P, M); }else{ BIT_OFF_MASK(P, M); } }while(0)
#else
    #define BIT_SET_MASK(P, M, V) do{ if( (V)       ){ BIT_ON_MASK(P, M); }else{ BIT_OFF_MASK(P, M); } }while(0)
#endif
/**
 * Проверяет биты, соответствующие маске.
 */
#define BIT_TEST_MASK(P, M) ((P) & (M))
/**
 * Возвращает наличие хотябы одного установленного бита по маске M в переменной P.
 */
#define BIT_TEST_VALUE_MASK(P, M) ((BIT_TEST_MASK(P, M)) ? 0x1 : 0x0)
/**
 * Формирует маску длиной N бит и смещением S бит.
 */
#define BIT_MAKE_MASK(N, S) ((BIT(N) - 1) << (S))

/**
 * Устанавливает бит в массиве.
 * @param bits Массив.
 * @param bit_n Номер бита.
 */
ALWAYS_INLINE static void bits_on(uint8_t* bits, uint8_t bit_n)
{
    BIT_ON(bits[bit_n >> 3], bit_n & 0x7);
}

/**
 * Сбрасывает бит в массиве.
 * @param bits Массив.
 * @param bit_n Номер бита.
 */
ALWAYS_INLINE static void bits_off(uint8_t* bits, uint8_t bit_n)
{
    BIT_OFF(bits[bit_n >> 3], bit_n & 0x7);
}

/**
 * Возвращает бит в массиве.
 * @param bits Массив.
 * @param bit_n Номер бита.
 * @return Бит.
 */
ALWAYS_INLINE static uint8_t bits_value(uint8_t* bits, uint8_t bit_n)
{
    return BIT_VALUE(bits[bit_n >> 3], bit_n & 0x7);
}

/**
 * Устанавливает значение бита в массиве.
 * @param bits Массив.
 * @param bit_n Номер бита.
 * @param bit Значение бита.
 */
static ALWAYS_INLINE void bits_set_value(uint8_t* bits, uint8_t bit_n, uint8_t bit)
{
    uint8_t byte_n = bit_n >> 3;
    uint8_t byte_bit_n = bit_n & 0x7;
    
    BIT_SET(bits[byte_n], byte_bit_n, bit);
}

#endif	/* BITS_H */

