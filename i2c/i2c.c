#include "i2c.h"
#include <stdio.h>
#include <sys/stat.h>
#include "utils/utils.h"
#include "dma/dma.h"


#ifdef I2C_EVENT_MASTER_BYTE_RECEIVING
#undef I2C_EVENT_MASTER_BYTE_RECEIVING
#endif

#ifdef I2C_EVENT_MASTER_BYTE_RECEIVED
#undef I2C_EVENT_MASTER_BYTE_RECEIVED
#endif

#define I2C_EVENT_MASTER_BYTE_RECEIVING 0x00030040
#define I2C_EVENT_MASTER_BYTE_RECEIVED  0x00030044


//#define I2C_BUS_DEBUG


#define I2C_STATE_IDLE                  0
#define I2C_STATE_ADDRESS_WRITING       1
#define I2C_STATE_DATA_WRITING          2
#define I2C_STATE_DATA_READING          3


err_t i2c_bus_init(i2c_bus_t* i2c, i2c_bus_init_t* init)
{
    if(init == NULL) return E_NULL_POINTER;
    
    i2c->i2c_device = init->i2c_device;
    i2c->dma_rx_channel = init->dma_rx_channel;
    i2c->dma_tx_channel = init->dma_tx_channel;
    i2c->message = NULL;
    i2c->status = I2C_STATUS_IDLE;
    i2c->error = I2C_NO_ERROR;
    i2c->transfer_id = I2C_BUS_DEFAULT_TRANSFER_ID;
    i2c->callback = NULL;
    
    i2c->state = I2C_STATE_IDLE;
    i2c->dma_rx_locked = false;
    i2c->dma_tx_locked = false;
    
    I2C_ITConfig(i2c->i2c_device, I2C_IT_EVT, ENABLE);
    //I2C_ITConfig(i2c->i2c_device, I2C_IT_BUF, ENABLE);
    I2C_ITConfig(i2c->i2c_device, I2C_IT_ERR, ENABLE);
    
    return E_NO_ERROR;
}

static void i2c_bus_dma_config(DMA_InitTypeDef* dma_is, i2c_bus_t* i2c, void* address, size_t size)
{
    dma_is->DMA_Priority = DMA_Priority_Medium;
    dma_is->DMA_M2M = DMA_M2M_Disable;
    dma_is->DMA_Mode = DMA_Mode_Normal;
    
    dma_is->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma_is->DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_is->DMA_PeripheralBaseAddr = (uint32_t)&i2c->i2c_device->DR;
    
    dma_is->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma_is->DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_is->DMA_MemoryBaseAddr = (uint32_t)address;
    
    dma_is->DMA_BufferSize = size;
}

static void i2c_bus_dma_tx_config(i2c_bus_t* i2c, void* address, size_t size)
{
    DMA_InitTypeDef dma_is;
    
    i2c_bus_dma_config(&dma_is, i2c, address, size);
    
    dma_is.DMA_DIR = DMA_DIR_PeripheralDST;
    
    DMA_DeInit(i2c->dma_tx_channel);
    
    DMA_Init(i2c->dma_tx_channel, &dma_is);
    DMA_ITConfig(i2c->dma_tx_channel, DMA_IT_TC, ENABLE);
    DMA_ITConfig(i2c->dma_tx_channel, DMA_IT_TE, ENABLE);
}

static void i2c_bus_dma_rx_config(i2c_bus_t* i2c, void* address, size_t size)
{
    DMA_InitTypeDef dma_is;
    
    i2c_bus_dma_config(&dma_is, i2c, address, size);
    
    dma_is.DMA_DIR = DMA_DIR_PeripheralSRC;
    
    DMA_DeInit(i2c->dma_rx_channel);
    
    DMA_Init(i2c->dma_rx_channel, &dma_is);
    DMA_ITConfig(i2c->dma_rx_channel, DMA_IT_TC, ENABLE);
    DMA_ITConfig(i2c->dma_rx_channel, DMA_IT_TE, ENABLE);
}

static bool i2c_bus_dma_lock_channels(i2c_bus_t* i2c, bool lock_rx, bool lock_tx)
{
    if(lock_rx){
        i2c->dma_rx_locked = dma_channel_trylock(i2c->dma_rx_channel);
        if(!i2c->dma_rx_locked) return false;
    }
    if(lock_tx){
        i2c->dma_tx_locked = dma_channel_trylock(i2c->dma_tx_channel);
        if(!i2c->dma_tx_locked){
            if(i2c->dma_rx_locked){
                dma_channel_unlock(i2c->dma_rx_channel);
                i2c->dma_rx_locked = false;
            }
            return false;
        }
    }
    return true;
}

static void i2c_bus_dma_unlock_channels(i2c_bus_t* i2c)
{
    if(i2c->dma_rx_locked){
        dma_channel_unlock(i2c->dma_rx_channel);
        i2c->dma_rx_locked = false;
    }
    if(i2c->dma_tx_locked){
        dma_channel_unlock(i2c->dma_tx_channel);
        i2c->dma_tx_locked = false;
    }
}

static inline bool i2c_bus_done(i2c_bus_t* i2c)
{
    if(i2c->callback) i2c->callback();
    return i2c->state == I2C_STATE_IDLE;
}

static void i2c_bus_mt_event_handler(i2c_bus_t* i2c, uint16_t SR1/*, uint16_t SR2*/)
{
    if(SR1 & I2C_SR1_SB){
        
        I2C_Send7bitAddress(i2c->i2c_device, i2c->message->address << 1, I2C_Direction_Transmitter);
    
    }else if(SR1 & I2C_SR1_ADDR){
        
        if(i2c->state == I2C_STATE_ADDRESS_WRITING){
            i2c_bus_dma_tx_config(i2c, i2c->message->rom_address, i2c->message->rom_address_size);
        }else{//I2C_STATE_DATA_WRITING
            i2c_bus_dma_tx_config(i2c, i2c->message->data, i2c->message->data_size);
        }
        I2C_DMACmd(i2c->i2c_device, ENABLE);
        DMA_Cmd(i2c->dma_tx_channel, ENABLE);
    
    }else if(SR1 & (I2C_SR1_BTF | I2C_SR1_TXE)){
        
        switch(i2c->state){
            case I2C_STATE_ADDRESS_WRITING:
                
                i2c_bus_dma_tx_config(i2c, i2c->message->data, i2c->message->data_size);
                
                i2c->state = I2C_STATE_DATA_WRITING;
                
                DMA_Cmd(i2c->dma_tx_channel, ENABLE);
                
                break;
                
            case I2C_STATE_DATA_WRITING:
                
                I2C_DMACmd(i2c->i2c_device, DISABLE);
                DMA_Cmd(i2c->dma_tx_channel, DISABLE);
                
                i2c->state = I2C_STATE_IDLE;
                i2c->status = I2C_STATUS_WRITED;
                
                i2c_bus_dma_unlock_channels(i2c);
                
                I2C_GenerateSTOP(i2c->i2c_device, ENABLE);
                
                i2c_bus_done(i2c);
                break;
        }
    }
}

static void i2c_bus_mr_event_handler(i2c_bus_t* i2c, uint16_t SR1/*, uint16_t SR2*/)
{
    if(SR1 & I2C_SR1_SB){
        
        if(i2c->state == I2C_STATE_ADDRESS_WRITING){
            I2C_Send7bitAddress(i2c->i2c_device, i2c->message->address << 1, I2C_Direction_Transmitter);
        }else{
            I2C_Send7bitAddress(i2c->i2c_device, i2c->message->address << 1, I2C_Direction_Receiver);
        }
    }else if(SR1 & I2C_SR1_ADDR){
        
        if(i2c->state == I2C_STATE_ADDRESS_WRITING){
            
            i2c_bus_dma_tx_config(i2c, i2c->message->rom_address, i2c->message->rom_address_size);
            
            I2C_DMACmd(i2c->i2c_device, ENABLE);
            DMA_Cmd(i2c->dma_tx_channel, ENABLE);
            
        }else{//receiver
            
            if(i2c->message->data_size == 1){
                
                I2C_ITConfig(i2c->i2c_device, I2C_IT_BUF, ENABLE);
                
                I2C_AcknowledgeConfig(i2c->i2c_device, DISABLE);
                I2C_GenerateSTOP(i2c->i2c_device, ENABLE);
                
            }else{
                i2c_bus_dma_rx_config(i2c, i2c->message->data, i2c->message->data_size);
                
                I2C_AcknowledgeConfig(i2c->i2c_device, ENABLE);
                I2C_DMALastTransferCmd(i2c->i2c_device, ENABLE);
                
                I2C_DMACmd(i2c->i2c_device, ENABLE);
                DMA_Cmd(i2c->dma_rx_channel, ENABLE);
            }
        }
    }else if(SR1 & (I2C_SR1_BTF | I2C_SR1_TXE)){
        
        i2c->state = I2C_STATE_DATA_READING;
        I2C_GenerateSTART(i2c->i2c_device, ENABLE);
        
    }else if(SR1 & I2C_SR1_RXNE){//receiver
        
        I2C_ITConfig(i2c->i2c_device, I2C_IT_BUF, DISABLE);

        *((uint8_t*)i2c->message->data) = I2C_ReceiveData(i2c->i2c_device);

        i2c->state = I2C_STATE_IDLE;
        i2c->status = I2C_STATUS_READED;
        
        i2c_bus_dma_unlock_channels(i2c);
        
        i2c_bus_done(i2c);
    }
}

static void i2c_bus_master_error_handler(i2c_bus_t* i2c, uint16_t SR1/*, uint16_t SR2*/)
{
    i2c->state = I2C_STATE_IDLE;
    
    i2c->status = I2C_STATUS_ERROR;
    
    if(SR1 & I2C_SR1_AF){
        
        i2c->error = I2C_ERROR_NACK;
        
        I2C_GenerateSTOP(i2c->i2c_device, ENABLE);
        
    }else{
        if(SR1 & I2C_SR1_ARLO){

            i2c->error = I2C_ERROR_ARB_LOST;

        }else if(SR1 & I2C_SR1_BERR){

            i2c->error = I2C_ERROR_BUS_FAIL;

        }else if(SR1 & I2C_SR1_TIMEOUT){

            i2c->error = I2C_ERROR_TIMEOUT;

        }else if(SR1 & I2C_SR1_OVR){

            i2c->error = I2C_ERROR_OVERRUN;

        }else{
            
            i2c->error = I2C_ERROR_UNKNOWN;
        }
    }
    
    I2C_DMACmd(i2c->i2c_device, DISABLE);
    I2C_DMALastTransferCmd(i2c->i2c_device, DISABLE);
    
    if(i2c->dma_rx_locked){
        DMA_Cmd(i2c->dma_rx_channel, DISABLE);
    }
    if(i2c->dma_tx_locked){
        DMA_Cmd(i2c->dma_tx_channel, DISABLE);
    }
    
    i2c_bus_dma_unlock_channels(i2c);

    i2c_bus_done(i2c);
}

bool i2c_bus_dma_tx_channel_irq_handler(i2c_bus_t* i2c)
{
    if(i2c->state != I2C_STATE_IDLE &&
       i2c->state != I2C_STATE_DATA_READING){
        
        uint32_t dma_tc_flag = dma_channel_it_flag(i2c->dma_tx_channel, DMA_IT_TC);
        uint32_t dma_te_flag = dma_channel_it_flag(i2c->dma_tx_channel, DMA_IT_TE);
        
        if(DMA_GetITStatus(dma_tc_flag)){

            DMA_ClearITPendingBit(dma_tc_flag);

#ifdef I2C_BUS_DEBUG
            printf("[DMA](TC) TX\r\n");
#endif
            
        }else if(DMA_GetITStatus(dma_te_flag)){
            
            DMA_ClearITPendingBit(dma_te_flag);
            
#ifdef I2C_BUS_DEBUG
            printf("[DMA](TE) TX\r\n");
#endif

            I2C_DMACmd(i2c->i2c_device, DISABLE);
            I2C_DMALastTransferCmd(i2c->i2c_device, DISABLE);
            DMA_Cmd(i2c->dma_rx_channel, DISABLE);

            i2c->state = I2C_STATE_IDLE;
            i2c->status = I2C_STATUS_ERROR;
            i2c->error = I2C_ERROR_DMA;

            i2c_bus_dma_unlock_channels(i2c);

            I2C_GenerateSTOP(i2c->i2c_device, ENABLE);

            i2c_bus_done(i2c);
        }
        return true;
    }
    return false;
}

bool i2c_bus_dma_rx_channel_irq_handler(i2c_bus_t* i2c)
{
    if(i2c->state == I2C_STATE_DATA_READING &&
       i2c->message->data_size > 1){
        
        uint32_t dma_tc_flag = dma_channel_it_flag(i2c->dma_rx_channel, DMA_IT_TC);
        uint32_t dma_te_flag = dma_channel_it_flag(i2c->dma_tx_channel, DMA_IT_TE);

        I2C_DMACmd(i2c->i2c_device, DISABLE);
        I2C_DMALastTransferCmd(i2c->i2c_device, DISABLE);
        DMA_Cmd(i2c->dma_rx_channel, DISABLE);
        
        if(DMA_GetITStatus(dma_tc_flag)){
        
            DMA_ClearITPendingBit(dma_tc_flag);

#ifdef I2C_BUS_DEBUG
            printf("[DMA](TC) RX\r\n");
#endif

            i2c->state = I2C_STATE_IDLE;
            i2c->status = I2C_STATUS_READED;
            
        }else if(DMA_GetITStatus(dma_te_flag)){
            
            DMA_ClearITPendingBit(dma_te_flag);
            
#ifdef I2C_BUS_DEBUG
            printf("[DMA](TE) RX\r\n");
#endif

            i2c->state = I2C_STATE_IDLE;
            i2c->status = I2C_STATUS_ERROR;
            i2c->error = I2C_ERROR_DMA;
        }
        
        i2c_bus_dma_unlock_channels(i2c);

        I2C_GenerateSTOP(i2c->i2c_device, ENABLE);

        i2c_bus_done(i2c);
        
        return true;
    }
    return false;
}

void i2c_bus_event_irq_handler(i2c_bus_t* i2c)
{
    uint16_t SR1 = i2c->i2c_device->SR1;
    uint16_t SR2 = i2c->i2c_device->SR2;
    
    // clear stopf
    if(SR1 & I2C_SR1_STOPF){
        i2c->i2c_device->CR1 |= I2C_CR1_PE;
    }
    
    if(SR2 & I2C_SR2_MSL){
        
        if(i2c->status == I2C_STATUS_WRITING){
            
            i2c_bus_mt_event_handler(i2c, SR1/*, SR2*/);
            
        }else if(i2c->status == I2C_STATUS_READING){
            
            i2c_bus_mr_event_handler(i2c, SR1/*, SR2*/);
        }
    }
    
#ifdef I2C_BUS_DEBUG
    printf("[I2C](EV) SR1: %d; SR2: %d; state: %d;\r\n", SR1, SR2, i2c->state);
#endif
}

void i2c_bus_error_irq_handler(i2c_bus_t* i2c)
{
    uint16_t SR1 = i2c->i2c_device->SR1;
    //uint16_t SR2 = i2c->i2c_device->SR2;
    
    I2C_ClearFlag(i2c->i2c_device, 
            I2C_FLAG_SMBALERT   |
            I2C_FLAG_TIMEOUT    |
            I2C_FLAG_PECERR     |
            I2C_FLAG_OVR        |
            I2C_FLAG_AF         |
            I2C_FLAG_ARLO       |
            I2C_FLAG_BERR
        );
    
    //if(SR2 & I2C_SR2_MSL){
        i2c_bus_master_error_handler(i2c, SR1/*, SR2*/);
    //}
    
#ifdef I2C_BUS_DEBUG
    printf("[I2C](ER) SR1: %d; SR2: %d;\r\n", SR1, SR2);
#endif
}

bool i2c_bus_busy(i2c_bus_t* i2c)
{
    return ((i2c->i2c_device->SR2 & I2C_SR2_BUSY) != 0) && (i2c->state != I2C_STATE_IDLE);
}

void i2c_bus_wait(i2c_bus_t* i2c)
{
    WAIT_WHILE_TRUE(i2c_bus_busy(i2c));
}

void i2c_device_reset(I2C_TypeDef* device)
{
    device->CR1 |= I2C_CR1_SWRST;
    __NOP();
    device->CR1 &= ~I2C_CR1_SWRST;
}

i2c_callback_t i2c_bus_callback(i2c_bus_t* i2c)
{
    return i2c->callback;
}

void i2c_bus_set_callback(i2c_bus_t* i2c, i2c_callback_t callback)
{
    i2c->callback = callback;
}

i2c_transfer_id_t i2c_bus_transfer_id(i2c_bus_t* i2c)
{
    return i2c->transfer_id;
}

void i2c_bus_set_transfer_id(i2c_bus_t* i2c, i2c_transfer_id_t id)
{
    i2c->transfer_id = id;
}

i2c_status_t i2c_bus_status(i2c_bus_t* i2c)
{
    return i2c->status;
}

i2c_error_t i2c_bus_error(i2c_bus_t* i2c)
{
    return i2c->error;
}

err_t i2c_bus_send(i2c_bus_t* i2c, i2c_message_t* message)
{
    if(i2c_bus_busy(i2c)) return E_BUSY;
    if(message == NULL) return E_NULL_POINTER;
    if(message->data == NULL || message->data_size == 0) return E_INVALID_VALUE;
    if(message->rom_address == NULL && message->rom_address_size != 0) return E_INVALID_VALUE;
    if(message->rom_address != NULL && message->rom_address_size == 0) return E_INVALID_VALUE;
    
    if(!i2c_bus_dma_lock_channels(i2c, false, true)) return E_BUSY;
    
    i2c->message = message;
    
    i2c->status = I2C_STATUS_WRITING;
    i2c->error = I2C_NO_ERROR;
    
    if(i2c->message->rom_address != NULL){
        i2c->state = I2C_STATE_ADDRESS_WRITING;
    }else{
        i2c->state = I2C_STATE_DATA_WRITING;
    }
    
    I2C_GenerateSTART(i2c->i2c_device, ENABLE);
    
    return E_NO_ERROR;
}

err_t i2c_bus_recv(i2c_bus_t* i2c, i2c_message_t* message)
{
    if(i2c_bus_busy(i2c)) return E_BUSY;
    if(message == NULL) return E_NULL_POINTER;
    if(message->data == NULL || message->data_size == 0) return E_INVALID_VALUE;
    if(message->rom_address == NULL && message->rom_address_size != 0) return E_INVALID_VALUE;
    if(message->rom_address != NULL && message->rom_address_size == 0) return E_INVALID_VALUE;
    
    if(!i2c_bus_dma_lock_channels(i2c, true, message->rom_address != NULL)) return E_BUSY;
    
    i2c->message = message;
    
    i2c->status = I2C_STATUS_READING;
    i2c->error = I2C_NO_ERROR;
    
    if(i2c->message->rom_address != NULL){
        i2c->state = I2C_STATE_ADDRESS_WRITING;
    }else{
        i2c->state = I2C_STATE_DATA_READING;
    }
    
    I2C_GenerateSTART(i2c->i2c_device, ENABLE);
    
    return E_NO_ERROR;
}