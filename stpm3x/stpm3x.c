#include "stpm3x.h"
#include <string.h>
#include "utils/utils.h"
#include "bits/bits.h"
#include "utils/delay.h"

// Debug.
//#define STPM3X_GET_MEM_DEBUG

// Переключать пин CE между сообщениями.
//#define STPM3X_TOGGLE_CE_BETWEEN_MSGS

// Ждать 4 мкс между сообщениями.
//#define STPM3X_WAIT_4US_BETWEEN_MSGS

// Ждать шину SPI между сообщениями.
//#define STPM3X_WAIT_SPI_BETWEEN_MSGS

//! Регистры и биты в них.
// CR1
#define STPM3X_CR1_ADDR 0x0
#define STPM3X_CR1_DEFAULT 0x040000a0
// CLRSS_TO1
#define STPM3X_CR1_CLRSS_TO1_BIT 0
#define STPM3X_CR1_CLRSS_TO1_MASK 0xf
// ClearSS1
#define STPM3X_CR1_ClearSS1_BIT 4
#define STPM3X_CR1_ClearSS1_MASK 1
// ENVREF1
#define STPM3X_CR1_ENVREF1_BIT 5
#define STPM3X_CR1_ENVREF1_MASK 1
// TC1
#define STPM3X_CR1_TC1_BIT 6
#define STPM3X_CR1_TC1_MASK 0x7
// AEM1
#define STPM3X_CR1_AEM1_BIT 17
#define STPM3X_CR1_AEM1_MASK 1
// APM1
#define STPM3X_CR1_APM1_BIT 18
#define STPM3X_CR1_APM1_MASK 1
// BHPFV1
#define STPM3X_CR1_BHPFV1_BIT 19
#define STPM3X_CR1_BHPFV1_MASK 1
// BHPFC1
#define STPM3X_CR1_BHPFC1_BIT 20
#define STPM3X_CR1_BHPFC1_MASK 1
// ROC1
#define STPM3X_CR1_ROC1_BIT 21
#define STPM3X_CR1_ROC1_MASK 1
// BLPFV1
#define STPM3X_CR1_BLPFV1_BIT 22
#define STPM3X_CR1_BLPFV1_MASK 1
// BLPFC1
#define STPM3X_CR1_BLPFC1_BIT 23
#define STPM3X_CR1_BLPFC1_MASK 1
// LPW1
#define STPM3X_CR1_LPW1_BIT 24
#define STPM3X_CR1_LPW1_MASK 0xf
// LPS1
#define STPM3X_CR1_LPS1_BIT 28
#define STPM3X_CR1_LPS1_MASK 0x3
// LCS1
#define STPM3X_CR1_LCS1_BIT 30
#define STPM3X_CR1_LCS1_MASK 0x3

// CR2
#define STPM3X_CR2_ADDR 0x2
#define STPM3X_CR2_DEFAULT 0x240000a0
// CLRSS_TO2
#define STPM3X_CR2_CLRSS_TO2_BIT 0
#define STPM3X_CR2_CLRSS_TO2_MASK 0xf
// ClearSS2
#define STPM3X_CR2_ClearSS2_BIT 4
#define STPM3X_CR2_ClearSS2_MASK 1
// ENVREF2
#define STPM3X_CR2_ENVREF2_BIT 5
#define STPM3X_CR2_ENVREF2_MASK 1
// TC2
#define STPM3X_CR2_TC2_BIT 6
#define STPM3X_CR2_TC2_MASK 0x7
// AEM2
#define STPM3X_CR2_AEM2_BIT 17
#define STPM3X_CR2_AEM2_MASK 1
// APM2
#define STPM3X_CR2_APM2_BIT 18
#define STPM3X_CR2_APM2_MASK 1
// BHPFV2
#define STPM3X_CR2_BHPFV2_BIT 19
#define STPM3X_CR2_BHPFV2_MASK 1
// BHPFC2
#define STPM3X_CR2_BHPFC2_BIT 20
#define STPM3X_CR2_BHPFC2_MASK 1
// ROC2
#define STPM3X_CR2_ROC2_BIT 21
#define STPM3X_CR2_ROC2_MASK 1
// BLPFV2
#define STPM3X_CR2_BLPFV2_BIT 22
#define STPM3X_CR2_BLPFV2_MASK 1
// BLPFC2
#define STPM3X_CR2_BLPFC2_BIT 23
#define STPM3X_CR2_BLPFC2_MASK 1
// LPW2
#define STPM3X_CR2_LPW2_BIT 24
#define STPM3X_CR2_LPW2_MASK 0xf
// LPS2
#define STPM3X_CR2_LPS2_BIT 28
#define STPM3X_CR2_LPS2_MASK 0x3
// LCS2
#define STPM3X_CR2_LCS2_BIT 30
#define STPM3X_CR2_LCS2_MASK 0x3

// CR3
#define STPM3X_CR3_ADDR 0x4
#define STPM3X_CR3_DEFAULT 0x000004e0
// TIME_VALUE
#define STPM3X_CR3_TIME_VALUE_BIT 0
#define STPM3X_CR3_TIME_VALUE_MASK 0x3fff
// ZCR_SEL
#define STPM3X_CR3_ZCR_SEL_BIT 14
#define STPM3X_CR3_ZCR_SEL_MASK 0x3
// ZCR_EN
#define STPM3X_CR3_ZCR_EN_BIT 16
#define STPM3X_CR3_ZCR_EN_MASK 1
// TMP_TOL
#define STPM3X_CR3_TMP_TOL_BIT 17
#define STPM3X_CR3_TMP_TOL_MASK 0x3
// TMP_EN
#define STPM3X_CR3_TMP_EN_BIT 19
#define STPM3X_CR3_TMP_EN_MASK 1
// SW_Reset
#define STPM3X_CR3_SW_Reset_BIT 20
#define STPM3X_CR3_SW_Reset_MASK 1
// SW_Latch1
#define STPM3X_CR3_SW_Latch1_BIT 21
#define STPM3X_CR3_SW_Latch1_MASK 1
// SW_Latch2
#define STPM3X_CR3_SW_Latch2_BIT 22
#define STPM3X_CR3_SW_Latch2_MASK 1
// SW_AutoLatch
#define STPM3X_CR3_SW_AutoLatch_BIT 23
#define STPM3X_CR3_SW_AutoLatch_MASK 1
// LED1OFF
#define STPM3X_CR3_LED1OFF_BIT 24
#define STPM3X_CR3_LED1OFF_MASK 1
// LED2OFF
#define STPM3X_CR3_LED2OFF_BIT 25
#define STPM3X_CR3_LED2OFF_MASK 1
// EN_CUM
#define STPM3X_CR3_EN_CUM_BIT 26
#define STPM3X_CR3_EN_CUM_MASK 1
// REF_FREQ
#define STPM3X_CR3_REF_FREQ_BIT 27
#define STPM3X_CR3_REF_FREQ_MASK 1

// CR4
#define STPM3X_CR4_ADDR 0x6
#define STPM3X_CR4_DEFAULT 0x0
// PHC2
#define STPM3X_CR4_PHC2_BIT 0
#define STPM3X_CR4_PHC2_MASK 0x3ff
// PHV2
#define STPM3X_CR4_PHV2_BIT 10
#define STPM3X_CR4_PHV2_MASK 0x3
// PHC1
#define STPM3X_CR4_PHC1_BIT 12
#define STPM3X_CR4_PHC1_MASK 0x3ff
// PHV1
#define STPM3X_CR4_PHV1_BIT 22
#define STPM3X_CR4_PHV1_MASK 0x3

// CR5
#define STPM3X_CR5_ADDR 0x8
#define STPM3X_CR5_DEFAULT 0x003ff800
// CHV1
#define STPM3X_CR5_CHV1_BIT 0
#define STPM3X_CR5_CHV1_MASK 0xfff
// SWV_THR1
#define STPM3X_CR5_SWV_THR1_BIT 12
#define STPM3X_CR5_SWV_THR1_MASK 0x3ff
// SAG_THR1
#define STPM3X_CR5_SAG_THR1_BIT 22
#define STPM3X_CR5_SAG_THR1_MASK 0x3ff

// CR6
#define STPM3X_CR6_ADDR 0xa
#define STPM3X_CR6_DEFAULT 0x003ff800
// CHC1
#define STPM3X_CR6_CHC1_BIT 0
#define STPM3X_CR6_CHC1_MASK 0xfff
// SWC_THR1
#define STPM3X_CR6_SWC_THR1_BIT 12
#define STPM3X_CR6_SWC_THR1_MASK 0x3ff

// CR7
#define STPM3X_CR7_ADDR 0xc
#define STPM3X_CR7_DEFAULT 0x003ff800
// CHV2
#define STPM3X_CR7_CHV2_BIT 0
#define STPM3X_CR7_CHV2_MASK 0xfff
// SWV_THR2
#define STPM3X_CR7_SWV_THR2_BIT 12
#define STPM3X_CR7_SWV_THR2_MASK 0x3ff
// SAG_THR2
#define STPM3X_CR7_SAG_THR2_BIT 22
#define STPM3X_CR7_SAG_THR2_MASK 0x3ff

// CR8
#define STPM3X_CR8_ADDR 0xe
#define STPM3X_CR8_DEFAULT 0x003ff800
// CHC2
#define STPM3X_CR8_CHC2_BIT 0
#define STPM3X_CR8_CHC2_MASK 0xfff
// SWC_THR2
#define STPM3X_CR8_SWC_THR2_BIT 12
#define STPM3X_CR8_SWC_THR2_MASK 0x3ff

// CR9
#define STPM3X_CR9_ADDR 0x10
#define STPM3X_CR9_DEFAULT 0x00000fff
// AH_UP1
#define STPM3X_CR9_AH_UP1_BIT 0
#define STPM3X_CR9_AH_UP1_MASK 0xfff
// OFA1
#define STPM3X_CR9_OFA1_BIT 12
#define STPM3X_CR9_OFA1_MASK 0x3ff
// OFAF1
#define STPM3X_CR9_OFAF1_BIT 22
#define STPM3X_CR9_OFAF1_MASK 0x3ff

// CR10
#define STPM3X_CR10_ADDR 0x12
#define STPM3X_CR10_DEFAULT 0x00000fff
// AH_DOWN1
#define STPM3X_CR10_AH_DOWN1_BIT 0
#define STPM3X_CR10_AH_DOWN1_MASK 0xfff
// OFR1
#define STPM3X_CR10_OFR1_BIT 12
#define STPM3X_CR10_OFR1_MASK 0x3ff
// OFS1
#define STPM3X_CR10_OFS1_BIT 22
#define STPM3X_CR10_OFS1_MASK 0x3ff

// CR11
#define STPM3X_CR11_ADDR 0x14
#define STPM3X_CR11_DEFAULT 0x00000fff
// AH_UP2
#define STPM3X_CR11_AH_UP2_BIT 0
#define STPM3X_CR11_AH_UP2_MASK 0xfff
// OFA2
#define STPM3X_CR11_OFA2_BIT 12
#define STPM3X_CR11_OFA2_MASK 0x3ff
// OFAF2
#define STPM3X_CR11_OFAF2_BIT 22
#define STPM3X_CR11_OFAF2_MASK 0x3ff

// CR12
#define STPM3X_CR12_ADDR 0x16
#define STPM3X_CR12_DEFAULT 0x0000fff
// AH_DOWN2
#define STPM3X_CR12_AH_DOWN2_BIT 0
#define STPM3X_CR12_AH_DOWN2_MASK 0xfff
// OFR2
#define STPM3X_CR12_OFR2_BIT 12
#define STPM3X_CR12_OFR2_MASK 0x3ff
// OFS2
#define STPM3X_CR12_OFS2_BIT 22
#define STPM3X_CR12_OFS2_MASK 0x3ff

// DFE CR1
#define STPM3X_DFE_CR1_ADDR 0x18
#define STPM3X_DFE_CR1_DEFAULT 0x0f270327

// DFE CR2
#define STPM3X_DFE_CR2_ADDR 0x1a
#define STPM3X_DFE_CR2_DEFAULT 0x03270327

// DSP IRQ1
#define STPM3X_DSP_IRQ1_ADDR 0x1c
#define STPM3X_DSP_IRQ1_DEFAULT 0x0

// DSP IRQ2
#define STPM3X_DSP_IRQ2_ADDR 0x1e
#define STPM3X_DSP_IRQ2_DEFAULT 0x0

// DSP SR1
#define STPM3X_DSP_SR1_ADDR 0x20
#define STPM3X_DSP_SR1_DEFAULT 0x0

// DSP SR2
#define STPM3X_DSP_SR2_ADDR 0x22
#define STPM3X_DSP_SR2_DEFAULT 0x0

// US REG1
#define STPM3X_US_REG1_ADDR 0x24
#define STPM3X_US_REG1_DEFAULT 0x00004007

// US REG2
#define STPM3X_US_REG2_ADDR 0x26
#define STPM3X_US_REG2_DEFAULT 0x00000683

// US REG3
#define STPM3X_US_REG3_ADDR 0x28
#define STPM3X_US_REG3_DEFAULT 0x0

// DSP EV1
#define STPM3X_DSP_EV1_ADDR 0x2a

// DSP EV2
#define STPM3X_DSP_EV2_ADDR 0x2c

// DSP REG1
#define STPM3X_DSP_REG1_ADDR 0x2e

// DSP REG2
#define STPM3X_DSP_REG2_ADDR 0x30

// DSP REG3
#define STPM3X_DSP_REG3_ADDR 0x32

// DSP REG4
#define STPM3X_DSP_REG4_ADDR 0x34

// DSP REG5
#define STPM3X_DSP_REG5_ADDR 0x36

// DSP REG6
#define STPM3X_DSP_REG6_ADDR 0x38

// DSP REG7
#define STPM3X_DSP_REG7_ADDR 0x3a

// DSP REG8
#define STPM3X_DSP_REG8_ADDR 0x3c

// DSP REG9
#define STPM3X_DSP_REG9_ADDR 0x3e

// DSP REG10
#define STPM3X_DSP_REG10_ADDR 0x40

// DSP REG11
#define STPM3X_DSP_REG11_ADDR 0x42

// DSP REG12
#define STPM3X_DSP_REG12_ADDR 0x44

// DSP REG13
#define STPM3X_DSP_REG13_ADDR 0x46

// DSP REG14
#define STPM3X_DSP_REG14_ADDR 0x48

// DSP REG15
#define STPM3X_DSP_REG15_ADDR 0x4a

// DSP REG16
#define STPM3X_DSP_REG16_ADDR 0x4c

// DSP REG17
#define STPM3X_DSP_REG17_ADDR 0x4e

// DSP REG18
#define STPM3X_DSP_REG18_ADDR 0x50

// DSP REG19
#define STPM3X_DSP_REG19_ADDR 0x52

// PH1 REG1
#define STPM3X_PH1_REG1_ADDR 0x54

// PH1 REG2
#define STPM3X_PH1_REG2_ADDR 0x56

// PH1 REG3
#define STPM3X_PH1_REG3_ADDR 0x58

// PH1 REG4
#define STPM3X_PH1_REG4_ADDR 0x5a

// PH1 REG5
#define STPM3X_PH1_REG5_ADDR 0x5c

// PH1 REG6
#define STPM3X_PH1_REG6_ADDR 0x5e

// PH1 REG7
#define STPM3X_PH1_REG7_ADDR 0x60

// PH1 REG8
#define STPM3X_PH1_REG8_ADDR 0x62

// PH1 REG9
#define STPM3X_PH1_REG9_ADDR 0x64

// PH1 REG10
#define STPM3X_PH1_REG10_ADDR 0x66

// PH1 REG11
#define STPM3X_PH1_REG11_ADDR 0x68

// PH1 REG12
#define STPM3X_PH1_REG12_ADDR 0x6a

// PH2 REG1
#define STPM3X_PH2_REG1_ADDR 0x6c

// PH2 REG2
#define STPM3X_PH2_REG2_ADDR 0x6e

// PH2 REG3
#define STPM3X_PH2_REG3_ADDR 0x70

// PH2 REG4
#define STPM3X_PH2_REG4_ADDR 0x72

// PH2 REG5
#define STPM3X_PH2_REG5_ADDR 0x74

// PH2 REG6
#define STPM3X_PH2_REG6_ADDR 0x76

// PH2 REG7
#define STPM3X_PH2_REG7_ADDR 0x78

// PH2 REG8
#define STPM3X_PH2_REG8_ADDR 0x7a

// PH2 REG9
#define STPM3X_PH2_REG9_ADDR 0x7c

// PH2 REG10
#define STPM3X_PH2_REG10_ADDR 0x7e

// PH2 REG11
#define STPM3X_PH2_REG11_ADDR 0x80

// PH2 REG12
#define STPM3X_PH2_REG12_ADDR 0x82

// TOT REG1
#define STPM3X_TOT_REG1_ADDR 0x84

// TOT REG2
#define STPM3X_TOT_REG2_ADDR 0x86

// TOT REG3
#define STPM3X_TOT_REG3_ADDR 0x88

// TOT REG4
#define STPM3X_TOT_REG4_ADDR 0x8a

/*
// CR
#define STPM3X_CR_ADDR 0x
// 
#define STPM3X_CR__BIT 
#define STPM3X_CR__MASK 

// CR
#define STPM3X_CR_ADDR 0x
// 
#define STPM3X_CR__BIT 
#define STPM3X_CR__MASK 

// CR
#define STPM3X_CR_ADDR 0x
// 
#define STPM3X_CR__BIT 
#define STPM3X_CR__MASK 
*/


//! Размер пакета.
#define STPM3X_PACKET_SIZE 4
//! Размер адреса.
#define STPM3X_ADDRESS_SIZE 2


//! Сообщение команды.
#define STPM3X_CMD_MESSAGE 0
//! Сообщение данных.
#define STPM3X_DATA_MESSAGE 1


/**
 * Выставляет значения управляющих пинов для начала обмена данными с измерителем.
 * @param stpm3x Измеритель.
 */
ALWAYS_INLINE static void stpm3x_pins_begin(stpm3x_t* stpm3x)
{
    GPIO_ResetBits(stpm3x->ce_gpio, stpm3x->ce_pin);
}

/**
 * Выставлят значения управляющих пинов для окончания обмена данными с измерителем.
 * @param stpm3x Измеритель.
 */
ALWAYS_INLINE static void stpm3x_pins_end(stpm3x_t* stpm3x)
{
    GPIO_SetBits(stpm3x->ce_gpio, stpm3x->ce_pin);
}

/**
 * Ждёт завершения текущей операции.
 * @return true, если шина i2c занята нами и мы дождались, иначе false.
 */
static bool stpm3x_wait_current_op(stpm3x_t* stpm3x)
{
    // Если шина занята, да ещё и не нами - возврат ошибки занятости.
    if(spi_bus_busy(stpm3x->spi) && spi_bus_transfer_id(stpm3x->spi) != stpm3x->transfer_id) return false;
    
    future_wait(&stpm3x->future);
    spi_bus_wait(stpm3x->spi);
    
    return true;
}

/**
 * Начинает обмен данными с измерителем.
 * @param stpm3x Измеритель.
 */
static void stpm3x_begin(stpm3x_t* stpm3x)
{
    future_set_result(&stpm3x->future, int_to_pvoid(E_NO_ERROR));
    
    future_start(&stpm3x->future);
    
    stpm3x_pins_begin(stpm3x);
    
    // Включим SPI CRC и установим полином.
    if(stpm3x->crc_enabled){
        spi_bus_set_enabled(stpm3x->spi, false);
        spi_bus_set_crc_enabled(stpm3x->spi, false);
        spi_bus_set_crc_polynomial(stpm3x->spi, stpm3x->crc_polynomial);
        spi_bus_set_crc_enabled(stpm3x->spi, true);
        spi_bus_set_enabled(stpm3x->spi, true);
    }
}

/**
 * Завершает обмен данными с измерителем.
 * @param stpm3x Измеритель.
 * @param err Код ошибки.
 */
static void stpm3x_end(stpm3x_t* stpm3x, err_t err)
{
    if(stpm3x->crc_enabled){
        spi_bus_set_enabled(stpm3x->spi, false);
        spi_bus_set_crc_enabled(stpm3x->spi, false);
        spi_bus_set_enabled(stpm3x->spi, true);
    }
    
    future_finish(&stpm3x->future, int_to_pvoid(err));
    
    stpm3x_pins_end(stpm3x);
}

static void stpm3x_cmd_msg_end_impl(stpm3x_t* stpm3x)
{
#ifdef STPM3X_WAIT_SPI_BETWEEN_MSGS
    spi_bus_wait(stpm3x->spi);
#endif
        
#ifdef STPM3X_TOGGLE_CE_BETWEEN_MSGS
    GPIO_SetBits(stpm3x->ce_gpio, stpm3x->ce_pin);
#endif
        
#ifdef STPM3X_WAIT_4US_BETWEEN_MSGS
    // Подождём 4 мкс между транзакциями.
    delay_us(4);
#endif
    // Сбросим crc.
    if(stpm3x->crc_enabled){
        spi_bus_set_enabled(stpm3x->spi, false);
        spi_bus_reset_crc(stpm3x->spi);
        spi_bus_set_enabled(stpm3x->spi, true);
    }
        
#ifdef STPM3X_TOGGLE_CE_BETWEEN_MSGS
    GPIO_ResetBits(stpm3x->ce_gpio, stpm3x->ce_pin);
#endif
}

static void stpm3x_cmd_msg_end(spi_message_t* message)
{
    stpm3x_t* stpm3x = (stpm3x_t*)message->sender_data;
    
    if(stpm3x != NULL){
        stpm3x_cmd_msg_end_impl(stpm3x);
    }
}

/*
static void stpm3x_cmd_msg_end_burst(spi_message_t* message)
{
    stpm3x_t* stpm3x = (stpm3x_t*)message->sender_data;
    
    stpm3x_write_packet_t* wpckt = (stpm3x_read_write_packet_t*)message->tx_data;
    
    if(wpckt){
        wpckt->read_address = STPM3X_ADDRESS_DUMMY;
    }
    
    if(message->rx_data){
        message->rx_data = message->rx_data + sizeof(uint32_t);
    }
    
    if(stpm3x != NULL){
        stpm3x_cmd_msg_end_impl(stpm3x);
    }
}
*/

/**
 * Отправляет данные в измеритель.
 * Асинхронно.
 * @param stpm3x Измеритель.
 * @param messages_count Число сообщений.
 * @return Код ошибки.
 */
static err_t stpm3x_transfer(stpm3x_t* stpm3x, size_t messages_count)
{
    if(!spi_bus_set_transfer_id(stpm3x->spi, stpm3x->transfer_id)) return E_BUSY;
    
    stpm3x_begin(stpm3x);
    
    err_t err = spi_bus_transfer(stpm3x->spi, stpm3x->messages, messages_count);
    
    if(err != E_NO_ERROR){
        stpm3x_end(stpm3x, err);
        return err;
    }
    
    return E_NO_ERROR;
}

bool stpm3x_spi_callback(stpm3x_t* stpm3x)
{
    if(spi_bus_transfer_id(stpm3x->spi) != stpm3x->transfer_id) return false;
    
    stpm3x_end(stpm3x, spi_bus_status(stpm3x->spi) != SPI_STATUS_TRANSFERED ? E_IO_ERROR : E_NO_ERROR);
    
    return true;
}

err_t stpm3x_init(stpm3x_t* stpm3x, stpm3x_init_t* stpm3x_init)
{
    // Clear.
    memset(stpm3x, 0x0, sizeof(stpm3x_t));
    
    // SPI.
    stpm3x->spi = stpm3x_init->spi;
    
    // CE.
    stpm3x->ce_gpio = stpm3x_init->ce_gpio;
    stpm3x->ce_pin = stpm3x_init->ce_pin;
    
    // Syn.
    stpm3x->syn_gpio = stpm3x_init->syn_gpio;
    stpm3x->syn_pin = stpm3x_init->syn_pin;
    
    future_init(&stpm3x->future);
    
    // Значения по-умолчанию для этих пинов.
    stpm3x_pins_end(stpm3x);
    
    // Идентификатор передачи.
    stpm3x->transfer_id = stpm3x_init->transfer_id;
    
    // Полином CRC.
    stpm3x->crc_polynomial = stpm3x_init->crc_polynomial;
    
    return E_NO_ERROR;
}

bool stpm3x_busy(stpm3x_t* stpm3x)
{
    return future_running(&stpm3x->future);
}

err_t stpm3x_error(stpm3x_t* stpm3x)
{
    return pvoid_to_int(err_t, future_result(&stpm3x->future));
}

err_t stpm3x_wait(stpm3x_t* stpm3x)
{
    future_wait(&stpm3x->future);
    return pvoid_to_int(err_t, future_result(&stpm3x->future));
}

bool stpm3x_crc_enabled(stpm3x_t* stpm3x)
{
    return stpm3x->crc_enabled;
}

void stpm3x_set_crc_enabled(stpm3x_t* stpm3x, bool enabled)
{
    stpm3x->crc_enabled = enabled;
}

/**
 * Получает указатель на очередной пакет приёма/передачи.
 * @param stpm3x Измеритель.
 * @param index Значение предыдущего индекса.
 * @return Указатель на пакет.
 */
ALWAYS_INLINE static stpm3x_read_write_packet_t* stpm3x_get_packet(stpm3x_t* stpm3x, size_t* index)
{
#ifdef STPM3X_GET_MEM_DEBUG
    if(index == NULL) return NULL;
#endif
    
    size_t i = *index;
    
#ifdef STPM3X_GET_MEM_DEBUG
    if(i + size > STPM3X_PACKETS_COUNT) return NULL;
#endif
    
    (*index) ++;
    
    return &stpm3x->packets[i];
}

/**
 * Получает указатель на сообщение.
 * @param stpm3x Измеритель.
 * @param index Значение предыдущего индекса.
 * @return Указатель на данные в бефере.
 */
ALWAYS_INLINE static spi_message_t* stpm3x_get_message(stpm3x_t* stpm3x, size_t* index)
{
#ifdef STPM3X_GET_MEM_DEBUG
    if(index == NULL) return NULL;
#endif
    
    size_t i = *index;
    
#ifdef STPM3X_GET_MEM_DEBUG
    if(i >= STPM3X_MESSAGES_COUNT) return NULL;
#endif
    
    (*index) ++;
    
    return &stpm3x->messages[i];
}

err_t stpm3x_read_register(stpm3x_t* stpm3x, stpm3x_address_t addr, uint32_t* reg)
{
    if(reg == NULL) return E_NULL_POINTER;
    
    if(!stpm3x_wait_current_op(stpm3x)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    stpm3x_read_write_packet_t* cmd_packet = stpm3x_get_packet(stpm3x, &buffer_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(cmd_packet == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = stpm3x_get_message(stpm3x, &message_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    stpm3x_read_write_packet_t* data_packet = stpm3x_get_packet(stpm3x, &buffer_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(data_packet == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = stpm3x_get_message(stpm3x, &message_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    cmd_packet->write.read_address = addr;
    cmd_packet->write.write_address = STPM3X_ADDRESS_DUMMY;
    cmd_packet->write.data[0] = STPM3X_DATA_DUMMY;
    cmd_packet->write.data[1] = STPM3X_DATA_DUMMY;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_packet, NULL, STPM3X_PACKET_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_callback(cmd_msg, stpm3x_cmd_msg_end);
    spi_message_set_sender_data(cmd_msg, (void*)stpm3x);
    
    data_packet->write.read_address = STPM3X_ADDRESS_DUMMY;
    data_packet->write.write_address = STPM3X_ADDRESS_DUMMY;
    data_packet->write.data[0] = STPM3X_DATA_DUMMY;
    data_packet->write.data[1] = STPM3X_DATA_DUMMY;
    
    err = spi_message_init(data_msg, SPI_READ_WRITE, data_packet, data_packet, STPM3X_PACKET_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = stpm3x_transfer(stpm3x, 2);
    if(err != E_NO_ERROR) return err;
    
    err = stpm3x_wait(stpm3x);
    if(err != E_NO_ERROR) return err;
    
    *reg = data_packet->read.data32;
    
    return E_NO_ERROR;
}

err_t stpm3x_write_register(stpm3x_t* stpm3x, stpm3x_address_t addr, uint32_t reg)
{
    if(!stpm3x_wait_current_op(stpm3x)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    stpm3x_read_write_packet_t* cmd_packet = stpm3x_get_packet(stpm3x, &buffer_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(cmd_packet == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = stpm3x_get_message(stpm3x, &message_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    stpm3x_read_write_packet_t* data_packet = stpm3x_get_packet(stpm3x, &buffer_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(data_packet == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* data_msg = stpm3x_get_message(stpm3x, &message_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(data_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    cmd_packet->write.read_address = STPM3X_ADDRESS_DUMMY;
    cmd_packet->write.write_address = addr;
    cmd_packet->write.data[0] = (reg     ) & 0xff;
    cmd_packet->write.data[1] = (reg >> 8) & 0xff;
    
    err = spi_message_init(cmd_msg, SPI_WRITE, cmd_packet, NULL, STPM3X_PACKET_SIZE);
    if(err != E_NO_ERROR) return err;
    spi_message_set_callback(cmd_msg, stpm3x_cmd_msg_end);
    spi_message_set_sender_data(cmd_msg, (void*)stpm3x);
    
    data_packet->write.read_address = STPM3X_ADDRESS_DUMMY;
    data_packet->write.write_address = addr + 1;
    data_packet->write.data[0] = (reg >> 16) & 0xff;
    data_packet->write.data[1] = (reg >> 24) & 0xff;
    
    err = spi_message_init(data_msg, SPI_WRITE, data_packet, NULL, STPM3X_PACKET_SIZE);
    if(err != E_NO_ERROR) return err;
    
    err = stpm3x_transfer(stpm3x, 2);
    if(err != E_NO_ERROR) return err;
    
    err = stpm3x_wait(stpm3x);
    if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

err_t stpm3x_regs_read(stpm3x_t* stpm3x, stpm3x_address_t addr, void* regs, size_t count)
{
    if(regs == NULL) return E_NULL_POINTER;
    if(count == 0) return E_INVALID_VALUE;
    if(count > STPM3X_REGS_READS_COUNT_MAX) return E_OUT_OF_RANGE;
    
    if(!stpm3x_wait_current_op(stpm3x)) return E_BUSY;
    
    err_t err = E_NO_ERROR;
    
    size_t buffer_index = 0;
    size_t message_index = 0;
    
    stpm3x_read_write_packet_t* cmd_packet = stpm3x_get_packet(stpm3x, &buffer_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(cmd_packet == NULL) return E_OUT_OF_MEMORY;
#endif
    
    spi_message_t* cmd_msg = stpm3x_get_message(stpm3x, &message_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(cmd_msg == NULL) return E_OUT_OF_MEMORY;
#endif
    
    stpm3x_read_write_packet_t* data_packet = stpm3x_get_packet(stpm3x, &buffer_index);
#ifdef STPM3X_GET_MEM_DEBUG
    if(data_packet == NULL) return E_OUT_OF_MEMORY;
#endif
    
    cmd_packet->write.read_address = addr;
    cmd_packet->write.write_address = STPM3X_ADDRESS_DUMMY;
    cmd_packet->write.data[0] = STPM3X_DATA_DUMMY;
    cmd_packet->write.data[1] = STPM3X_DATA_DUMMY;
    
    spi_message_setup(cmd_msg, SPI_WRITE, cmd_packet, NULL, STPM3X_PACKET_SIZE);
    //if(err != E_NO_ERROR) return err;
    spi_message_set_callback(cmd_msg, stpm3x_cmd_msg_end);
    spi_message_set_sender_data(cmd_msg, (void*)stpm3x);
    
    data_packet->write.read_address = STPM3X_ADDRESS_DUMMY;
    data_packet->write.write_address = STPM3X_ADDRESS_DUMMY;
    data_packet->write.data[0] = STPM3X_DATA_DUMMY;
    data_packet->write.data[1] = STPM3X_DATA_DUMMY;
    
    size_t i = 0;
    for(; i < count; i ++){
        spi_message_t* data_msg = stpm3x_get_message(stpm3x, &message_index);
        #ifdef STPM3X_GET_MEM_DEBUG
            if(data_msg == NULL) return E_OUT_OF_MEMORY;
        #endif
    
        spi_message_setup(data_msg, SPI_READ_WRITE, data_packet, regs, STPM3X_PACKET_SIZE);
        //if(err != E_NO_ERROR) return err;
        if(i < count - 1){
            spi_message_set_callback(data_msg, stpm3x_cmd_msg_end);
            spi_message_set_sender_data(data_msg, (void*)stpm3x);
        }
        
        regs += sizeof(uint32_t);
    }
    
    err = stpm3x_transfer(stpm3x, count + 1);
    if(err != E_NO_ERROR) return err;
    
    //err = stpm3x_wait(stpm3x);
    //if(err != E_NO_ERROR) return err;
    
    return E_NO_ERROR;
}

void stpm3x_reset_dsp_cr1(stpm3x_t* stpm3x, stpm32_dsp_cr1_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR1_DEFAULT;
}

err_t stpm3x_read_dsp_cr1(stpm3x_t* stpm3x, stpm32_dsp_cr1_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR1_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr1(stpm3x_t* stpm3x, stpm32_dsp_cr1_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR1_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr2(stpm3x_t* stpm3x, stpm32_dsp_cr2_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR2_DEFAULT;
}

err_t stpm3x_read_dsp_cr2(stpm3x_t* stpm3x, stpm32_dsp_cr2_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR2_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr2(stpm3x_t* stpm3x, stpm32_dsp_cr2_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR2_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr3(stpm3x_t* stpm3x, stpm32_dsp_cr3_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR3_DEFAULT;
}

err_t stpm3x_read_dsp_cr3(stpm3x_t* stpm3x, stpm32_dsp_cr3_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR3_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr3(stpm3x_t* stpm3x, stpm32_dsp_cr3_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR3_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr4(stpm3x_t* stpm3x, stpm32_dsp_cr4_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR4_DEFAULT;
}

err_t stpm3x_read_dsp_cr4(stpm3x_t* stpm3x, stpm32_dsp_cr4_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR4_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr4(stpm3x_t* stpm3x, stpm32_dsp_cr4_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR4_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr5(stpm3x_t* stpm3x, stpm32_dsp_cr5_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR5_DEFAULT;
}

err_t stpm3x_read_dsp_cr5(stpm3x_t* stpm3x, stpm32_dsp_cr5_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR5_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr5(stpm3x_t* stpm3x, stpm32_dsp_cr5_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR5_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr6(stpm3x_t* stpm3x, stpm32_dsp_cr6_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR6_DEFAULT;
}

err_t stpm3x_read_dsp_cr6(stpm3x_t* stpm3x, stpm32_dsp_cr6_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR6_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr6(stpm3x_t* stpm3x, stpm32_dsp_cr6_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR6_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr7(stpm3x_t* stpm3x, stpm32_dsp_cr7_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR7_DEFAULT;
}

err_t stpm3x_read_dsp_cr7(stpm3x_t* stpm3x, stpm32_dsp_cr7_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR7_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr7(stpm3x_t* stpm3x, stpm32_dsp_cr7_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR7_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr8(stpm3x_t* stpm3x, stpm32_dsp_cr8_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR8_DEFAULT;
}

err_t stpm3x_read_dsp_cr8(stpm3x_t* stpm3x, stpm32_dsp_cr8_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR8_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr8(stpm3x_t* stpm3x, stpm32_dsp_cr8_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR8_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr9(stpm3x_t* stpm3x, stpm32_dsp_cr9_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR9_DEFAULT;
}

err_t stpm3x_read_dsp_cr9(stpm3x_t* stpm3x, stpm32_dsp_cr9_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR9_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr9(stpm3x_t* stpm3x, stpm32_dsp_cr9_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR9_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr10(stpm3x_t* stpm3x, stpm32_dsp_cr10_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR10_DEFAULT;
}

err_t stpm3x_read_dsp_cr10(stpm3x_t* stpm3x, stpm32_dsp_cr10_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR10_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr10(stpm3x_t* stpm3x, stpm32_dsp_cr10_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR10_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr11(stpm3x_t* stpm3x, stpm32_dsp_cr11_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR11_DEFAULT;
}

err_t stpm3x_read_dsp_cr11(stpm3x_t* stpm3x, stpm32_dsp_cr11_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR11_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr11(stpm3x_t* stpm3x, stpm32_dsp_cr11_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR11_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_cr12(stpm3x_t* stpm3x, stpm32_dsp_cr12_t* reg)
{
    *(uint32_t*)reg = STPM3X_CR12_DEFAULT;
}

err_t stpm3x_read_dsp_cr12(stpm3x_t* stpm3x, stpm32_dsp_cr12_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_CR12_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_cr12(stpm3x_t* stpm3x, stpm32_dsp_cr12_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_CR12_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dfe_cr1(stpm3x_t* stpm3x, stpm32_dfe_cr1_t* reg)
{
    *(uint32_t*)reg = STPM3X_DFE_CR1_DEFAULT;
}

err_t stpm3x_read_dfe_cr1(stpm3x_t* stpm3x, stpm32_dfe_cr1_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DFE_CR1_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dfe_cr1(stpm3x_t* stpm3x, stpm32_dfe_cr1_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_DFE_CR1_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dfe_cr2(stpm3x_t* stpm3x, stpm32_dfe_cr2_t* reg)
{
    *(uint32_t*)reg = STPM3X_DFE_CR2_DEFAULT;
}

err_t stpm3x_read_dfe_cr2(stpm3x_t* stpm3x, stpm32_dfe_cr2_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DFE_CR2_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dfe_cr2(stpm3x_t* stpm3x, stpm32_dfe_cr2_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_DFE_CR2_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_irq1(stpm3x_t* stpm3x, stpm32_dsp_irq1_t* reg)
{
    *(uint32_t*)reg = STPM3X_DSP_IRQ1_DEFAULT;
}

err_t stpm3x_read_dsp_irq1(stpm3x_t* stpm3x, stpm32_dsp_irq1_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_IRQ1_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_irq1(stpm3x_t* stpm3x, stpm32_dsp_irq1_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_DSP_IRQ1_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_irq2(stpm3x_t* stpm3x, stpm32_dsp_irq2_t* reg)
{
    *(uint32_t*)reg = STPM3X_DSP_IRQ2_DEFAULT;
}

err_t stpm3x_read_dsp_irq2(stpm3x_t* stpm3x, stpm32_dsp_irq2_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_IRQ2_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_irq2(stpm3x_t* stpm3x, stpm32_dsp_irq2_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_DSP_IRQ2_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_sr1(stpm3x_t* stpm3x, stpm32_dsp_sr1_t* reg)
{
    *(uint32_t*)reg = STPM3X_DSP_SR1_DEFAULT;
}

err_t stpm3x_read_dsp_sr1(stpm3x_t* stpm3x, stpm32_dsp_sr1_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_SR1_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_sr1(stpm3x_t* stpm3x, stpm32_dsp_sr1_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_DSP_SR1_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_dsp_sr2(stpm3x_t* stpm3x, stpm32_dsp_sr2_t* reg)
{
    *(uint32_t*)reg = STPM3X_DSP_SR2_DEFAULT;
}

err_t stpm3x_read_dsp_sr2(stpm3x_t* stpm3x, stpm32_dsp_sr2_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_SR2_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_dsp_sr2(stpm3x_t* stpm3x, stpm32_dsp_sr2_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_DSP_SR2_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_us_reg1(stpm3x_t* stpm3x, stpm32_us_reg1_t* reg)
{
    *(uint32_t*)reg = STPM3X_US_REG1_DEFAULT;
}

err_t stpm3x_read_us_reg1(stpm3x_t* stpm3x, stpm32_us_reg1_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_US_REG1_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_us_reg1(stpm3x_t* stpm3x, stpm32_us_reg1_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_US_REG1_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_us_reg2(stpm3x_t* stpm3x, stpm32_us_reg2_t* reg)
{
    *(uint32_t*)reg = STPM3X_US_REG2_DEFAULT;
}

err_t stpm3x_read_us_reg2(stpm3x_t* stpm3x, stpm32_us_reg2_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_US_REG2_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_us_reg2(stpm3x_t* stpm3x, stpm32_us_reg2_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_US_REG2_ADDR, *(uint32_t*)reg);
}


void stpm3x_reset_us_reg3(stpm3x_t* stpm3x, stpm32_us_reg3_t* reg)
{
    *(uint32_t*)reg = STPM3X_US_REG3_DEFAULT;
}

err_t stpm3x_read_us_reg3(stpm3x_t* stpm3x, stpm32_us_reg3_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_US_REG3_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_us_reg3(stpm3x_t* stpm3x, stpm32_us_reg3_t* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_US_REG3_ADDR, *(uint32_t*)reg);
}


err_t stpm3x_read_dsp_ev1(stpm3x_t* stpm3x, stpm32_dsp_ev1_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_EV1_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_ev2(stpm3x_t* stpm3x, stpm32_dsp_ev2_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_EV2_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg1(stpm3x_t* stpm3x, stpm32_dsp_reg1_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG1_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg2(stpm3x_t* stpm3x, stpm32_dsp_reg2_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG2_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg3(stpm3x_t* stpm3x, stpm32_dsp_reg3_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG3_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg4(stpm3x_t* stpm3x, stpm32_dsp_reg4_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG4_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg5(stpm3x_t* stpm3x, stpm32_dsp_reg5_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG5_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg6(stpm3x_t* stpm3x, stpm32_dsp_reg6_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG6_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg7(stpm3x_t* stpm3x, stpm32_dsp_reg7_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG7_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg8(stpm3x_t* stpm3x, stpm32_dsp_reg8_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG8_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg9(stpm3x_t* stpm3x, stpm32_dsp_reg9_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG9_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg14(stpm3x_t* stpm3x, stpm32_dsp_reg14_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG14_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg15(stpm3x_t* stpm3x, stpm32_dsp_reg15_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG15_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg16(stpm3x_t* stpm3x, stpm32_dsp_reg16_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG16_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg17(stpm3x_t* stpm3x, stpm32_dsp_reg17_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG17_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg18(stpm3x_t* stpm3x, stpm32_dsp_reg18_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG18_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_dsp_reg19(stpm3x_t* stpm3x, stpm32_dsp_reg19_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_DSP_REG19_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg1(stpm3x_t* stpm3x, stpm32_ph1_reg1_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG1_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg2(stpm3x_t* stpm3x, stpm32_ph1_reg2_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG2_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg3(stpm3x_t* stpm3x, stpm32_ph1_reg3_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG3_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg4(stpm3x_t* stpm3x, stpm32_ph1_reg4_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG4_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg5(stpm3x_t* stpm3x, stpm32_ph1_reg5_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG5_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg6(stpm3x_t* stpm3x, stpm32_ph1_reg6_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG6_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg7(stpm3x_t* stpm3x, stpm32_ph1_reg7_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG7_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg8(stpm3x_t* stpm3x, stpm32_ph1_reg8_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG8_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg9(stpm3x_t* stpm3x, stpm32_ph1_reg9_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG9_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg10(stpm3x_t* stpm3x, stpm32_ph1_reg10_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG10_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg11(stpm3x_t* stpm3x, stpm32_ph1_reg11_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG11_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph1_reg12(stpm3x_t* stpm3x, stpm32_ph1_reg12_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH1_REG12_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg1(stpm3x_t* stpm3x, stpm32_ph2_reg1_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG1_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg2(stpm3x_t* stpm3x, stpm32_ph2_reg2_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG2_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg3(stpm3x_t* stpm3x, stpm32_ph2_reg3_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG3_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg4(stpm3x_t* stpm3x, stpm32_ph2_reg4_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG4_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg5(stpm3x_t* stpm3x, stpm32_ph2_reg5_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG5_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg6(stpm3x_t* stpm3x, stpm32_ph2_reg6_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG6_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg7(stpm3x_t* stpm3x, stpm32_ph2_reg7_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG7_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg8(stpm3x_t* stpm3x, stpm32_ph2_reg8_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG8_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg9(stpm3x_t* stpm3x, stpm32_ph2_reg9_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG9_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg10(stpm3x_t* stpm3x, stpm32_ph2_reg10_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG10_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg11(stpm3x_t* stpm3x, stpm32_ph2_reg11_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG11_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_ph2_reg12(stpm3x_t* stpm3x, stpm32_ph2_reg12_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_PH2_REG12_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_tot_reg1(stpm3x_t* stpm3x, stpm32_tot_reg1_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_TOT_REG1_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_tot_reg2(stpm3x_t* stpm3x, stpm32_tot_reg2_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_TOT_REG2_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_tot_reg3(stpm3x_t* stpm3x, stpm32_tot_reg3_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_TOT_REG3_ADDR, (uint32_t*)reg);
}


err_t stpm3x_read_tot_reg4(stpm3x_t* stpm3x, stpm32_tot_reg4_t* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_TOT_REG4_ADDR, (uint32_t*)reg);
}


/*
void stpm3x_reset_regname(stpm3x_t* stpm3x, regtype* reg)
{
    *(uint32_t*)reg = STPM3X_REGNAME_DEFAULT;
}

err_t stpm3x_read_regname(stpm3x_t* stpm3x, regtype* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_REGNAME_ADDR, (uint32_t*)reg);
}

err_t stpm3x_write_regname(stpm3x_t* stpm3x, regtype* reg)
{
    return stpm3x_write_register(stpm3x, STPM3X_REGNAME_ADDR, *(uint32_t*)reg);
}
*/
