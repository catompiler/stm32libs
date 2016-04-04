#include "usart_bus.h"
#include "utils/utils.h"
#include "dma/dma.h"


/*
 * Работа с DMA.
 */

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
    dma_channel_deinit(usart->dma_tx_channel);
    dma_channel_unlock(usart->dma_tx_channel);
    usart->dma_tx_locked = false;
}

static void usart_bus_dma_unlock_rx_channel(usart_bus_t* usart)
{
    dma_channel_deinit(usart->dma_rx_channel);
    dma_channel_unlock(usart->dma_rx_channel);
    usart->dma_rx_locked = false;
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

static void usart_bus_dma_start_rx(usart_bus_t* usart)
{
    usart->usart_device->CR3 |= USART_CR3_DMAR;
    usart->dma_rx_channel->CCR |= DMA_CCR1_EN;
}

static void usart_bus_dma_start_tx(usart_bus_t* usart)
{
    usart->usart_device->CR3 |= USART_CR3_DMAT;
    usart->dma_tx_channel->CCR |= DMA_CCR1_EN;
}

static void usart_bus_dma_stop_rx(usart_bus_t* usart)
{
    usart->dma_rx_channel->CCR &= ~DMA_CCR1_EN;
    usart->usart_device->CR3 &= ~USART_CR3_DMAR;
}

static void usart_bus_dma_stop_tx(usart_bus_t* usart)
{
    usart->dma_tx_channel->CCR &= ~DMA_CCR1_EN;
    usart->usart_device->CR3 &= ~USART_CR3_DMAT;
}

static void usart_bus_dma_rx_done(usart_bus_t* usart)
{
    usart_bus_dma_stop_rx(usart);

    usart->rx_status = USART_STATUS_TRANSFERED;
    usart->rx_size -= usart->dma_rx_channel->CNDTR;
    
    usart_bus_dma_unlock_rx_channel(usart);

    USART_ITConfig(usart->usart_device, USART_IT_RXNE, ENABLE);
}

static void usart_bus_dma_rx_error(usart_bus_t* usart)
{
    usart_bus_dma_stop_rx(usart);

    usart->rx_status = USART_STATUS_ERROR;
    usart->rx_error = USART_ERROR_DMA;
    usart->rx_size -= usart->dma_rx_channel->CNDTR;
    
    usart_bus_dma_unlock_rx_channel(usart);

    USART_ITConfig(usart->usart_device, USART_IT_RXNE, ENABLE);
}

static void usart_bus_dma_tx_done(usart_bus_t* usart)
{
    usart_bus_dma_stop_tx(usart);

    usart->tx_status = USART_STATUS_TRANSFERED;
    usart->tx_size -= usart->dma_tx_channel->CNDTR;
    
    usart_bus_dma_unlock_tx_channel(usart);
}

static void usart_bus_dma_tx_error(usart_bus_t* usart)
{
    usart_bus_dma_stop_tx(usart);

    usart->tx_status = USART_STATUS_ERROR;
    usart->tx_error = USART_ERROR_DMA;
    usart->tx_size -= usart->dma_tx_channel->CNDTR;
    
    usart_bus_dma_unlock_tx_channel(usart);
}

/*
 * Функции - помощники.
 */

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

/*
 * Основной функционал.
 */

err_t usart_bus_init(usart_bus_t* usart, usart_bus_init_t* usart_bus_is)
{
    // Адрес периферии не может быть нулём.
    if(usart_bus_is->usart_device == NULL) return E_NULL_POINTER;
    
    usart->usart_device = usart_bus_is->usart_device;
    usart->dma_tx_channel = usart_bus_is->dma_tx_channel;
    usart->dma_rx_channel = usart_bus_is->dma_rx_channel;
    
    usart->rx_byte_callback = NULL;
    usart->rx_callback = NULL;
    usart->tx_callback = NULL;
    usart->dma_rx_locked = false;
    usart->dma_tx_locked = false;
    usart->rx_status = USART_STATUS_IDLE;
    usart->tx_status = USART_STATUS_IDLE;
    usart->rx_error = USART_ERROR_NONE;
    usart->tx_error = USART_ERROR_NONE;
    
    usart->rx_transfer_id = USART_BUS_DEFAULT_TRANSFER_ID;
    usart->tx_transfer_id = USART_BUS_DEFAULT_TRANSFER_ID;
    
    usart->idle_mode = USART_IDLE_MODE_NONE;
    
    USART_WakeUpConfig(usart->usart_device, USART_WakeUp_IdleLine);
    
    /*
    USART_ITConfig(usart->usart_device, USART_IT_PE, ENABLE);
    USART_ITConfig(usart->usart_device, USART_IT_ORE, ENABLE);
    USART_ITConfig(usart->usart_device, USART_IT_NE, ENABLE);
    USART_ITConfig(usart->usart_device, USART_IT_FE, ENABLE);
    USART_ITConfig(usart->usart_device, USART_IT_ERR, ENABLE);
    */
    
    if(usart_bus_receiver_state(usart->usart_device) != DISABLE){
        USART_ITConfig(usart->usart_device, USART_IT_RXNE, ENABLE);
    }
    
    return E_NO_ERROR;
}

static ALWAYS_INLINE void usart_bus_rx_done(usart_bus_t* usart)
{
    if(usart->rx_callback) usart->rx_callback();
}

static ALWAYS_INLINE void usart_bus_tx_done(usart_bus_t* usart)
{
    if(usart->tx_callback) usart->tx_callback();
}

void usart_bus_irq_handler(usart_bus_t* usart)
{
    uint16_t SR = usart->usart_device->SR;
    uint8_t byte = usart->usart_device->DR;
    
    // Получен IDLE при приёме данных.
    if(SR & USART_SR_IDLE){
        if(usart->idle_mode == USART_IDLE_MODE_END_RX &&
           usart->rx_status == USART_STATUS_TRANSFERING){
            
            usart_bus_dma_rx_done(usart);
            
            usart_bus_rx_done(usart);
        }
    }
    // Получен байт.
    if(SR & USART_SR_RXNE){
        if(usart->rx_byte_callback) usart->rx_byte_callback(byte);
    }
}

bool usart_bus_dma_rx_channel_irq_handler(usart_bus_t* usart)
{
    if(usart->rx_status != USART_STATUS_TRANSFERING) return false;
    
    bool can_rx = usart_bus_transmitter_state(usart->usart_device) == ENABLE;
    
    if(!can_rx || !usart->dma_rx_locked) return false;
    
    uint32_t dma_tc_flag = dma_channel_it_flag(usart->dma_rx_channel, DMA_IT_TC);
    uint32_t dma_te_flag = dma_channel_it_flag(usart->dma_rx_channel, DMA_IT_TE);

    if(DMA_GetITStatus(dma_tc_flag)){

        DMA_ClearITPendingBit(dma_tc_flag);

        usart_bus_dma_rx_done(usart);
        
        usart_bus_rx_done(usart);

    }else if(DMA_GetITStatus(dma_te_flag)){

        DMA_ClearITPendingBit(dma_te_flag);

        usart_bus_dma_rx_error(usart);
        
        usart_bus_rx_done(usart);
    }
    
    return true;
}

bool usart_bus_dma_tx_channel_irq_handler(usart_bus_t* usart)
{
    if(usart->tx_status != USART_STATUS_TRANSFERING) return false;
    
    bool can_tx = usart_bus_transmitter_state(usart->usart_device) == ENABLE;
    
    if(!can_tx || !usart->dma_tx_locked) return false;
    
    uint32_t dma_tc_flag = dma_channel_it_flag(usart->dma_tx_channel, DMA_IT_TC);
    uint32_t dma_te_flag = dma_channel_it_flag(usart->dma_tx_channel, DMA_IT_TE);

    if(DMA_GetITStatus(dma_tc_flag)){

        DMA_ClearITPendingBit(dma_tc_flag);

        usart_bus_dma_tx_done(usart);
        
        usart_bus_tx_done(usart);

    }else if(DMA_GetITStatus(dma_te_flag)){

        DMA_ClearITPendingBit(dma_te_flag);

        usart_bus_dma_tx_error(usart);
        
        usart_bus_tx_done(usart);
    }
    
    return true;
}

bool usart_bus_rx_busy(usart_bus_t* usart)
{
    return usart->rx_status == USART_STATUS_TRANSFERING;
}

bool usart_bus_tx_busy(usart_bus_t* usart)
{
    return usart->tx_status == USART_STATUS_TRANSFERING;
}

void usart_bus_rx_wait(usart_bus_t* usart)
{
    WAIT_WHILE_TRUE(usart_bus_rx_busy(usart));
}

void usart_bus_tx_wait(usart_bus_t* usart)
{
    WAIT_WHILE_TRUE(usart_bus_tx_busy(usart));
}

usart_transfer_id_t usart_bus_rx_transfer_id(usart_bus_t* usart)
{
    return usart->rx_transfer_id;
}

bool usart_bus_set_rx_transfer_id(usart_bus_t* usart, usart_transfer_id_t id)
{
    if(usart_bus_rx_busy(usart)) return false;
    usart->rx_transfer_id = id;
    return true;
}

usart_transfer_id_t usart_bus_tx_transfer_id(usart_bus_t* usart)
{
    return usart->tx_transfer_id;
}

bool usart_bus_set_tx_transfer_id(usart_bus_t* usart, usart_transfer_id_t id)
{
    if(usart_bus_tx_busy(usart)) return false;
    usart->tx_transfer_id = id;
    return true;
}

usart_bus_rx_byte_callback_t usart_bus_rx_byte_callback(usart_bus_t* usart)
{
    return usart->rx_byte_callback;
}

void usart_bus_set_rx_byte_callback(usart_bus_t* usart, usart_bus_rx_byte_callback_t callback)
{
    usart->rx_byte_callback = callback;
}

usart_bus_callback_t usart_bus_rx_callback(usart_bus_t* usart)
{
    return usart->rx_callback;
}

void usart_bus_set_rx_callback(usart_bus_t* usart, usart_bus_callback_t callback)
{
    usart->rx_callback = callback;
}

usart_bus_callback_t usart_bus_tx_callback(usart_bus_t* usart)
{
    return usart->tx_callback;
}

void usart_bus_set_tx_callback(usart_bus_t* usart, usart_bus_callback_t callback)
{
    usart->tx_callback = callback;
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

size_t usart_bus_bytes_received(usart_bus_t* usart)
{
    return usart->rx_size;
}

size_t usart_bus_bytes_transmitted(usart_bus_t* usart)
{
    return usart->tx_size;
}

usart_idle_mode_t usart_bus_idle_mode(usart_bus_t* usart)
{
    return usart->idle_mode;
}

void usart_bus_set_idle_mode(usart_bus_t* usart, usart_idle_mode_t mode)
{
    usart->idle_mode = mode;
    if(mode == USART_IDLE_MODE_NONE){
        USART_ITConfig(usart->usart_device, USART_IT_IDLE, DISABLE);
    }else{
        USART_ITConfig(usart->usart_device, USART_IT_IDLE, ENABLE);
    }
}

void usart_bus_sleep(usart_bus_t* usart)
{
    USART_ReceiverWakeUpCmd(usart->usart_device, ENABLE);
}

void usart_bus_wake(usart_bus_t* usart)
{
    USART_ReceiverWakeUpCmd(usart->usart_device, DISABLE);
}

err_t usart_bus_send(usart_bus_t* usart, const void* data, size_t size)
{
    if(usart_bus_tx_busy(usart)) return E_BUSY;
    if(data == NULL) return E_NULL_POINTER;
    if(size == 0) return E_INVALID_VALUE;
    if(size > UINT16_MAX) return E_OUT_OF_RANGE;
    
    if(!usart_bus_dma_lock_tx_channel(usart)) return E_BUSY;
    
    usart->tx_error = E_NO_ERROR;
    usart->tx_status = USART_STATUS_TRANSFERING;
    usart->tx_size = size;
    
    usart_bus_dma_tx_config(usart, (void*)data, size);
    
    usart_bus_dma_start_tx(usart);
    
    return E_NO_ERROR;
}

err_t usart_bus_recv(usart_bus_t* usart, void* data, size_t size)
{
    if(usart_bus_rx_busy(usart)) return E_BUSY;
    if(data == NULL) return E_NULL_POINTER;
    if(size == 0) return E_INVALID_VALUE;
    if(size > UINT16_MAX) return E_OUT_OF_RANGE;
    
    if(!usart_bus_dma_lock_rx_channel(usart)) return E_BUSY;
    if(usart_bus_receiver_state(usart->usart_device) != ENABLE) return E_BUSY;
    
    USART_ITConfig(usart->usart_device, USART_IT_RXNE, DISABLE);
    
    usart->rx_error = E_NO_ERROR;
    usart->rx_status = USART_STATUS_TRANSFERING;
    usart->rx_size = size;
    
    usart_bus_dma_rx_config(usart, data, size);
    
    usart_bus_dma_start_rx(usart);
    
    return E_NO_ERROR;
}

