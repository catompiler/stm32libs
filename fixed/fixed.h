/**
 * @file fixed.h
 * Библиотека для работы с числами с фиксированной запятой.
 */

#ifndef FIXED_H
#define	FIXED_H

/**
 * Получает имя типа заданной битности.
 * @param prefix Начало имени типа.
 * @param bits Число бит типа.
 */
#define fixed_make_type_impl(prefix, bits) prefix ## bits ## _t

/**
 * Получает имя типа заданной битности для целой и дробной части.
 * @param prefix Начало имени типа.
 * @param intbits Число бит целой части.
 * @param fractbits Число бит дробной части.
 */
#define fixed_make_type_impl2(prefix, intbits, fractbits)\
                        fixed_make_type_impl(prefix, intbits ## _ ## fractbits)

/**
 * Получает имя типа stdint заданной битности.
 * @param bits Число бит типа.
 */
#define fixed_make_stdint_type(bits) fixed_make_type_impl(int, bits)

/**
 * Получает имя типа целого для числа с фиксированной запятой заданной битности.
 * @param bits Число бит типа.
 */
#define fixed_make_int_type(bits) fixed_make_type_impl(fixed_int, bits)

/**
 * Получает имя типа целого для числа с фиксированной запятой заданной битности для целой и дробной части.
 * @param intbits Число бит целой части.
 * @param fractbits Число бит дробной части.
 */
#define fixed_make_int_type2(intbits, fractbits) fixed_make_type_impl2(fixed_int, intbits, fractbits)

/**
 * Получает имя типа числа с фиксированной запятой заданной битности.
 * @param bits Число бит типа.
 */
#define fixed_make_type(bits) fixed_make_type_impl(fixed, bits)

/**
 * Получает имя типа числа с фиксированной запятой заданной битности для целой и дробной части.
 * @param intbits Число бит целой части.
 * @param fractbits Число бит дробной части.
 */
#define fixed_make_type2(intbits, fractbits) fixed_make_type_impl2(fixed, intbits, fractbits)

/**
 * Получает число с фиксированной запятой с заданным размером дробной части из целого числа.
 * @param int_n Целое число.
 * @param fractbits Число бит дробной части.
 * @return Число с фиксированной запятой.
 */
#define fixed_make_from_int_bits(int_n, fractbits) ((int_n) << (fractbits))

/**
 * Получает число с фиксированной запятой с заданным размером дробной части из отношения двух целых чисел.
 * @param dividend Целое число, делимое.
 * @param divider Целое число, делитель.
 * @param fractbits Число бит дробной части.
 * @return Число с фиксированной запятой.
 */
#define fixed_make_from_fract_bits(dividend, divider, fractbits) (fixed_make_from_int_bits(dividend, fractbits) / divider)

/**
 * Получает битовую маску заданной размерности.
 * @param bits Число бит.
 * @return Битовая маска.
 */
#define fixed_make_mask(bits) ((bits < 16) ? ((((uint16_t)1) << bits) - 1) : ((((uint32_t)1) << bits) - 1) )

/**
 * Получает модуль числа с фиксированной запятой.
 * @param f Число с фиксированной запятой.
 * @return Модуль числа с фиксированной запятой.
 */
#define fixed_abs(f) ((f < 0) ? (-f) : (f))

/**
 * Получает округлённое значение числа с фиксированной запятой.
 * @param f Число с фиксированной запятой.
 * @param intbits Число бит целой части.
 * @param fractbits Число бит дробной части.
 * @return Округлённое значение числа с фиксированной запятой.
 */
#define fixed_round_bits(f, intbits, fractbits) ((f + (1 << (fractbits - 1))) & (fixed_make_mask(intbits) << fractbits))

/**
 * Получает целую часть числа с фиксированной запятой при заданной размерности дробной части.
 * @param f Число с фиксированной запятой.
 * @param fractbits Число бит дробной части.
 * @return Целая часть числа с фиксированной запятой.
 */
#define fixed_get_int_bits(f, fractbits)\
                ((f < 0) ?\
                        (-((-f) >> (fractbits))) :\
                        ((f) >> (fractbits)))

/**
 * Получает знаменатель дробной части заданной размерности.
 * @param fractbits Число бит дробной части.
 * @return Знаменатель дробной части заданной размерности.
 */
#define fixed_get_fract_denom_bits(fractbits) (1 << fractbits)

/**
 * Получает дробную часть числа с фиксированной запятой.
 * @param f Число с фиксированной запятой.
 * @param fractbits Число бит дробной части.
 * @param Дробную часть числа с фиксированной запятой.
 */
#define fixed_get_fract_bits(f, fractbits)\
                ((f < 0) ?\
                        (-((-f) & fixed_make_mask(fractbits))) :\
                        (f & fixed_make_mask(fractbits)))

/**
 * Получает дробную часть числа с фиксированной запятой для заданного знаменателя.
 * @param f Число с фиксированной запятой.
 * @param denom Необходимый знаменатель.
 * @param fractbits Число бит дробной части.
 * @param Дробную часть числа с фиксированной запятой.
 */
#define fixed_get_fract_by_denom_bits(f, denom, fractbits) ((fixed_get_fract_bits(f, fractbits) * (denom)) >> fractbits)

#endif	/* FIXED_H */
