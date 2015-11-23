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
#define CONCAT_SIMPLE(a, ...) a ## __VA_ARGS__
#define CONCAT_SIMPLE3(a, b, ...) a ## b ## __VA_ARGS__
#define CONCAT(a, ...) CONCAT_SIMPLE(a, __VA_ARGS__)

#define IS_NULL_ARG_IS_NULL 1, 1,
#define IS_NULL_ARGS(t, res, ...) res
#define IS_NULL_IMPL(...) IS_NULL_ARGS(__VA_ARGS__)
#define IS_NULL_ARG(n) IS_NULL_IMPL(n, 0)
#define IS_NULL(n) IS_NULL_IMPL(IS_NULL_ARG_IS_ ## n, 0)

#define IF(cond) CONCAT_SIMPLE(IF_, cond)
#define IF_1(if_true, ...) if_true
#define IF_0(if_true, ...) __VA_ARGS__

#define IF_NOT(cond) CONCAT_SIMPLE(IF_NOT_, cond)
#define IF_NOT_1(if_true, ...) __VA_ARGS__
#define IF_NOT_0(if_true, ...) if_true

#endif	/* DEFS_H */

