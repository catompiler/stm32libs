/**
 * @file critical.h Библиотека критических секций.
 */
#ifndef CRITICAL_H
#define CRITICAL_H

#define CRITICAL_ENTER() __asm__ volatile ("cpsid i")
#define CRITICAL_EXIT() __asm__ volatile ("cpsie i")

#endif /* CRITICAL_H */

