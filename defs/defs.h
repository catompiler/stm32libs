/**
 * @file defs.h
 * Общие часто встречающиеся макросы.
 */

#ifndef DEFS_H
#define	DEFS_H

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

#define ALWAYS_INLINE inline  __attribute__((always_inline))

#define WEAK __attribute__((weak))

#define STRING(s) #s
#define CONCAT(a, b) a ## b
#define CONCAT3(a, b, c) a ## b ## c

#endif	/* DEFS_H */

