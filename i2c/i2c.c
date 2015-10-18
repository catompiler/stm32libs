#include "i2c.h"
#include <stdio.h>
#include <stddef.h>
#include "utils/utils.h"
#include "dma/dma.h"

#define I2C_BUS_READ_RESTART_READ

//#define I2C_BUS_DEBUG

#define I2C_STATE_IDLE                  0
#define I2C_STATE_READING               1
#define I2C_STATE_WRITING               2
/*#define I2C_STATE_ADDRESS_WRITING       1
#define I2C_STATE_DATA_WRITING          2
#define I2C_STATE_DATA_READING          3*/


//#define I2C_EVENT_MASTER_BYTE_RECEIVING 0x00030040
//#define I2C_EVENT_MASTER_BYTE_RECEIVED  0x00030044


static ALWAYS_INLINE void i2c_bus_enable_irq(i2c_bus_t* i2c)
{
    I2C_ITConfig(i2c->i2c_device, I2C_IT_EVT | I2C_IT_ERR, ENABLE);
}

static ALWAYS_INLINE void i2c_bus_disable_irq(i2c_bus_t* i2c)
{
    I2C_ITConfig(i2c->i2c_device, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);
}

err_t i2c_bus_init(i2c_bus_t* i2c, i2c_bus_init_t* init)
{
    if(init == NULL) return E_NULL_POINTER;
    
    i2c->i2c_device = init->i2c_device;
    i2c->dma_rx_channel = init->dma_rx_channel;
    i2c->dma_tx_channel = init->dma_tx_channel;
    i2c->messages = NULL;
    i2c->message_index = 0;
    i2c->messages_count = 0;
    i2c->status = I2C_STATUS_IDLE;
    i2c->error = I2C_NO_ERROR;
    i2c->transfer_id = I2C_BUS_DEFAULT_TRANSFER_ID;
    i2c->callback = NULL;
    
    i2c->state = I2C_STATE_IDLE;
    i2c->dma_rx_locked = false;
    i2c->dma_tx_locked = false;

    I2C_NACKPositionConfig(i2c->i2c_device, I2C_NACKPosition_Current);
    
    //i2c_bus_enable_irq(i2c);
    
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
        DMA_DeInit(i2c->dma_rx_channel);
        dma_channel_unlock(i2c->dma_rx_channel);
        i2c->dma_rx_locked = false;
    }
    if(i2c->dma_tx_locked){
        DMA_DeInit(i2c->dma_tx_channel);
        dma_channel_unlock(i2c->dma_tx_channel);
        i2c->dma_tx_locked = false;
    }
}

static void i2c_bus_dma_start_rx(i2c_bus_t* i2c)
{
    if(i2c->dma_rx_locked){
        I2C_DMACmd(i2c->i2c_device, ENABLE);
        DMA_Cmd(i2c->dma_rx_channel, ENABLE);
    }
}

static void i2c_bus_dma_start_tx(i2c_bus_t* i2c)
{
    if(i2c->dma_tx_locked){
        I2C_DMACmd(i2c->i2c_device, ENABLE);
        DMA_Cmd(i2c->dma_tx_channel, ENABLE);
    }
}

static void i2c_bus_dma_stop_rx(i2c_bus_t* i2c)
{
    if(i2c->dma_rx_locked){
        I2C_DMACmd(i2c->i2c_device, DISABLE);
        DMA_Cmd(i2c->dma_rx_channel, DISABLE);
        I2C_AcknowledgeConfig(i2c->i2c_device, DISABLE);
        I2C_DMALastTransferCmd(i2c->i2c_device, DISABLE);
    }
}

static void i2c_bus_dma_stop_tx(i2c_bus_t* i2c)
{
    if(i2c->dma_tx_locked){
        I2C_DMACmd(i2c->i2c_device, DISABLE);
        I2C_DMALastTransferCmd(i2c->i2c_device, DISABLE);
        DMA_Cmd(i2c->dma_tx_channel, DISABLE);
    }
}

static void i2c_bus_setup_message(i2c_bus_t* i2c)
{
    i2c_message_t* msg = &i2c->messages[i2c->message_index];
    switch(msg->direction){
        case I2C_WRITE:
            i2c->state = I2C_STATE_WRITING;
            i2c_bus_dma_tx_config(i2c, msg->data, msg->data_size);
            break;
        case I2C_READ:
            i2c->state = I2C_STATE_READING;
            if(msg->data_size > 1) i2c_bus_dma_rx_config(i2c, msg->data, msg->data_size);
            break;
    }
}

static bool i2c_bus_has_next_message(i2c_bus_t* i2c)
{
    return (i2c->message_index + 1) < i2c->messages_count;
}

static ALWAYS_INLINE void i2c_bus_on_message_sent(i2c_bus_t* i2c)
{
    if(i2c->messages[i2c->message_index].callback != NULL){
        i2c->messages[i2c->message_index].callback(&i2c->messages[i2c->message_index]);
    }
}

static bool i2c_bus_setup_next_message(i2c_bus_t* i2c)
{
    i2c_bus_on_message_sent(i2c);
    if(++ i2c->message_index >= i2c->messages_count) return false;
    i2c_bus_setup_message(i2c);
    return true;
}

static bool i2c_bus_need_restart(i2c_bus_t* i2c)
{
    if(i2c->message_index == 0) return true;
    
    i2c_message_t* msg1 = &i2c->messages[i2c->message_index - 1];
    i2c_message_t* msg2 = &i2c->messages[i2c->message_index];
    
#ifdef I2C_BUS_READ_RESTART_READ
    if(msg2->direction == I2C_READ) return true;
#endif
    return msg1->address != msg2->address || msg1->direction != msg2->direction;
}

#ifndef I2C_BUS_READ_RESTART_READ
static bool i2c_bus_need_next_restart(i2c_bus_t* i2c)
{
    if(!i2c_bus_has_next_message(i2c)) return false;
    
    i2c_message_t* msg1 = &i2c->messages[i2c->message_index];
    i2c_message_t* msg2 = &i2c->messages[i2c->message_index + 1];
    
#ifdef I2C_BUS_READ_RESTART_READ
    if(msg2->direction == I2C_READ) return true;
#endif
    return msg1->address != msg2->address || msg1->direction != msg2->direction;
}
#endif

static ALWAYS_INLINE bool i2c_bus_done(i2c_bus_t* i2c)
{
#ifdef I2C_BUS_DEBUG
    printf("[I2C] done\r\n");
#endif
    
    i2c_bus_disable_irq(i2c);
    
    if(i2c->callback) i2c->callback();
    return i2c->state == I2C_STATE_IDLE;
}

static ALWAYS_INLINE void i2c_bus_clear_btf(i2c_bus_t* i2c)
{
    I2C_ReadRegister(i2c->i2c_device, I2C_Register_SR1);
    I2C_ReceiveData(i2c->i2c_device);
}

static void i2c_bus_mt_event_handler(i2c_bus_t* i2c, uint16_t SR1, uint16_t SR2)
{
    i2c_message_t* message = &i2c->messages[i2c->message_index];
    
    if(SR1 & I2C_SR1_SB){
        
        I2C_Send7bitAddress(i2c->i2c_device, message->address << 1, I2C_Direction_Transmitter);
    
    }else if(SR1 & I2C_SR1_ADDR){
        
        i2c_bus_dma_start_tx(i2c);
    
    }else if(SR1 & I2C_SR1_BTF){//(I2C_SR1_BTF | I2C_SR1_TXE)
        
        if(i2c_bus_setup_next_message(i2c)){
            
            if(i2c_bus_need_restart(i2c)){
                I2C_GenerateSTART(i2c->i2c_device, ENABLE);
            }else{
                i2c_bus_dma_start_tx(i2c);
            }
        }else{
            
            i2c->state = I2C_STATE_IDLE;
            i2c->status = I2C_STATUS_TRANSFERED;

            i2c_bus_dma_unlock_channels(i2c);

            I2C_GenerateSTOP(i2c->i2c_device, ENABLE);

            i2c_bus_done(i2c);
        }
            
        //i2c_bus_clear_btf(i2c);
    }
}

static void i2c_bus_mr_setup_receiving(i2c_bus_t* i2c, i2c_message_t* message)
{
    if(message == NULL){
        message = &i2c->messages[i2c->message_index];
    }
    
    if(message->data_size == 1){
        
        I2C_ITConfig(i2c->i2c_device, I2C_IT_BUF, ENABLE);

        if(i2c_bus_has_next_message(i2c)){

#ifndef I2C_BUS_READ_RESTART_READ
            if(i2c_bus_need_next_restart(i2c)){
#endif
                I2C_AcknowledgeConfig(i2c->i2c_device, DISABLE);
                I2C_GenerateSTART(i2c->i2c_device, ENABLE);
#ifndef I2C_BUS_READ_RESTART_READ
            }else{
                I2C_AcknowledgeConfig(i2c->i2c_device, ENABLE);
            }
#endif

        }else{
            I2C_AcknowledgeConfig(i2c->i2c_device, DISABLE);
            I2C_GenerateSTOP(i2c->i2c_device, ENABLE);
        }
        I2C_ITConfig(i2c->i2c_device, I2C_IT_BUF, ENABLE);

    }else{
        I2C_AcknowledgeConfig(i2c->i2c_device, ENABLE);
        
#ifndef I2C_BUS_READ_RESTART_READ
        if(!i2c_bus_has_next_message(i2c) || i2c_bus_need_next_restart(i2c)){
#endif
            I2C_DMALastTransferCmd(i2c->i2c_device, ENABLE);
#ifndef I2C_BUS_READ_RESTART_READ
        }else{
            I2C_DMALastTransferCmd(i2c->i2c_device, DISABLE);
        }
#endif
        i2c_bus_dma_start_rx(i2c);
    }
}

static void i2c_bus_mr_event_handler(i2c_bus_t* i2c, uint16_t SR1, uint16_t SR2)
{
    if(SR1 & I2C_SR1_RXNE){//receiver
        
        i2c_message_t* message = &i2c->messages[i2c->message_index];
        
        I2C_ITConfig(i2c->i2c_device, I2C_IT_BUF, DISABLE);

#ifdef I2C_BUS_DEBUG
        uint8_t data = I2C_ReceiveData(i2c->i2c_device);
        *((uint8_t*)message->data) = data;
        
        printf("[I2C] Readed: 0x%x\r\n", data);
#else
        *((uint8_t*)message->data) = I2C_ReceiveData(i2c->i2c_device);
#endif

        if(!i2c_bus_setup_next_message(i2c)){
            i2c->state = I2C_STATE_IDLE;
            i2c->status = I2C_STATUS_TRANSFERED;

            i2c_bus_dma_unlock_channels(i2c);

            //I2C_GenerateSTOP(i2c->i2c_device, ENABLE);

            i2c_bus_done(i2c);
        }
#ifndef I2C_BUS_READ_RESTART_READ
        else{
            if(!i2c_bus_need_restart(i2c)){
                i2c_bus_mr_setup_receiving(i2c, NULL);
            }
        }
#endif
    }
    if(SR1 & I2C_SR1_SB){
        
        i2c_message_t* message = &i2c->messages[i2c->message_index];
        
        I2C_Send7bitAddress(i2c->i2c_device, message->address << 1, I2C_Direction_Receiver);

    }else if(SR1 & I2C_SR1_ADDR){
        
        i2c_bus_mr_setup_receiving(i2c, NULL);
        
    }
}

static void i2c_bus_master_error_handler(i2c_bus_t* i2c, uint16_t SR1/*, uint16_t SR2*/)
{
    i2c->state = I2C_STATE_IDLE;
    
    i2c->status = I2C_STATUS_ERROR;
        
    I2C_GenerateSTOP(i2c->i2c_device, ENABLE);
    
    if(SR1 & I2C_SR1_AF){
        
        i2c->error = I2C_ERROR_NACK;
        
    }else if(SR1 & I2C_SR1_ARLO){

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
    
    i2c_bus_dma_stop_rx(i2c);
    i2c_bus_dma_stop_tx(i2c);
    
    i2c_bus_dma_unlock_channels(i2c);

    i2c_bus_done(i2c);
}

bool i2c_bus_dma_tx_channel_irq_handler(i2c_bus_t* i2c)
{
    if(i2c->state == I2C_STATE_WRITING){

        i2c_bus_dma_stop_tx(i2c);
        
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
    i2c_message_t* message = &i2c->messages[i2c->message_index];
    
    if(i2c->state == I2C_STATE_READING &&
       message->data_size > 1){
            
        i2c_bus_dma_stop_rx(i2c);
        
        uint32_t dma_tc_flag = dma_channel_it_flag(i2c->dma_rx_channel, DMA_IT_TC);
        uint32_t dma_te_flag = dma_channel_it_flag(i2c->dma_tx_channel, DMA_IT_TE);
        
        if(DMA_GetITStatus(dma_tc_flag)){
        
            DMA_ClearITPendingBit(dma_tc_flag);

#ifdef I2C_BUS_DEBUG
            printf("[DMA](TC) RX\r\n");
#endif
            
            if(i2c_bus_setup_next_message(i2c)){

#ifndef I2C_BUS_READ_RESTART_READ
                if(i2c_bus_need_restart(i2c)){
#endif
                    I2C_GenerateSTART(i2c->i2c_device, ENABLE);
#ifndef I2C_BUS_READ_RESTART_READ
                }else{
                    i2c_bus_mr_setup_receiving(i2c, NULL);
                }
#endif
            }else{
                
                i2c->state = I2C_STATE_IDLE;
                i2c->status = I2C_STATUS_TRANSFERED;
                
                i2c_bus_dma_unlock_channels(i2c);
                
                I2C_GenerateSTOP(i2c->i2c_device, ENABLE);
                
                i2c_bus_done(i2c);
            }
            
        }else if(DMA_GetITStatus(dma_te_flag)){
            
            DMA_ClearITPendingBit(dma_te_flag);
            
#ifdef I2C_BUS_DEBUG
            printf("[DMA](TE) RX\r\n");
#endif

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

void i2c_bus_event_irq_handler(i2c_bus_t* i2c)
{
    uint16_t SR1 = i2c->i2c_device->SR1;
    uint16_t SR2 = i2c->i2c_device->SR2;
    
    // clear stopf
    if(SR1 & I2C_SR1_STOPF){
        i2c->i2c_device->CR1 |= I2C_CR1_PE;
    }
    
    //if(SR2 & I2C_SR2_MSL){
        
        if(i2c->state == I2C_STATE_WRITING){
            
            i2c_bus_mt_event_handler(i2c, SR1, SR2);
            
        }else if(i2c->state == I2C_STATE_READING){
            
            i2c_bus_mr_event_handler(i2c, SR1, SR2);
        }else{
#ifdef I2C_BUS_DEBUG
            printf("[I2C](UEV) SR1: %d; SR2: %d; state: %d;\r\n", SR1, SR2, i2c->state);
#endif
            if(SR1 & I2C_SR1_BTF){
                i2c_bus_clear_btf(i2c);
            }
        }
    //}
    
#ifdef I2C_BUS_DEBUG
    printf("[I2C](EV) SR1: %d; SR2: %d; state: %d;\r\n", SR1, SR2, i2c->state);
#endif
}

void i2c_bus_error_irq_handler(i2c_bus_t* i2c)
{
    uint16_t SR1 = i2c->i2c_device->SR1;
#ifdef I2C_BUS_DEBUG
    uint16_t SR2 = i2c->i2c_device->SR2;
#endif
    
    I2C_ClearITPendingBit(i2c->i2c_device, 
            I2C_IT_SMBALERT   |
            I2C_IT_TIMEOUT    |
            I2C_IT_PECERR     |
            I2C_IT_OVR        |
            I2C_IT_AF         |
            I2C_IT_ARLO       |
            I2C_IT_BERR
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
    return ((i2c->i2c_device->SR2 & I2C_SR2_BUSY) != 0);// || (i2c->state != I2C_STATE_IDLE);
}

void i2c_bus_wait(i2c_bus_t* i2c)
{
    WAIT_WHILE_TRUE(i2c_bus_busy(i2c));
}

void i2c_bus_reset(i2c_bus_t* i2c)
{
    i2c_device_reset(i2c->i2c_device);
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

bool i2c_bus_set_callback(i2c_bus_t* i2c, i2c_callback_t callback)
{
    if(i2c_bus_busy(i2c)) return false;
    
    i2c->callback = callback;
    
    return true;
}

i2c_transfer_id_t i2c_bus_transfer_id(i2c_bus_t* i2c)
{
    return i2c->transfer_id;
}

bool i2c_bus_set_transfer_id(i2c_bus_t* i2c, i2c_transfer_id_t id)
{
    if(i2c_bus_busy(i2c)) return false;
    
    i2c->transfer_id = id;
    
    return true;
}

i2c_status_t i2c_bus_status(i2c_bus_t* i2c)
{
    return i2c->status;
}

i2c_error_t i2c_bus_error(i2c_bus_t* i2c)
{
    return i2c->error;
}

err_t i2c_message_init(i2c_message_t* message, i2c_address_t address, i2c_direction_t direction, void* data, size_t data_size)
{
    if(data_size == 0) return E_INVALID_VALUE;
    if(data == NULL) return E_NULL_POINTER;
    
    message->address = address;
    message->direction = direction;
    message->data = data;
    message->data_size = data_size;
    
    return E_NO_ERROR;
}

i2c_message_callback_t i2c_message_callback(i2c_message_t* message)
{
    return message->callback;
}

void i2c_message_set_callback(i2c_message_t* message, i2c_message_callback_t callback)
{
    message->callback = callback;
}

void* i2c_message_sender_data(i2c_message_t* message)
{
    return message->sender_data;
}

void i2c_message_set_sender_data(i2c_message_t* message, void* sender_data)
{
    message->sender_data = sender_data;
}

err_t i2c_bus_transfer(i2c_bus_t* i2c, i2c_message_t* messages, size_t messages_count)
{
    if(i2c_bus_busy(i2c)) return E_BUSY;
    if(messages == NULL) return E_NULL_POINTER;
    if(messages_count == 0) return E_INVALID_VALUE;
    
    bool need_rx_channel = false;
    bool need_tx_channel = false;
    
    size_t i = 0;
    
    i2c_message_t* msg = NULL;
    // Проверим все сообщения.
    for(; i < messages_count; i ++){
        msg = &messages[i];
        if(msg->data_size == 0) return E_INVALID_VALUE;
        switch(msg->direction){
            case I2C_READ:
                need_rx_channel = true;
                break;
            case I2C_WRITE:
                need_tx_channel = true;
                break;
        }
        if(need_rx_channel && need_tx_channel) break;
    }
    
    if(!i2c_bus_dma_lock_channels(i2c, need_rx_channel, need_tx_channel)) return E_BUSY;
    
    i2c->messages = messages;
    i2c->messages_count = messages_count;
    i2c->message_index = 0;
    i2c->error = E_NO_ERROR;
    i2c->status = I2C_STATUS_TRANSFERING;
    
    i2c_bus_setup_message(i2c);
    
    I2C_GenerateSTART(i2c->i2c_device, ENABLE);
    
    i2c_bus_enable_irq(i2c);
    
    return E_NO_ERROR;
}

