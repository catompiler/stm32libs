#include "m95x.h"
#include <string.h>
#include "utils/utils.h"
#include "bits/bits.h"

// Debug.
//#define M95X_GET_MEM_DEBUG

// Команды.
//! Разрешение записи.
#define M95X_CMD_WREN 0x6
//! Запрещение записи.
#define M95X_CMD_WRDI 0x4
//! Чтение регистра статуса.
#define M95X_CMD_RDSR 0x5
//! Запись регистра статуса.
#define M95X_CMD_WRSR 0x1
//! Чтение памяти.
#define M95X_CMD_READ 0x3
//! Запись памяти.
#define M95X_CMD_WRITE 0x2

// Биты в регистрах.
#define M95X_SR_WIP_OFFSET 0x0
#define M95X_SR_WIP_MASK   0x1
#define M95X_SR_WEL_OFFSET 0x1
#define M95X_SR_WEL_MASK   0x1
#define M95X_SR_BP_OFFSET  0x2
#define M95X_SR_BP_MASK    0x2
#define M95X_SR_SRWD_OFFSET 0x7
#define M95X_SR_SRWD_MASK  0x1

//! Размер команды.
#define M95X_CMD_SIZE 1
//! Размер регистра.
#define M95X_DATA_SIZE 1
//! Размер адреса.
#define M95X_ADDRESS_SIZE 2


//! Сообщение команды.
#define M95X_CMD_MESSAGE 0
//! Сообщение данных.
#define M95X_DATA_MESSAGE 1


/**
 * Выставляет значения управляющих пинов для начала обмена данными с EEPROM.
 * @param eeprom EEPROM.
 */
ALWAYS_INLINE static void m95x_pins_begin(m95x_t* eeprom)
{
    GPIO_ResetBits(eeprom->ce_gpio, eeprom->ce_pin);
}

/**
 * Выставлят значения управляющих пинов для окончания обмена данными с EEPROM.
 * @param eeprom EEPROM.
 */
ALWAYS_INLINE static void m95x_pins_end(m95x_t* eeprom)
{
    GPIO_SetBits(eeprom->ce_gpio, eeprom->ce_pin);
}

/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool m95x_wait_current_op(m95x_t* eeprom)
{
    // Если шина занята, да ещё и не нами - возврат ошибки занятости.
    if(spi_bus_busy(eeprom->spi) && spi_bus_transfer_id(eeprom->spi) != eeprom->transfer_id) return false;
    
    future_wait(&eeprom->future);
    spi_bus_wait(eeprom->spi);
    
    return true;
}

/**
 * Начинает обмен данными с EEPROM.
 * @param eeprom EEPROM.
 */
static void m95x_begin(m95x_t* eeprom)
{
    future_set_result(&eeprom->future, int_to_pvoid(E_NO_ERROR));
    
    future_start(&eeprom->future);
    
    m95x_pins_begin(eeprom);
}

/**
 * Завершает обмен данными с EEPROM.
 * @param eeprom EEPROM.
 * @param err Код ошибки.
 */
static void m95x_end(m95x_t* eeprom, err_t err)
{
    future_finish(&eeprom->future, int_to_pvoid(err));
    
    m95x_pins_end(eeprom);
}

/**
 * Отправляет данные в EEPROM.
 * Асинхронно.
 * @param eeprom EEPROM.
 * @param messages_count Число сообщений.
 * @return Код ошибки.
 */
static err_t m95x_transfer(m95x_t* eeprom, size_t messages_count)
{
    if(!spi_bus_set_transfer_id(eeprom->spi, eeprom->transfer_id)) return E_BUSY;
    
    m95x_begin(eeprom);
    
    err_t err = spi_bus_transfer(eeprom->spi, eeprom->messages, messages_count);
    
    if(err != E_NO_ERROR){
        m95x_end(eeprom, err);
        return err;
    }
    
    return E_NO_ERROR;
}

bool m95x_spi_callback(m95x_t* eeprom)
{
    if(spi_bus_transfer_id(eeprom->spi) != eeprom->transfer_id) return false;
    
    m95x_end(eeprom, spi_bus_status(eeprom->spi) != SPI_STATUS_TRANSFERED ? E_IO_ERROR : E_NO_ERROR);
    
    return true;
}

uint16_t m95x_pages_size_in_bytes(m95x_page_size_t size)
{
    switch(size){
        default:
        case M95X_PAGE_64:
            break;
        case M95X_PAGE_128:
            return 128;
    }
    return 64;
}

err_t m95x_init(m95x_t* eeprom, m95x_init_t* eeprom_init)
{
    // Clear.
    memset(eeprom, 0x0, sizeof(m95x_t));
    
    // SPI.
    eeprom->spi = eeprom_init->spi;
    
    // CE.
    eeprom->ce_gpio = eeprom_init->ce_gpio;
    eeprom->ce_pin = eeprom_init->ce_pin;
    
    future_init(&eeprom->future);
    
    // Значения по-умолчанию для этих пинов.
    m95x_pins_end(eeprom);
    
    // Идентификатор передачи.
    eeprom->transfer_id = eeprom_init->transfer_id;
    
    // Размер страницы в байтах.
    eeprom->page_size = m95x_pages_size_in_bytes(eeprom_init->page_size);
    
    return E_NO_ERROR;
}

bool m95x_busy(m95x_t* eeprom)
{
    return future_running(&eeprom->future);
}

err_t m95x_error(m95x_t* eeprom)
{
    return pvoid_to_int(err_t, future_result(&eeprom->future));
}

err_t m95x_wait(m95x_t* eeprom)
{
    future_wait(&eeprom->future);
    return pvoid_to_int(err_t, future_result(&eeprom->future));
}

/**
 * Получает указатель на данные в буфере нужного размера.
 * @param eeprom EEPROM.
 * @param size Необходимый размер.
 * @param index Значение предыдущего индекса.
 * @return Указатель на данные в бефере.
 */
ALWAYS_INLINE static uint8_t* m95x_get_buffer(m95x_t* eeprom, size_t size, size_t* index)
{
#ifdef M95X_GET_MEM_DEBUG
    if(index == NULL) return NULL;
#endif
    
    size_t i = *index;
    
#ifdef M95X_GET_MEM_DEBUG
    if(i + size > M95X_BUFFER_SIZE) return NULL;
#endif
    
    (*index) += size;
    
    return &eeprom->buffer[i];
}

/**
 * Получает указатель на сообщение.
 * @param eeprom EEPROM.
 * @param index Значение предыдущего индекса.
 * @return Указатель на данные в бефере.
 */
ALWAYS_INLINE static spi_message_t* m95x_get_message(m95x_t* eeprom, size_t* index)
{
#ifdef M95X_GET_MEM_DEBUG
    if(index == NULL) return NULL;
#endif
    
    size_t i = *index;
    
#ifdef M95X_GET_MEM_DEBUG
    if(i >= M95X_MESSAGES_COUNT) return NULL;
#endif
    
    (*index) ++;
    
    return &eeprom->messages[i];
}

err_t m95x_read_status(m95x_t* eeprom, m95x_status_t* status)
{
    if(status == NULL) return E_NULL_POINTER;
    
    if(!m95x_wait_current_op(eeprom)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_buf = m95x_get_buffer(eeprom, M95X_CMD_SIZE, &buffer_index);
#ifdef M95X_GET_MEM_DEBUG
    if(cmd_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = m95x_get_message(eeprom, &message_index);
#ifdef M95X_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    uint8_t* data_buf = m95x_get_buffer(eeprom, M95X_DATA_SIZE, &buffer_index);
#ifdef M95X_GET_MEM_DEBUG
    if(data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = m95x_get_message(eeprom, &message_index);
#ifdef M95X_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    *cmd_buf = M95X_CMD_RDSR;
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_buf, NULL, M95X_CMD_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data_buf, M95X_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = m95x_transfer(eeprom, 2);
    if(err != E_NO_ERROR) return err;
    
    err = m95x_wait(eeprom);
    if(err != E_NO_ERROR) return err;
    
    uint8_t data = *data_buf;
    
    status->write_in_progress = BIT_VALUE_MASK(data, M95X_SR_WIP_OFFSET, M95X_SR_WIP_MASK);
    status->write_enable_latch = BIT_VALUE_MASK(data, M95X_SR_WEL_OFFSET, M95X_SR_WEL_MASK);
    status->block_protect = BIT_VALUE_MASK(data, M95X_SR_BP_OFFSET, M95X_SR_BP_MASK);
    status->block_protect = BIT_VALUE_MASK(data, M95X_SR_SRWD_OFFSET, M95X_SR_SRWD_MASK);
    
    return E_NO_ERROR;
}

err_t m95x_write_status(m95x_t* eeprom, m95x_status_t* status)
{
    if(status == NULL) return E_NULL_POINTER;
    
    if(!m95x_wait_current_op(eeprom)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_data_buf = m95x_get_buffer(eeprom, M95X_CMD_SIZE + M95X_DATA_SIZE, &buffer_index);
#ifdef M95X_GET_MEM_DEBUG
    if(cmd_data_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = m95x_get_message(eeprom, &message_index);
#ifdef M95X_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    cmd_data_buf[0] = M95X_CMD_WRSR;
    
    uint8_t data = 0;
    // Да здравствует ARM!
    // orr r3, r3, r2, lsl #N
    data |= status->write_in_progress << M95X_SR_WIP_OFFSET;
    data |= status->write_enable_latch << M95X_SR_WEL_OFFSET;
    data |= status->block_protect << M95X_SR_BP_OFFSET;
    data |= status->status_reg_write_protect << M95X_SR_SRWD_OFFSET;
    
    cmd_data_buf[1] = data;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_data_buf, NULL, M95X_CMD_SIZE + M95X_DATA_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = m95x_transfer(eeprom, 1);
    if(err != E_NO_ERROR) return err;
    
    err = m95x_wait(eeprom);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

size_t m95x_avail_data_size(m95x_t* eeprom, m95x_address_t address)
{
    uint16_t page_mask = eeprom->page_size - 1;
    uint16_t next_page_address = (address & ~page_mask) + eeprom->page_size;
    
    return next_page_address - address;
}

err_t m95x_read(m95x_t* eeprom, m95x_address_t address, void* data, size_t size)
{
    if(size == 0) return E_NO_ERROR;
    
    if(data == NULL) return E_NULL_POINTER;
    
    if(!m95x_wait_current_op(eeprom)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_addr_buf = m95x_get_buffer(eeprom, M95X_CMD_SIZE + M95X_ADDRESS_SIZE, &buffer_index);
#ifdef M95X_GET_MEM_DEBUG
    if(cmd_addr_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = m95x_get_message(eeprom, &message_index);
#ifdef M95X_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = m95x_get_message(eeprom, &message_index);
#ifdef M95X_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    cmd_addr_buf[0] = M95X_CMD_READ;
    cmd_addr_buf[1] = (address >> 8) & 0xff;
    cmd_addr_buf[2] = address & 0xff;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_addr_buf, NULL, M95X_CMD_SIZE + M95X_ADDRESS_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = spi_message_init(data_msg, SPI_READ, NULL, data, size);
    if(err != E_NO_ERROR) return err;
    
    err = m95x_transfer(eeprom, 2);
    
    return err;
}

err_t m95x_write(m95x_t* eeprom, m95x_address_t address, const void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    
    if(size == 0) return E_NO_ERROR;
    
    if(m95x_avail_data_size(eeprom, address) < size) return E_INVALID_VALUE;
    
    if(!m95x_wait_current_op(eeprom)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    uint8_t* cmd_addr_buf = m95x_get_buffer(eeprom, M95X_CMD_SIZE + M95X_CMD_SIZE + M95X_ADDRESS_SIZE, &buffer_index);
#ifdef M95X_GET_MEM_DEBUG
    if(cmd_addr_buf == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = m95x_get_message(eeprom, &message_index);
#ifdef M95X_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = m95x_get_message(eeprom, &message_index);
#ifdef M95X_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    cmd_addr_buf[0] = M95X_CMD_WREN;
    cmd_addr_buf[1] = M95X_CMD_READ;
    cmd_addr_buf[2] = (address >> 8) & 0xff;
    cmd_addr_buf[3] = address & 0xff;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_addr_buf, NULL, M95X_CMD_SIZE + M95X_CMD_SIZE + M95X_ADDRESS_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = spi_message_init(data_msg, SPI_WRITE, data, NULL, size);
    if(err != E_NO_ERROR) return err;
    
    err = m95x_transfer(eeprom, 2);
    
    return err;
}
