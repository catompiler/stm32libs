#include "modbus_rtu.h"
#include <string.h>
#include "utils/net.h"



/*
 * Вычисление CRC взято из мануала http://www.modbus.org/docs/PI_MBUS_300.pdf
 */

/* Table of CRC values for high–order byte     */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
};

/* Table of CRC values for low–order byte      */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
};

static uint16_t modbus_rtu_calc_crc(const void* data, size_t size)
{
    uint8_t crc_hi = 0xff;
    uint8_t crc_lo = 0xff;
    
    size_t index = 0;
    
    for(; size != 0; size --){
        index = crc_lo ^ *(uint8_t*)data ++;
        crc_lo = crc_hi ^ table_crc_hi[index];
        crc_hi = table_crc_lo[index];
    }
    return ((uint16_t)crc_hi << 8) | crc_lo;
}



ALWAYS_INLINE static void modbus_rtu_message_recv_init(modbus_rtu_message_t* msg, modbus_rtu_address_t address)
{
    msg->data_size = 0;
    msg->adu.address = address;
}

ALWAYS_INLINE static err_t modbus_rtu_message_recv(modbus_rtu_message_t* msg, usart_bus_t* usart)
{
    return usart_bus_recv(usart, &msg->adu.func, MODBUS_RTU_PACKET_SIZE_MAX - 1);
}

ALWAYS_INLINE static void modbus_rtu_message_recv_done(modbus_rtu_message_t* msg, usart_bus_t* usart)
{
    size_t bytes_received = usart_bus_bytes_received(usart) + 1;
    if(bytes_received <= MODBUS_RTU_FIELDS_CRC_SIZE){
        msg->data_size = 0;
    }else{
        msg->data_size = bytes_received - MODBUS_RTU_FIELDS_CRC_SIZE;
    }
}



err_t modbus_rtu_init(modbus_rtu_t* modbus, modbus_rtu_init_t* modbus_is)
{
    if(modbus_is->usart == NULL) return E_NULL_POINTER;
    
    if(modbus_is->rx_message == NULL) return E_NULL_POINTER;
    if(modbus_is->tx_message == NULL) return E_NULL_POINTER;
    
    memset(modbus, 0x0, sizeof(modbus_rtu_t));
    
    modbus->usart = modbus_is->usart;
    
    modbus->mode = modbus_is->mode;
    modbus->address = modbus_is->address;
    
    modbus->rx_message = modbus_is->rx_message;
    modbus->tx_message = modbus_is->tx_message;
    
    modbus_rtu_message_reset(modbus->rx_message);
    modbus_rtu_message_reset(modbus->tx_message);
    
    return E_NO_ERROR;
}

modbus_rtu_address_t modbus_rtu_address(modbus_rtu_t* modbus)
{
    return modbus->address;
}

err_t modbus_rtu_set_address(modbus_rtu_t* modbus, modbus_rtu_address_t address)
{
    if(address == MODBUS_RTU_ADDRESS_BROADCAST) return E_INVALID_VALUE;
    
    modbus->address = address;
    
    return E_NO_ERROR;
}

err_t modbus_rtu_send_message(modbus_rtu_t* modbus)
{
    if(modbus->tx_message->adu.address == MODBUS_RTU_ADDRESS_BROADCAST &&
       modbus->mode == MODBUS_RTU_MODE_SLAVE){
        return E_INVALID_VALUE;
    }
    
    if(usart_bus_rx_busy(modbus->usart)) return E_BUSY;
    
    usart_bus_receiver_disable(modbus->usart);
    
    err_t err = usart_bus_send(modbus->usart, &modbus->tx_message->adu, modbus->tx_message->data_size + MODBUS_RTU_FIELDS_CRC_SIZE);
    
    if(err != E_NO_ERROR){
        usart_bus_receiver_enable(modbus->usart);
    }
    
    return err;
}

bool modbus_rtu_usart_rx_byte_callback(modbus_rtu_t* modbus, uint8_t byte)
{
    if(modbus->mode == MODBUS_RTU_MODE_SLAVE &&
       modbus->address != byte && byte != MODBUS_RTU_ADDRESS_BROADCAST){
        usart_bus_sleep(modbus->usart);
        return true;
    }
    
    if(usart_bus_rx_errors(modbus->usart) != USART_ERROR_NONE){
        usart_bus_sleep(modbus->usart);
        return true;
    }
    
    if(modbus_rtu_message_recv(modbus->rx_message, modbus->usart) == E_NO_ERROR){
        modbus_rtu_message_recv_init(modbus->rx_message, byte);
    }else{
        usart_bus_sleep(modbus->usart);
    }
    
    return true;
}

bool modbus_rtu_usart_rx_callback(modbus_rtu_t* modbus)
{
    if(usart_bus_rx_errors(modbus->usart) != USART_ERROR_NONE) return true;
    
    modbus_rtu_message_recv_done(modbus->rx_message, modbus->usart);
    
    uint16_t calc_crc = modbus_rtu_calc_crc(&modbus->rx_message->adu, modbus->rx_message->data_size + MODBUS_RTU_FIELDS_SIZE);
    uint16_t crc = modbus_rtu_message_crc(modbus->rx_message);
    
    if(crc != calc_crc) return true;
    
    if(modbus->recv_callback) modbus->recv_callback();
    
    return true;
}

bool modbus_rtu_usart_tx_callback(modbus_rtu_t* modbus)
{
    usart_bus_receiver_enable(modbus->usart);
    
    if(modbus->sent_callback) modbus->sent_callback();
    
    return true;
}

modbus_rtu_msg_recv_callback_t modbus_rtu_msg_recv_callback(modbus_rtu_t* modbus)
{
    return modbus->recv_callback;
}

void modbus_rtu_set_msg_recv_callback(modbus_rtu_t* modbus, modbus_rtu_msg_recv_callback_t callback)
{
    modbus->recv_callback = callback;
}

modbus_rtu_msg_sent_callback_t modbus_rtu_msg_sent_callback(modbus_rtu_t* modbus)
{
    return modbus->sent_callback;
}

void modbus_rtu_set_msg_sent_callback(modbus_rtu_t* modbus, modbus_rtu_msg_sent_callback_t callback)
{
    modbus->sent_callback = callback;
}

modbus_rtu_message_t* modbus_rtu_rx_message(modbus_rtu_t* modbus)
{
    return modbus->rx_message;
}

modbus_rtu_message_t* modbus_rtu_tx_message(modbus_rtu_t* modbus)
{
    return modbus->tx_message;
}

void modbus_rtu_message_answer_succ(modbus_rtu_message_t* message, const modbus_rtu_message_t* from_message)
{
    message->adu.address = from_message->adu.address;
    message->adu.func = from_message->adu.func;
    message->data_size = 0;
}

void modbus_rtu_message_answer_fail(modbus_rtu_message_t* message, const modbus_rtu_message_t* from_message, modbus_rtu_error_t error)
{
    message->adu.address = from_message->adu.address;
    message->adu.func = from_message->adu.func | 0x80;
    message->adu.data_and_crc[0] = error;
    message->data_size = 1;
}

void modbus_rtu_message_reset(modbus_rtu_message_t* message)
{
    message->adu.address = 0;
    message->adu.func = 0;
    message->data_size = 0;
}

modbus_rtu_address_t modbus_rtu_message_address(modbus_rtu_message_t* message)
{
    return message->adu.address;
}

void modbus_rtu_message_set_address(modbus_rtu_message_t* message, modbus_rtu_address_t address)
{
    message->adu.address = address;
}

modbus_rtu_func_t modbus_rtu_message_func(modbus_rtu_message_t* message)
{
    return message->adu.func;
}

void modbus_rtu_message_set_func(modbus_rtu_message_t* message, modbus_rtu_func_t func)
{
    message->adu.func = func;
}

size_t modbus_rtu_message_data_size(modbus_rtu_message_t* message)
{
    return message->data_size;
}

err_t modbus_rtu_message_set_data_size(modbus_rtu_message_t* message, size_t size)
{
    if(size > MODBUS_RTU_DATA_SIZE_MAX) return E_INVALID_VALUE;
    
    message->data_size = size;
    
    return E_NO_ERROR;
}

void* modbus_rtu_message_data(modbus_rtu_message_t* message)
{
    return message->adu.data_and_crc;
}

err_t modbus_rtu_message_copy_data(modbus_rtu_message_t* message, const void* data, size_t size)
{
    if(size > MODBUS_RTU_DATA_SIZE_MAX) return E_INVALID_VALUE;
    if(size == 0) return E_INVALID_VALUE;
    if(data == NULL) return E_NULL_POINTER;
    
    memcpy(message->adu.data_and_crc, data, size);
    
    message->data_size = size;
    
    return E_NO_ERROR;
}

uint16_t modbus_rtu_message_crc(modbus_rtu_message_t* message)
{
    uint8_t crc_lo = message->adu.data_and_crc[message->data_size];
    uint8_t crc_hi = message->adu.data_and_crc[message->data_size + 1];
    
    return ((uint16_t)crc_hi << 8) | crc_lo;
}

uint16_t modbus_rtu_message_calc_crc(modbus_rtu_message_t* message)
{
    uint16_t crc = modbus_rtu_calc_crc(&message->adu, message->data_size + MODBUS_RTU_FIELDS_SIZE);
    
    message->adu.data_and_crc[message->data_size] = crc & 0xff;
    message->adu.data_and_crc[message->data_size + 1] = crc >> 8;
    
    return crc;
}

modbus_rtu_read_coil_callback_t modbus_rtu_read_coil_callback(modbus_rtu_t* modbus)
{
    return modbus->read_coil_callback;
}

void modbus_rtu_set_read_coil_callback(modbus_rtu_t* modbus, modbus_rtu_read_coil_callback_t callback)
{
    modbus->read_coil_callback = callback;
}

modbus_rtu_read_din_callback_t modbus_rtu_read_din_callback(modbus_rtu_t* modbus)
{
    return modbus->read_din_callback;
}

void modbus_rtu_set_read_din_callback(modbus_rtu_t* modbus, modbus_rtu_read_din_callback_t callback)
{
    modbus->read_din_callback = callback;
}

modbus_rtu_read_holding_reg_callback_t modbus_rtu_read_holding_reg_callback(modbus_rtu_t* modbus)
{
    return modbus->read_holding_reg_callback;
}

void modbus_rtu_set_read_holding_reg_callback(modbus_rtu_t* modbus, modbus_rtu_read_holding_reg_callback_t callback)
{
    modbus->read_holding_reg_callback = callback;
}

modbus_rtu_read_input_reg_callback_t modbus_rtu_read_input_reg_callback(modbus_rtu_t* modbus)
{
    return modbus->read_input_reg_callback;
}

void modbus_rtu_set_read_input_reg_callback(modbus_rtu_t* modbus, modbus_rtu_read_input_reg_callback_t callback)
{
    modbus->read_input_reg_callback = callback;
}

modbus_rtu_write_coil_callback_t modbus_rtu_write_coil_callback(modbus_rtu_t* modbus)
{
    return modbus->write_coil_callback;
}

void modbus_rtu_set_write_coil_callback(modbus_rtu_t* modbus, modbus_rtu_write_coil_callback_t callback)
{
    modbus->write_coil_callback = callback;
}

modbus_rtu_write_holding_reg_callback_t modbus_rtu_write_holding_reg_callback(modbus_rtu_t* modbus)
{
    return modbus->write_holding_reg_callback;
}

void modbus_rtu_set_write_holding_reg_callback(modbus_rtu_t* modbus, modbus_rtu_write_holding_reg_callback_t callback)
{
    modbus->write_holding_reg_callback = callback;
}

modbus_rtu_change_holding_reg_callback_t modbus_rtu_change_holding_reg_callback(modbus_rtu_t* modbus)
{
    return modbus->change_holding_reg_callback;
}

void modbus_rtu_set_change_holding_reg_callback(modbus_rtu_t* modbus, modbus_rtu_change_holding_reg_callback_t callback)
{
    modbus->change_holding_reg_callback = callback;
}

modbus_rtu_report_slave_id_callback_t modbus_rtu_report_slave_id_callback(modbus_rtu_t* modbus)
{
    return modbus->report_slave_id_callback;
}

void modbus_rtu_set_report_slave_id_callback(modbus_rtu_t* modbus, modbus_rtu_report_slave_id_callback_t callback)
{
    modbus->report_slave_id_callback = callback;
}

modbus_rtu_read_file_record_t modbus_rtu_read_file_record_callback(modbus_rtu_t* modbus)
{
    return modbus->read_file_record_callback;
}

void modbus_rtu_set_read_file_record_callback(modbus_rtu_t* modbus, modbus_rtu_read_file_record_t callback)
{
    modbus->read_file_record_callback = callback;
}

modbus_rtu_write_file_record_t modbus_rtu_write_file_record_callback(modbus_rtu_t* modbus)
{
    return modbus->write_file_record_callback;
}

void modbus_rtu_set_write_file_record_callback(modbus_rtu_t* modbus, modbus_rtu_write_file_record_t callback)
{
    modbus->write_file_record_callback = callback;
}

modbus_rtu_custom_function_callback_t modbus_rtu_custom_function_callback(modbus_rtu_t* modbus)
{
    return modbus->custom_function_callback;
}

void modbus_rtu_set_custom_function_callback(modbus_rtu_t* modbus, modbus_rtu_custom_function_callback_t callback)
{
    modbus->custom_function_callback = callback;
}

static err_t modbus_rtu_disp_fail(modbus_rtu_t* modbus, modbus_rtu_error_t error)
{
    if(modbus_rtu_message_address(modbus->rx_message) == MODBUS_RTU_ADDRESS_BROADCAST &&
       modbus->mode == MODBUS_RTU_MODE_SLAVE){
        return E_NO_ERROR;
    }
    
    modbus_rtu_message_answer_fail(modbus->tx_message, modbus->rx_message, error);
    modbus_rtu_message_calc_crc(modbus->tx_message);
    
    return modbus_rtu_send_message(modbus);
}

static err_t modbus_rtu_disp_succ(modbus_rtu_t* modbus, size_t data_size)
{
    if(modbus_rtu_message_address(modbus->rx_message) == MODBUS_RTU_ADDRESS_BROADCAST &&
       modbus->mode == MODBUS_RTU_MODE_SLAVE){
        return E_NO_ERROR;
    }
    
    modbus_rtu_message_answer_succ(modbus->tx_message, modbus->rx_message);
    modbus_rtu_message_set_data_size(modbus->tx_message, data_size);
    modbus_rtu_message_calc_crc(modbus->tx_message);
    
    return modbus_rtu_send_message(modbus);
}

static err_t modbus_rtu_disp_read_coils(modbus_rtu_t* modbus)
{
    if(modbus->read_coil_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) != sizeof(uint16_t) * 2)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint16_t* rx_data = (uint16_t*)modbus_rtu_message_data(modbus->rx_message);
    uint8_t* tx_data = modbus_rtu_message_data(modbus->tx_message);
    
    uint16_t address = ntohs(rx_data[0]);
    uint16_t count = ntohs(rx_data[1]);
    
    if(count > (MODBUS_RTU_DATA_SIZE_MAX - 1) * 8)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_coil_value_t value;
    
    uint8_t byte_index = 1;
    uint8_t bit_index = 0;
    
    tx_data[byte_index] = 0;
    
    uint16_t i = 0;
    for(; i < count; i ++){
        modbus_err = modbus->read_coil_callback(address, &value);
        
        if(modbus_err != MODBUS_RTU_ERROR_NONE)
            return modbus_rtu_disp_fail(modbus, modbus_err);
        
        if(value){
            tx_data[byte_index] |= (1 << bit_index);
        }
        if(++ bit_index >= 8){
            bit_index = 0;
            tx_data[++ byte_index] = 0;
        }
        address ++;
    }
    
    tx_data[0] = (byte_index - 1) + (count > 0);
    
    return modbus_rtu_disp_succ(modbus, tx_data[0] + 1);
}

static err_t modbus_rtu_disp_read_dins(modbus_rtu_t* modbus)
{
    if(modbus->read_din_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) != sizeof(uint16_t) * 2)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint16_t* rx_data = (uint16_t*)modbus_rtu_message_data(modbus->rx_message);
    uint8_t* tx_data = modbus_rtu_message_data(modbus->tx_message);
    
    uint16_t address = ntohs(rx_data[0]);
    uint16_t count = ntohs(rx_data[1]);
    
    if(count > (MODBUS_RTU_DATA_SIZE_MAX - 1) * 8)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_din_value_t value;
    
    uint8_t byte_index = 1;
    uint8_t bit_index = 0;
    
    tx_data[byte_index] = 0;
    
    uint16_t i = 0;
    for(; i < count; i ++){
        modbus_err = modbus->read_din_callback(address, &value);
        
        if(modbus_err != MODBUS_RTU_ERROR_NONE)
            return modbus_rtu_disp_fail(modbus, modbus_err);
        
        if(value){
            tx_data[byte_index] |= (1 << bit_index);
        }
        if(++ bit_index >= 8){
            bit_index = 0;
            tx_data[++ byte_index] = 0;
        }
        address ++;
    }
    
    tx_data[0] = (byte_index - 1) + (count > 0);
    
    return modbus_rtu_disp_succ(modbus, tx_data[0] + 1);
}

static err_t modbus_rtu_disp_read_holding_regs(modbus_rtu_t* modbus)
{
    if(modbus->read_holding_reg_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) != sizeof(uint16_t) * 2)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint16_t* rx_data = (uint16_t*)modbus_rtu_message_data(modbus->rx_message);
    uint8_t* tx_data = modbus_rtu_message_data(modbus->tx_message);
    
    uint16_t address = ntohs(rx_data[0]);
    uint16_t count = ntohs(rx_data[1]);
    
    if(count > (MODBUS_RTU_DATA_SIZE_MAX - 1) / 2)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    uint16_t value;
    
    uint16_t* regs_values = (uint16_t*)(tx_data + 1);
    
    uint16_t i = 0;
    for(; i < count; i ++){
        modbus_err = modbus->read_holding_reg_callback(address, &value);
        
        if(modbus_err != MODBUS_RTU_ERROR_NONE)
            return modbus_rtu_disp_fail(modbus, modbus_err);
        
        regs_values[i] = htons(value);
        address ++;
    }
    
    tx_data[0] = count * 2;
    
    return modbus_rtu_disp_succ(modbus, tx_data[0] + 1);
}

static err_t modbus_rtu_disp_read_input_regs(modbus_rtu_t* modbus)
{
    if(modbus->read_input_reg_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) != sizeof(uint16_t) * 2)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint16_t* rx_data = (uint16_t*)modbus_rtu_message_data(modbus->rx_message);
    uint8_t* tx_data = modbus_rtu_message_data(modbus->tx_message);
    
    uint16_t address = ntohs(rx_data[0]);
    uint16_t count = ntohs(rx_data[1]);
    
    if(count > (MODBUS_RTU_DATA_SIZE_MAX - 1) / 2)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    uint16_t value;
    
    uint16_t* regs_values = (uint16_t*)(tx_data + 1);
    
    uint16_t i = 0;
    for(; i < count; i ++){
        modbus_err = modbus->read_input_reg_callback(address, &value);
        
        if(modbus_err != MODBUS_RTU_ERROR_NONE)
            return modbus_rtu_disp_fail(modbus, modbus_err);
        
        regs_values[i] = htons(value);
        address ++;
    }
    
    tx_data[0] = count * 2;
    
    return modbus_rtu_disp_succ(modbus, tx_data[0] + 1);
}

static err_t modbus_rtu_disp_write_coil(modbus_rtu_t* modbus)
{
    if(modbus->write_coil_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) != sizeof(uint16_t) * 2)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint16_t* rx_data = (uint16_t*)modbus_rtu_message_data(modbus->rx_message);
    uint16_t* tx_data = (uint16_t*)modbus_rtu_message_data(modbus->tx_message);
    
    uint16_t address = ntohs(rx_data[0]);
    uint16_t value = ntohs(rx_data[1]);

    if(value != 0 && value != 0xff00)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_err = modbus->write_coil_callback(address,
                            value ? MODBUS_RTU_COIL_ON : MODBUS_RTU_COIL_OFF);

    if(modbus_err != MODBUS_RTU_ERROR_NONE)
        return modbus_rtu_disp_fail(modbus, modbus_err);
    
    tx_data[0] = rx_data[0];
    tx_data[1] = rx_data[1];
    
    return modbus_rtu_disp_succ(modbus, modbus_rtu_message_data_size(modbus->rx_message));
}

static err_t modbus_rtu_disp_write_reg(modbus_rtu_t* modbus)
{
    if(modbus->write_holding_reg_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) != sizeof(uint16_t) * 2)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint16_t* rx_data = (uint16_t*)modbus_rtu_message_data(modbus->rx_message);
    uint16_t* tx_data = (uint16_t*)modbus_rtu_message_data(modbus->tx_message);
    
    uint16_t address = ntohs(rx_data[0]);
    uint16_t value = ntohs(rx_data[1]);

    modbus_err = modbus->write_holding_reg_callback(address, value);

    if(modbus_err != MODBUS_RTU_ERROR_NONE)
        return modbus_rtu_disp_fail(modbus, modbus_err);
    
    tx_data[0] = rx_data[0];
    tx_data[1] = rx_data[1];
    
    return modbus_rtu_disp_succ(modbus, modbus_rtu_message_data_size(modbus->rx_message));
}

static err_t modbus_rtu_disp_write_coils(modbus_rtu_t* modbus)
{
    if(modbus->write_coil_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    const size_t header_size = (sizeof(uint16_t) * 2 + 1);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) < header_size)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint16_t* rx_data = (uint16_t*)modbus_rtu_message_data(modbus->rx_message);
    uint16_t* tx_data = (uint16_t*)modbus_rtu_message_data(modbus->tx_message);
    
    uint16_t address = ntohs(rx_data[0]);
    uint16_t count = ntohs(rx_data[1]);
    uint8_t bytes_count = *((uint8_t*)&rx_data[2]);
    
    if((count > (MODBUS_RTU_DATA_SIZE_MAX - header_size) * 8) || (count > bytes_count * 8))
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    uint8_t* coils_values = modbus_rtu_message_data(modbus->rx_message) + header_size;
    
    uint8_t byte_index = 0;
    uint8_t bit_index = 0;
    
    modbus_rtu_coil_value_t value;
    
    uint16_t i = 0;
    for(; i < count; i ++){
        
        if(coils_values[byte_index] & (1 << bit_index)){
            value = MODBUS_RTU_COIL_ON;
        }else{
            value = MODBUS_RTU_COIL_OFF;
        }
        
        modbus_err = modbus->write_coil_callback(address, value);
        
        if(modbus_err != MODBUS_RTU_ERROR_NONE)
            return modbus_rtu_disp_fail(modbus, modbus_err);
        
        if(++ bit_index >= 8){
            bit_index = 0;
            byte_index ++;
        }
        address ++;
    }
    
    tx_data[0] = rx_data[0];
    tx_data[1] = rx_data[1];
    
    return modbus_rtu_disp_succ(modbus, sizeof(uint16_t) * 2);
}

static err_t modbus_rtu_disp_write_regs(modbus_rtu_t* modbus)
{
    if(modbus->write_holding_reg_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    const size_t header_size = (sizeof(uint16_t) * 2 + 1);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) < header_size)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint16_t* rx_data = (uint16_t*)modbus_rtu_message_data(modbus->rx_message);
    uint16_t* tx_data = (uint16_t*)modbus_rtu_message_data(modbus->tx_message);
    
    uint16_t address = ntohs(rx_data[0]);
    uint16_t count = ntohs(rx_data[1]);
    uint8_t bytes_count = *((uint8_t*)&rx_data[2]);
    
    if((count > (MODBUS_RTU_DATA_SIZE_MAX - header_size) / 2) || (count > bytes_count / 2))
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    uint16_t* regs_values = (uint16_t*)(modbus_rtu_message_data(modbus->rx_message) + header_size);
    
    uint16_t i = 0;
    for(; i < count; i ++){
        
        modbus_err = modbus->write_holding_reg_callback(address, ntohs(regs_values[i]));
        
        if(modbus_err != MODBUS_RTU_ERROR_NONE)
            return modbus_rtu_disp_fail(modbus, modbus_err);
        
        address ++;
    }
    
    tx_data[0] = rx_data[0];
    tx_data[1] = rx_data[1];
    
    return modbus_rtu_disp_succ(modbus, sizeof(uint16_t) * 2);
}

static err_t modbus_rtu_disp_change_reg(modbus_rtu_t* modbus)
{
    if(modbus->change_holding_reg_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) != sizeof(uint16_t) * 3)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint16_t* rx_data = (uint16_t*)modbus_rtu_message_data(modbus->rx_message);
    uint16_t* tx_data = (uint16_t*)modbus_rtu_message_data(modbus->tx_message);
    
    uint16_t address = ntohs(rx_data[0]);
    uint16_t and_mask = ntohs(rx_data[1]);
    uint16_t or_mask = ntohs(rx_data[2]);

    modbus_err = modbus->change_holding_reg_callback(address, and_mask, or_mask);

    if(modbus_err != MODBUS_RTU_ERROR_NONE)
        return modbus_rtu_disp_fail(modbus, modbus_err);
    
    tx_data[0] = rx_data[0];
    tx_data[1] = rx_data[1];
    tx_data[2] = rx_data[2];
    
    return modbus_rtu_disp_succ(modbus, modbus_rtu_message_data_size(modbus->rx_message));
}

static err_t modbus_rtu_disp_report_slave_id(modbus_rtu_t* modbus)
{
    if(modbus->report_slave_id_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) != 0)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint8_t* tx_data = modbus_rtu_message_data(modbus->tx_message);
    
    const size_t max_slave_id_size = MODBUS_RTU_DATA_SIZE_MAX - 3;
    
    modbus_rtu_slave_id_t slave_id;
    slave_id.id = 0;
    slave_id.data = NULL;
    slave_id.data_size = 0;
    slave_id.status = MODBUS_RTU_RUN_STATUS_OFF;
    
    modbus_err = modbus->report_slave_id_callback(&slave_id);

    if(modbus_err != MODBUS_RTU_ERROR_NONE)
        return modbus_rtu_disp_fail(modbus, modbus_err);
    
    if(slave_id.data_size > max_slave_id_size)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    if(slave_id.data == NULL) slave_id.data_size = 0;
    
    size_t offset = 0;
    
    tx_data[0] = (uint8_t) (slave_id.data_size + 2);
    offset ++;
    
    tx_data[offset] = slave_id.id;
    offset ++;
    
    tx_data[offset] = (slave_id.status == MODBUS_RTU_RUN_STATUS_OFF) ? 0x0 : 0xff;
    offset ++;
    
    if(slave_id.data){
        memcpy(&tx_data[offset], slave_id.data, slave_id.data_size);
    }
    
    return modbus_rtu_disp_succ(modbus, tx_data[0] + 1);
}

static err_t modbus_rtu_disp_read_file_record(modbus_rtu_t* modbus)
{
    if(modbus->read_file_record_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) == 0)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    uint8_t* rx_data = (uint8_t*)modbus_rtu_message_data(modbus->rx_message);
    uint8_t* tx_data = (uint8_t*)modbus_rtu_message_data(modbus->tx_message);
    
    uint8_t byte_count = rx_data[0];
    if(byte_count < 0x7 || byte_count > 0xf5) return MODBUS_RTU_ERROR_INVALID_DATA;
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    uint8_t* request_buffer = (uint8_t*)&rx_data[1];
    uint8_t* response_buffer = (uint8_t*)&tx_data[1];
    
    modbus_rtu_file_record_request_t* request = NULL;
    modbus_rtu_read_file_record_response_t* response = NULL;
    
    uint16_t file = 0;
    uint16_t record = 0;
    uint16_t length = 0;
    
    uint8_t resp_size = 0;
    uint16_t index = 0;
    
    uint16_t* response_data = NULL;
    size_t response_cur_data_size = 0;
    size_t response_cur_size = 0;
    
    while(resp_size < MODBUS_RTU_DATA_SIZE_MAX - 1 && byte_count >= sizeof(modbus_rtu_file_record_request_t)){
        
        request = (modbus_rtu_file_record_request_t*)request_buffer;
        response = (modbus_rtu_read_file_record_response_t*)response_buffer;
        
        if(request->reference_type != 0x6){
            modbus_err = MODBUS_RTU_ERROR_INVALID_ADDRESS;
            break;
        }
        file = ntohs(request->file_number);
        record = ntohs(request->record_number);
        length = ntohs(request->record_length);
        
        if((uint32_t)record + (uint32_t)length > 0x270f /* 9999 file records */){
            modbus_err = MODBUS_RTU_ERROR_INVALID_ADDRESS;
            break;
        }
        
        response_data = (uint16_t*)(response_buffer + sizeof(modbus_rtu_read_file_record_response_t));
        
        modbus_err = modbus->read_file_record_callback(file, record, length, response_data);
        if(modbus_err != MODBUS_RTU_ERROR_NONE) break;
        
        response_cur_data_size = length * 2;
        response_cur_size = response_cur_data_size + sizeof(modbus_rtu_read_file_record_response_t);
        
        response->data_length = response_cur_data_size + 1;
        response->reference_type = 0x6;
        
        // host to network order.
        for(index = 0; index < length; index ++){
            response_data[index] = htons(response_data[index]);
        }
        
        byte_count -= sizeof(modbus_rtu_file_record_request_t);
        request_buffer += sizeof(modbus_rtu_file_record_request_t);
        resp_size += response_cur_size;
        response_buffer += response_cur_size;
    }
    
    if(modbus_err != MODBUS_RTU_ERROR_NONE)
        return modbus_rtu_disp_fail(modbus, modbus_err);
    
    if(byte_count != 0)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    tx_data[0] = resp_size;
    
    return modbus_rtu_disp_succ(modbus, tx_data[0] + 1);
}

static err_t modbus_rtu_disp_write_file_record(modbus_rtu_t* modbus)
{
    if(modbus->write_file_record_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    if(modbus_rtu_message_data_size(modbus->rx_message) == 0)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    uint8_t* rx_data = (uint8_t*)modbus_rtu_message_data(modbus->rx_message);
    uint8_t* tx_data = (uint8_t*)modbus_rtu_message_data(modbus->tx_message);
    
    uint8_t byte_count = rx_data[0];
    if(byte_count < 0x7 || byte_count > 0xf5) return MODBUS_RTU_ERROR_INVALID_DATA;
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    modbus_rtu_file_record_request_t* request = (modbus_rtu_file_record_request_t*)&rx_data[1];
    modbus_rtu_file_record_request_t* response = (modbus_rtu_file_record_request_t*)&tx_data[1];
    
    uint16_t file = 0;
    uint16_t record = 0;
    uint16_t length = 0;
    
    uint16_t* request_data = NULL;
    size_t request_size = 0;
    
    while(byte_count >= sizeof(modbus_rtu_file_record_request_t)){
        if(request->reference_type != 0x6){
            modbus_err = MODBUS_RTU_ERROR_INVALID_ADDRESS;
            break;
        }
        file = ntohs(request->file_number);
        record = ntohs(request->record_number);
        length = ntohs(request->record_length);
        
        if((uint32_t)record + (uint32_t)length > 0x270f /* 9999 file records */){
            modbus_err = MODBUS_RTU_ERROR_INVALID_ADDRESS;
            break;
        }
        
        request_data = (uint16_t*)((uint8_t*)request + sizeof(modbus_rtu_file_record_request_t));
        
        modbus_err = modbus->write_file_record_callback(file, record, length, request_data);
        if(modbus_err != MODBUS_RTU_ERROR_NONE) break;
        
        request_size = sizeof(modbus_rtu_file_record_request_t) + length * 2;
        
        memcpy(response, request, request_size);
        
        byte_count -= request_size;
        request = (modbus_rtu_file_record_request_t*)((uint8_t*)request + request_size);
        response = (modbus_rtu_file_record_request_t*)((uint8_t*)response + request_size);
    }
    
    if(modbus_err != MODBUS_RTU_ERROR_NONE)
        return modbus_rtu_disp_fail(modbus, modbus_err);
    
    if(byte_count != 0)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    tx_data[0] = rx_data[0];
    
    return modbus_rtu_disp_succ(modbus, tx_data[0] + 1);
}

static err_t modbus_rtu_disp_custom(modbus_rtu_t* modbus)
{
    if(modbus->custom_function_callback == NULL)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_FUNC);
    
    modbus_rtu_error_t modbus_err = MODBUS_RTU_ERROR_NONE;
    
    void* rx_data = modbus_rtu_message_data(modbus->rx_message);
    void* tx_data = modbus_rtu_message_data(modbus->tx_message);
    
    size_t rx_size = modbus_rtu_message_data_size(modbus->rx_message);
    size_t tx_size = 0;
    
    modbus_err = modbus->custom_function_callback(
                    modbus_rtu_message_func(modbus->rx_message),
                    rx_data, rx_size, tx_data, &tx_size
                 );
    
    if(modbus_err != MODBUS_RTU_ERROR_NONE)
        return modbus_rtu_disp_fail(modbus, modbus_err);

    if(tx_size > MODBUS_RTU_DATA_SIZE_MAX)
        return modbus_rtu_disp_fail(modbus, MODBUS_RTU_ERROR_INVALID_DATA);
    
    return modbus_rtu_disp_succ(modbus, tx_size);
}


err_t modbus_rtu_dispatch(modbus_rtu_t* modbus)
{
    switch(modbus->rx_message->adu.func){
        default:
            return modbus_rtu_disp_custom(modbus);
        case MODBUS_RTU_FUNC_READ_COILS_STATUS:
            return modbus_rtu_disp_read_coils(modbus);
        case MODBUS_RTU_FUNC_READ_DISCR_INPUTS:
            return modbus_rtu_disp_read_dins(modbus);
        case MODBUS_RTU_FUNC_READ_HOLDING_REGS:
            return modbus_rtu_disp_read_holding_regs(modbus);
        case MODBUS_RTU_FUNC_READ_INPUT_REGS:
            return modbus_rtu_disp_read_input_regs(modbus);
        case MODBUS_RTU_FUNC_WRITE_SINGLE_COIL:
            return modbus_rtu_disp_write_coil(modbus);
        case MODBUS_RTU_FUNC_WRITE_SINGLE_REG:
            return modbus_rtu_disp_write_reg(modbus);
        case MODBUS_RTU_FUNC_WRITE_MULTIPLE_COILS:
            return modbus_rtu_disp_write_coils(modbus);
        case MODBUS_RTU_FUNC_WRITE_MULTIPLE_REGS:
            return modbus_rtu_disp_write_regs(modbus);
        case MODBUS_RTU_FUNC_CHANGE_REG:
            return modbus_rtu_disp_change_reg(modbus);
        case MODBUS_RTU_FUNC_READ_SLAVE_ID:
            return modbus_rtu_disp_report_slave_id(modbus);
        case MODBUS_RTU_FUNC_READ_FILE:
            return modbus_rtu_disp_read_file_record(modbus);
        case MODBUS_RTU_FUNC_WRITE_FILE:
            return modbus_rtu_disp_write_file_record(modbus);
    }
    
    return E_NO_ERROR;
}
