/**
 * @file dma.h Утилиты по работе с DMA.
 */

#ifndef DMA_H
#define	DMA_H

#include <stm32f10x.h>
#include <stdint.h>
#include <stdbool.h>
#include "errors/errors.h"

/**
 * Получает номер канала DMA, 1-7 для DMA1 и 8-12 для DMA2.
 * @param dma_channel Канал DMA.
 * @return Номер канала DMA, либо 0 в случае ошибки.
 */
extern uint32_t dma_channel_number(DMA_Channel_TypeDef* dma_channel);

/**
 * Получает заданный флаг прерывания для заданного канала.
 * @param dma_channel Канал DMA.
 * @param dma_it_flag Флаг прерывания (DMA_IT_TC, DMA_IT_HT, DMA_IT_TE).
 * @return Флаг прерывания для заданного канала, 0 в случае ошибки.
 */
extern uint32_t dma_channel_it_flag(DMA_Channel_TypeDef* dma_channel, uint32_t dma_it_flag);

/**
 * Ждёт разблокировки канала DMA и блокирует его.
 * @param dma_channel Канал DMA.
 * @return Код ошибки.
 */
extern err_t dma_channel_lock(DMA_Channel_TypeDef* dma_channel);

/**
 * Пытается заблокировать канал DMA.
 * @param dma_channel Кнала DMA.
 * @return true в случае блокировки канала, иначе false.
 */
extern bool dma_channel_trylock(DMA_Channel_TypeDef* dma_channel);

/**
 * Разблокирует канал DMA.
 * @param dma_channel Канал DMA.
 * @return Код ошибки.
 */
extern err_t dma_channel_unlock(DMA_Channel_TypeDef* dma_channel);

#endif	/* DMA_H */

