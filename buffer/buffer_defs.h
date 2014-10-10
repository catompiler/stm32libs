/**
 * @file buffer_defs.h
 * Макросы для работы с буфером.
 */

#ifndef BUFFER_DEFS_H
#define	BUFFER_DEFS_H

/**
 * Имеется ли данные после позиции P в буфере размера S.
 */
#define BUFFER_POS_HAS_NEXT(P, S) (P < S)

/**
 * Находится ли позиция P в конце буфера размера S.
 */
#define BUFFER_POS_AT_END(P, S) (P == S)

/**
 * Находится ли позиция P перед концом буфера размера S.
 */
#define BUFFER_POS_AT_LAST(P, S) ((P + 1) == S)

/**
 * Получает значение буфера с адресом PTR на позиции P.
 */
#define BUFFER_POS_GET(PTR, P) (PTR[P])

/**
 * Получает значение буфера с адресом PTR на позиции P со смещением O.
 */
#define BUFFER_POS_GET_OFFSET(PTR, P, O) (PTR[P + O])

/**
 * Устанавливает значение V буфера с адресом PTR на позиции P.
 */
#define BUFFER_POS_SET(PTR, P, V) (PTR[P] = V)

/**
 * Устанавливает значение V буфера с адресом PTR на позиции P со смещением O.
 */
#define BUFFER_POS_SET_OFFSET(PTR, P, V) (PTR[P + O] = V)

#endif	/* BUFFER_DEFS_H */

