/**
 * @file net.h Функции преобразования порядка байт.
 */

#ifndef NET_H
#define NET_H

#include <stdint.h>
#include "defs/defs.h"

/**
 * Преобразует узловой порядок расположения байтов
 * положительного короткого целого в
 * сетевой порядок расположения байтов.
 * @param hostshort Целое для преобразования.
 * @return Целое в сетевом порядке байт.
 */
ALWAYS_INLINE static uint16_t htons(uint16_t hostshort)
{
    uint16_t res;
    
    __asm__ __volatile__ ("rev16 %0, %1" : "=r"(res) : "r"(hostshort));
    
    return res;
}

/**
 * Преобразует сетевой порядок расположения байтов
 * положительного короткого целого в
 * узловой порядок расположения байтов.
 * @param netshort Целое для преобразования.
 * @return Целое в узловом порядке байт.
 */
ALWAYS_INLINE static uint16_t ntohs(uint16_t netshort)
{
    uint16_t res;
    
    __asm__ __volatile__ ("rev16 %0, %1" : "=r"(res) : "r"(netshort));
    
    return res;
}

/**
 * Преобразует узловой порядок расположения байтов
 * положительного целого в
 * сетевой порядок расположения байтов.
 * @param hostshort Целое для преобразования.
 * @return Целое в сетевом порядке байт.
 */
ALWAYS_INLINE static uint32_t htonl(uint32_t hostlong)
{
    uint32_t res;
    
    __asm__ __volatile__ ("rev %0, %1" : "=r"(res) : "r"(hostlong));
    
    return res;
}

/**
 * Преобразует сетевой порядок расположения байтов
 * положительного целого в
 * узловой порядок расположения байтов.
 * @param netshort Целое для преобразования.
 * @return Целое в узловом порядке байт.
 */
ALWAYS_INLINE static uint32_t ntohl(uint32_t netlong)
{
    uint32_t res;
    
    __asm__ __volatile__ ("rev %0, %1" : "=r"(res) : "r"(netlong));
    
    return res;
}

#endif  //NET_H
