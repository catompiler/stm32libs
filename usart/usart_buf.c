#include "usart_buf.h"
#include "utils/utils.h"
#include "defs/defs.h"


ALWAYS_INLINE static ITStatus usart_tx_it_state(USART_TypeDef* usart)
{
    if(usart->CR1 & USART_CR1_TXEIE) return ENABLE;
    return DISABLE;
}

ALWAYS_INLINE static ITStatus usart_rx_it_state(USART_TypeDef* usart)
{
    if(usart->CR1 & USART_CR1_RXNEIE) return ENABLE;
    return DISABLE;
}

ALWAYS_INLINE static FunctionalState usart_transmitter_state(USART_TypeDef* usart)
{
    if(usart->CR1 & USART_CR1_TE) return ENABLE;
    return DISABLE;
}

ALWAYS_INLINE static FunctionalState usart_receiver_state(USART_TypeDef* usart)
{
    if(usart->CR1 & USART_CR1_RE) return ENABLE;
    return DISABLE;
}

err_t usart_buf_init(usart_buf_t* usart_buf, usart_buf_init_t* usart_buf_init)
{
    // Адрес периферии не может быть нулём.
    if(usart_buf_init->usart == NULL) return E_NULL_POINTER;
    
    // Если буфера нет.
    if(usart_buf_init->write_buffer == NULL){
        // То и его размер должен быть нулём.
        if(usart_buf_init->write_buffer_size != 0) return E_NULL_POINTER;
    }else{
        // Иначе размер не может буть нулевым.
        if(usart_buf_init->write_buffer_size == 0) return E_INVALID_VALUE;
    }
    
    // Если буфера нет.
    if(usart_buf_init->read_buffer == NULL){
        // То и его размер должен быть нулём.
        if(usart_buf_init->read_buffer_size != 0) return E_NULL_POINTER;
    }else{
        // Иначе размер не может буть нулевым.
        if(usart_buf_init->read_buffer_size == 0) return E_INVALID_VALUE;
    }
    
    usart_buf->usart = usart_buf_init->usart;
    
    usart_buf->data_overrun = false;
    
    circular_buffer_init(&usart_buf->write_buffer, (uint8_t*)usart_buf_init->write_buffer, usart_buf_init->write_buffer_size);
    circular_buffer_init(&usart_buf->read_buffer,  (uint8_t*)usart_buf_init->read_buffer,  usart_buf_init->read_buffer_size);
    
    if((usart_receiver_state(usart_buf->usart) != DISABLE) &&
            circular_buffer_valid(&usart_buf->read_buffer)){
        USART_ITConfig(usart_buf->usart, USART_IT_RXNE, ENABLE);
    }
    
    return E_NO_ERROR;
}

/*static bool usart_buf_send_byte(usart_buf_t* usart_buf)
{
    uint8_t data;
    if(circular_buffer_get(&usart_buf->write_buffer, &data)){
        USART_SendData(usart_buf->usart, data);
        return true;
    }
    return false;
}*/

void usart_buf_irq_handler(usart_buf_t* usart_buf)
{
    if(USART_GetITStatus(usart_buf->usart, USART_IT_RXNE) != RESET){
        USART_ClearITPendingBit(usart_buf->usart, USART_IT_RXNE);
        
        uint8_t data = USART_ReceiveData(usart_buf->usart);
        if(!circular_buffer_put(&usart_buf->read_buffer, data)){
            usart_buf->data_overrun = true;
        }
    }
    if(USART_GetITStatus(usart_buf->usart, USART_IT_TXE) != RESET){
        // Cleared only by write to USARTx->DR.
        //USART_ClearITPendingBit(usart_buf->usart, USART_IT_TXE);
        
        uint8_t data;
        if(circular_buffer_get(&usart_buf->write_buffer, &data)){
            USART_SendData(usart_buf->usart, data);
        }else{
            USART_ITConfig(usart_buf->usart, USART_IT_TXE, DISABLE);
        }
    }
}

bool usart_buf_data_overrun(usart_buf_t* usart_buf)
{
    return usart_buf->data_overrun;
}

void usart_buf_flush(usart_buf_t* usart_buf)
{
    while(circular_buffer_avail_size(&usart_buf->write_buffer) != 0);
}

size_t usart_buf_data_avail(usart_buf_t* usart_buf)
{
    return circular_buffer_avail_size(&usart_buf->read_buffer);
}

size_t usart_buf_put(usart_buf_t* usart_buf, uint8_t data)
{
    size_t res;
    
    if(!circular_buffer_valid(&usart_buf->write_buffer) ||
       usart_transmitter_state(usart_buf->usart) == DISABLE) return 0;
    
    while(circular_buffer_free_size(&usart_buf->write_buffer) == 0);
    
    ITStatus itstatus = usart_tx_it_state(usart_buf->usart);
    USART_ITConfig(usart_buf->usart, USART_IT_TXE, DISABLE);
    
    res = circular_buffer_put(&usart_buf->write_buffer, data);
    
    if(res != 0){
        USART_ITConfig(usart_buf->usart, USART_IT_TXE, ENABLE);
    }else{
        USART_ITConfig(usart_buf->usart, USART_IT_TXE, itstatus);
    }
    
    return res;
}

size_t usart_buf_get(usart_buf_t* usart_buf, uint8_t* data)
{
    size_t res;
    
    if(!circular_buffer_valid(&usart_buf->read_buffer) ||
       usart_receiver_state(usart_buf->usart) == DISABLE) return 0;
    
    ITStatus itstatus = usart_rx_it_state(usart_buf->usart);
    USART_ITConfig(usart_buf->usart, USART_IT_RXNE, DISABLE);
    
    res = circular_buffer_get(&usart_buf->read_buffer, data);
    
    USART_ITConfig(usart_buf->usart, USART_IT_RXNE, itstatus);
    
    if(res != 0){
        usart_buf->data_overrun = false;
    }
    
    return res;
}

size_t usart_buf_write(usart_buf_t* usart_buf, const void* data, size_t size)
{
    if(size == 0 || data == NULL) return 0;
    if(!circular_buffer_valid(&usart_buf->write_buffer) ||
       usart_transmitter_state(usart_buf->usart) == DISABLE) return 0;
    
    size_t res_size = 0;
    size_t n;
    size_t free_size;
    
    do{

        do{
            free_size = circular_buffer_free_size(&usart_buf->write_buffer);
            n = MIN(free_size, size);
        }while(n == 0);

        ITStatus itstatus = usart_tx_it_state(usart_buf->usart);
        USART_ITConfig(usart_buf->usart, USART_IT_TXE, DISABLE);

        n = circular_buffer_write(&usart_buf->write_buffer, data, n);

        if(n != 0){
            USART_ITConfig(usart_buf->usart, USART_IT_TXE, ENABLE);
        }else{
            USART_ITConfig(usart_buf->usart, USART_IT_TXE, itstatus);
            break;
        }
        
        res_size += n;
        data += n;
        size -= n;
    
    }while(size > 0);
    
    return res_size;
}

size_t usart_buf_read(usart_buf_t* usart_buf, void* data, size_t size)
{
    if(size == 0 || data == NULL) return 0;
    if(!circular_buffer_valid(&usart_buf->read_buffer) ||
       usart_receiver_state(usart_buf->usart) == DISABLE) return 0;
    
    size_t res_size = 0;
    
    ITStatus itstatus = usart_rx_it_state(usart_buf->usart);
    USART_ITConfig(usart_buf->usart, USART_IT_RXNE, DISABLE);
    
    res_size = circular_buffer_read(&usart_buf->read_buffer, data, size);
    
    USART_ITConfig(usart_buf->usart, USART_IT_RXNE, itstatus);
    
    if(res_size != 0){
        usart_buf->data_overrun = false;
    }
    
    return res_size;
}

