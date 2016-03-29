#include "usart_bus.h"
#include "utils/utils.h"
#include "dma/dma.h"


/*static ITStatus usart_tx_it_state(USART_TypeDef* usart)
{
    if(usart->CR1 & USART_CR1_TXEIE) return ENABLE;
    return DISABLE;
}

static ITStatus usart_rx_it_state(USART_TypeDef* usart)
{
    if(usart->CR1 & USART_CR1_RXNEIE) return ENABLE;
    return DISABLE;
}*/

FunctionalState usart_bus_transmitter_state(USART_TypeDef* usart)
{
    if(usart->CR1 & USART_CR1_TE) return ENABLE;
    return DISABLE;
}

FunctionalState usart_bus_receiver_state(USART_TypeDef* usart)
{
    if(usart->CR1 & USART_CR1_RE) return ENABLE;
    return DISABLE;
}

FunctionalState usart_bus_halfduplex_state(USART_TypeDef* usart)
{
    if(usart->CR3 & USART_CR3_HDSEL) return ENABLE;
    return DISABLE;
}

err_t usart_bus_init(usart_bus_t* usart, usart_bus_init_t* usart_bus_is)
{
    // Адрес периферии не может быть нулём.
    if(usart_bus_is->usart_device == NULL) return E_NULL_POINTER;
    
    usart->usart_device = usart_bus_is->usart_device;
    usart->dma_tx_channel = usart_bus_is->dma_tx_channel;
    usart->dma_rx_channel = usart_bus_is->dma_rx_channel;
    
    usart->callback = NULL;
    usart->rx_callback = NULL;
    usart->dma_rx_locked = false;
    usart->dma_tx_locked = false;
    usart->rx_status = USART_STATUS_IDLE;
    usart->tx_status = USART_STATUS_IDLE;
    usart->rx_error = USART_ERROR_NONE;
    usart->tx_error = USART_ERROR_NONE;
    
    USART_WakeUpConfig(usart->usart_device, USART_WakeUp_IdleLine);
    
    USART_ITConfig(usart->usart_device, USART_IT_PE, ENABLE);
    USART_ITConfig(usart->usart_device, USART_IT_ORE, ENABLE);
    USART_ITConfig(usart->usart_device, USART_IT_NE, ENABLE);
    USART_ITConfig(usart->usart_device, USART_IT_FE, ENABLE);
    USART_ITConfig(usart->usart_device, USART_IT_ERR, ENABLE);
    
    if((usart_bus_receiver_state(usart->usart_device) != DISABLE)){
        USART_ITConfig(usart->usart_device, USART_IT_RXNE, ENABLE);
    }
    
    return E_NO_ERROR;
}

void usart_bus_irq_handler(usart_bus_t* usart)
{
    if(USART_GetITStatus(usart->usart_device, USART_IT_RXNE) != RESET){
        USART_ClearITPendingBit(usart->usart_device, USART_IT_RXNE);
    }
}

bool usart_bus_rx_busy(usart_bus_t* usart)
{
}

bool usart_bus_tx_busy(usart_bus_t* usart)
{
}

void usart_bus_rx_wait(usart_bus_t* usart)
{
}

void usart_bus_tx_wait(usart_bus_t* usart)
{
}

usart_transfer_id_t usart_bus_rx_transfer_id(usart_bus_t* usart)
{
}

bool usart_bus_set_rx_transfer_id(usart_bus_t* usart, usart_transfer_id_t id)
{
}

usart_transfer_id_t usart_bus_tx_transfer_id(usart_bus_t* usart)
{
}

bool usart_bus_set_tx_transfer_id(usart_bus_t* usart, usart_transfer_id_t id)
{
}

usart_bus_callback_t usart_bus_callback(usart_bus_t* usart)
{
    return usart->callback;
}

void usart_bus_set_callback(usart_bus_t* usart, usart_bus_callback_t callback)
{
    usart->callback = callback;
}

usart_bus_rx_callback_t usart_bus_rx_callback(usart_bus_t* usart)
{
    return usart->rx_callback;
}

void usart_bus_set_rx_callback(usart_bus_t* usart, usart_bus_rx_callback_t callback)
{
    usart->rx_callback = callback;
}

usart_status_t usart_bus_rx_status(usart_bus_t* usart)
{
    return usart->rx_status;
}

usart_status_t usart_bus_tx_status(usart_bus_t* usart)
{
    return usart->tx_status;
}

usart_error_t usart_bus_rx_error(usart_bus_t* usart)
{
    return usart->rx_error;
}

usart_error_t usart_bus_tx_error(usart_bus_t* usart)
{
    return usart->tx_error;
}

void usart_bus_sleep(usart_bus_t* usart)
{
    USART_ReceiverWakeUpCmd(usart->usart_device, ENABLE);
}

void usart_bus_wake(usart_bus_t* usart)
{
    USART_ReceiverWakeUpCmd(usart->usart_device, DISABLE);
}


static void usart_bus_dma_tx_config(usart_bus_t* usart, void* address, size_t size)
{
    usart->dma_tx_channel->CCR &= ~DMA_CCR1_EN;
    usart->dma_tx_channel->CNDTR = size;
    usart->dma_tx_channel->CPAR = (uint32_t)&usart->usart_device->DR;
    usart->dma_tx_channel->CMAR = (uint32_t)address;
    usart->dma_tx_channel->CCR = DMA_CCR1_PL_0 | DMA_CCR1_TEIE | DMA_CCR1_TCIE | DMA_CCR1_DIR | DMA_CCR1_MINC;
}

static void usart_bus_dma_rx_config(usart_bus_t* usart, void* address, size_t size)
{
    usart->dma_rx_channel->CCR &= ~DMA_CCR1_EN;
    usart->dma_rx_channel->CNDTR = size;
    usart->dma_rx_channel->CPAR = (uint32_t)&usart->usart_device->DR;
    usart->dma_rx_channel->CMAR = (uint32_t)address;
    usart->dma_rx_channel->CCR = DMA_CCR1_PL_0 | DMA_CCR1_TEIE | DMA_CCR1_TCIE | DMA_CCR1_MINC;
}

static bool usart_bus_dma_lock_tx_channel(usart_bus_t* usart)
{
    usart->dma_tx_locked = dma_channel_trylock(usart->dma_tx_channel);
    return usart->dma_tx_locked;
}

static bool usart_bus_dma_lock_rx_channel(usart_bus_t* usart)
{
    usart->dma_rx_locked = dma_channel_trylock(usart->dma_rx_channel);
    return usart->dma_rx_locked;
}

static void usart_bus_dma_unlock_tx_channel(usart_bus_t* usart)
{
    if(usart->dma_tx_locked){
        dma_channel_deinit(usart->dma_tx_channel);
        dma_channel_unlock(usart->dma_tx_channel);
        usart->dma_tx_locked = false;
    }
}

static void usart_bus_dma_unlock_rx_channel(usart_bus_t* usart)
{
    if(usart->dma_rx_locked){
        dma_channel_deinit(usart->dma_rx_channel);
        dma_channel_unlock(usart->dma_rx_channel);
        usart->dma_rx_locked = false;
    }
}

static void usart_bus_dma_start_rx(usart_bus_t* usart)
{
    if(usart->dma_rx_locked){
        usart->usart_device->CR3 |= USART_CR3_DMAR;
        usart->dma_rx_channel->CCR |= DMA_CCR1_EN;
    }
}

static void usart_bus_dma_start_tx(usart_bus_t* usart)
{
    if(usart->dma_tx_locked){
        usart->usart_device->CR3 |= USART_CR3_DMAT;
        usart->dma_tx_channel->CCR |= DMA_CCR1_EN;
    }
}

static void usart_bus_dma_stop_rx(usart_bus_t* usart)
{
    if(usart->dma_rx_locked){
        usart->dma_rx_channel->CCR &= ~DMA_CCR1_EN;
        usart->usart_device->CR3 &= ~USART_CR3_DMAR;
    }
}

static void usart_bus_dma_stop_tx(usart_bus_t* usart)
{
    if(usart->dma_tx_locked){
        usart->dma_tx_channel->CCR &= ~DMA_CCR1_EN;
        usart->usart_device->CR3 &= ~USART_CR3_DMAT;
    }
}

err_t usart_bus_write(usart_bus_t* usart, const void* data, size_t size)
{
    return E_NO_ERROR;
}

err_t usart_bus_read(usart_bus_t* usart, void* data, size_t size)
{
    return E_NO_ERROR;
}

