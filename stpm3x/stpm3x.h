/**
 * @file stpm.h Библиотека работы с измерителями stpm3x.
 */

#ifndef STPM3X_H
#define STPM3X_H

#include <stm32f10x.h>
#include <stddef.h>
#include <stdint.h>
#include "errors/errors.h"
#include "future/future.h"
#include "spi/spi.h"
#include "defs/defs.h"

//! Идентификатор передачи по-умолчанию.
#define STPM3X_DEFAULT_TRANSFER_ID 32

//! Тип адреса памяти измерителя.
typedef uint8_t stpm3x_address_t;

//! Адрес-заглушка.
#define STPM3X_ADDRESS_DUMMY 0xff

//! Данные-заглушка.
#define STPM3X_DATA_DUMMY 0xff

//! Размер данных в пакете записи.
#define STPM3X_WRITE_DATA_SIZE 2

//! Размер данных в пакете чтения.
#define STPM3X_READ_DATA_SIZE 4

//! Пакет для отправки данных.
#pragma pack(push, 1)
typedef struct _STPM3X_Write_Packet {
    stpm3x_address_t read_address; //!< Адрес для чтения.
    stpm3x_address_t write_address; //!< Адрес для записи.
    uint8_t data[STPM3X_WRITE_DATA_SIZE]; //!< Данные для записи.
} stpm3x_write_packet_t;
#pragma pack(pop)

//! Пакет для получения данных.
#pragma pack(push, 1)
typedef union _STPM3X_Read_Packet {
    uint8_t data[STPM3X_READ_DATA_SIZE]; //!< Буфер для чтения.
    uint32_t data32; //!< Представление прочитанных данных 32 бит.
} stpm3x_read_packet_t;
#pragma pack(pop)

//! Объединение пакетов передачи и приёма данных.
#pragma pack(push, 1)
typedef union _STPM3X_Read_Write_Packet {
    stpm3x_write_packet_t write;
    stpm3x_read_packet_t read;
} stpm3x_read_write_packet_t;
#pragma pack(pop)

//! Размер буфера данных.
#define STPM3X_PACKETS_COUNT 2
//! Максимальное число подряд считываемых регистров.
#define STPM3X_REGS_READS_COUNT_MAX 4
//! Число доступных сообщений SPI.
#define STPM3X_MESSAGES_COUNT (STPM3X_REGS_READS_COUNT_MAX + 1)

//! Тип структуры памяти stpm3x.
typedef struct _STPM3X {
    //! Шина SPI.
    spi_bus_t* spi;
    //! Идентификатор передачи.
    spi_transfer_id_t transfer_id;
    //! Полином CRC SPI.
    uint8_t crc_polynomial;
    //! Разрешение CRC.
    bool crc_enabled;
    //! Порт выбора ведомого.
    GPIO_TypeDef* ce_gpio;
    //! Пин выбора ведомого.
    uint16_t ce_pin;
    //! Порт синхронизации данных.
    GPIO_TypeDef* syn_gpio;
    //! Пин синхронизации данных.
    uint16_t syn_pin;
    //! Будущее.
    future_t future;
    //! Буфер данных.
    stpm3x_read_write_packet_t packets[STPM3X_PACKETS_COUNT];
    //! Сообщения SPI.
    spi_message_t messages[STPM3X_MESSAGES_COUNT];
} stpm3x_t;

/**
 * Тип структуры инициализации дисплея.
 */
typedef struct _STPM3X_Init {
    spi_bus_t*          spi; //!< Шина SPI.
    spi_transfer_id_t   transfer_id; //!< Идентификатор передачи.
    uint8_t crc_polynomial; //!< Полином CRC SPI.
    GPIO_TypeDef*       ce_gpio; //!< Порт выбора ведомого.
    uint16_t            ce_pin; //!< Пин выбора ведомого.
    GPIO_TypeDef*       syn_gpio; //!< Порт синхронизации данных.
    uint16_t            syn_pin; //!< Пин синхронизации данных.
} stpm3x_init_t;


/**
 * Каллбэк SPI.
 * @return true, если событие обработано, иначе false.
 */
EXTERN bool stpm3x_spi_callback(stpm3x_t* stpm3x);

/**
 * Инициализирует измеритель.
 * @param stpm3x Измеритель.
 * @param stpm3x_init Структура инициализации.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_init(stpm3x_t* stpm3x, stpm3x_init_t* stpm3x_init);

/**
 * Получает флаг занятости измерителя.
 * @param stpm3x Измеритель.
 * @return Флаг занятости измерителя.
 */
EXTERN bool stpm3x_busy(stpm3x_t* stpm3x);

/**
 * Получает код ошибки асинхронно выполненой операции.
 * @param stpm3x Измеритель.
 * @return Код ошибки асинхронно выполненой операции.
 */
EXTERN err_t stpm3x_error(stpm3x_t* stpm3x);

/**
 * Ожидает завершения операции с измерителем.
 * @param stpm3x Измеритель.
 * @return Код ошибки операции.
 */
EXTERN err_t stpm3x_wait(stpm3x_t* stpm3x);

/**
 * Получает флаг разрешения использования контрольной суммы.
 * Не обращается к микросхеме, возвращает локальную установку.
 * @param stpm3x Измеритель.
 * @return Флаг разрешения использования контрольной суммы.
 */
EXTERN bool stpm3x_crc_enabled(stpm3x_t* stpm3x);

/**
 * Устанавливает флаг разрешения использования контрольной суммы.
 * Не обращается к микросхеме, меняет локальную установку.
 * @param stpm3x Измеритель.
 * @param enabled Флаг разрешения использования контрольной суммы.
 */
EXTERN void stpm3x_set_crc_enabled(stpm3x_t* stpm3x, bool enabled);

/**
 * Считывает регистр.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param addr Адрес регистра.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_register(stpm3x_t* stpm3x, stpm3x_address_t addr, uint32_t* reg);

/**
 * Записывает регистр.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param addr Адрес регистра.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_register(stpm3x_t* stpm3x, stpm3x_address_t addr, uint32_t reg);

/**
 * Считывает последовательно несколько 32 битных регистров.
 * Асинхронная операция.
 * @param stpm3x Измеритель.
 * @param addr Адрес первого регистра.
 * @param regs Указатель на регистры для чтения.
 * @param count Количество регистров для чтения.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_regs_read(stpm3x_t* stpm3x, stpm3x_address_t addr, void* regs, size_t count);


// DSP_CR1
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr1 {
    unsigned int clrss_to1:4;
    unsigned int clearss1:1;
    unsigned int envref1:1;
    unsigned int tc1:3;
    unsigned int reserved0:8;
    unsigned int aem1:1;
    unsigned int apm1:1;
    unsigned int bhpfv1:1;
    unsigned int bhpfc1:1;
    unsigned int roc1:1;
    unsigned int blpfv1:1;
    unsigned int blpfc1:1;
    unsigned int lpw1:4;
    unsigned int lps1:2;
    unsigned int lcs1:2;
}stpm32_dsp_cr1_t;
#pragma pack(pop)

// DSP_CR2
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr2 {
    unsigned int clrss_to2:4;
    unsigned int clearss2:1;
    unsigned int envref2:1;
    unsigned int tc2:3;
    unsigned int reserved0:8;
    unsigned int aem2:1;
    unsigned int apm2:1;
    unsigned int bhpfv2:1;
    unsigned int bhpfc2:1;
    unsigned int roc2:1;
    unsigned int blpfv2:1;
    unsigned int blpfc2:1;
    unsigned int lpw2:4;
    unsigned int lps2:2;
    unsigned int lcs2:2;
}stpm32_dsp_cr2_t;
#pragma pack(pop)

// DSP_CR3
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr3 {
    unsigned int time_value:14;
    unsigned int zcr_sel:2;
    unsigned int zcr_en:1;
    unsigned int tmp_tol:2;
    unsigned int tmp_en:1;
    unsigned int sw_reset:1;
    unsigned int sw_latch1:1;
    unsigned int sw_latch2:1;
    unsigned int sw_auto_latch:1;
    unsigned int led1off:1;
    unsigned int led2off:1;
    unsigned int en_cum:1;
    unsigned int ref_req:1;
    unsigned int reserved0:4;
}stpm32_dsp_cr3_t;
#pragma pack(pop)

// DSP_CR4
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr4 {
    unsigned int phc2:10;
    unsigned int phv2:2;
    unsigned int phc1:10;
    unsigned int phv1:2;
    unsigned int reserved0:8;
}stpm32_dsp_cr4_t;
#pragma pack(pop)

// DSP_CR5
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr5 {
    unsigned int chv1:12;
    unsigned int swv_thr1:10;
    unsigned int sag_thr1:10;
}stpm32_dsp_cr5_t;
#pragma pack(pop)

// DSP_CR6
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr6 {
    unsigned int chc1:12;
    unsigned int swc_thr1:10;
    unsigned int reserved0:10;
}stpm32_dsp_cr6_t;
#pragma pack(pop)

// DSP_CR7
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr7 {
    unsigned int chv2:12;
    unsigned int swv_thr2:10;
    unsigned int sag_thr2:10;
}stpm32_dsp_cr7_t;
#pragma pack(pop)

// DSP_CR8
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr8 {
    unsigned int chc2:12;
    unsigned int swc_thr2:10;
    unsigned int reserved0:10;
}stpm32_dsp_cr8_t;
#pragma pack(pop)

// DSP_CR9
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr9 {
    unsigned int ah_up1:12;
    unsigned int ofa1:10;
    unsigned int ofaf1:10;
}stpm32_dsp_cr9_t;
#pragma pack(pop)

// DSP_CR10
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr10 {
    unsigned int ah_down1:12;
    unsigned int ofr1:10;
    unsigned int ofs1:10;
}stpm32_dsp_cr10_t;
#pragma pack(pop)

// DSP_CR11
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr11 {
    unsigned int ah_up2:12;
    unsigned int ofa2:10;
    unsigned int ofaf2:10;
}stpm32_dsp_cr11_t;
#pragma pack(pop)

// DSP_CR12
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Cr12 {
    unsigned int ah_down2:12;
    unsigned int ofr2:10;
    unsigned int ofs2:10;
}stpm32_dsp_cr12_t;
#pragma pack(pop)

// DFE_CR1
#pragma pack(push, 1)
typedef struct _Stpm32_Dfe_Cr1 {
    unsigned int reserved0:26;
    unsigned int gain1:2;
    unsigned int reserved1:4;
}stpm32_dfe_cr1_t;
#pragma pack(pop)

// DFE_CR2
#pragma pack(push, 1)
typedef struct _Stpm32_Dfe_Cr2 {
    unsigned int reserved0:26;
    unsigned int gain2:2;
    unsigned int reserved1:4;
}stpm32_dfe_cr2_t;
#pragma pack(pop)

// DSP_IRQ1
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Irq1 {
    unsigned int cum_irq_cr:4;
    unsigned int ph2_irq_cr:8;
    unsigned int ph1_irq_cr:8;
    unsigned int c1_irq_cr:4;
    unsigned int v1_irq_cr:6;
    unsigned int tamper:2;
}stpm32_dsp_irq1_t;
#pragma pack(pop)

// DSP_IRQ2
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Irq2 {
    unsigned int ph1_ph2_irq_cr:4;
    unsigned int ph2_irq_cr:8;
    unsigned int ph1_irq_cr:8;
    unsigned int c2_irq_cr:4;
    unsigned int v2_irq_cr:6;
    unsigned int tamper:2;
}stpm32_dsp_irq2_t;
#pragma pack(pop)

// DSP_SR1
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Sr1 {
    unsigned int ph1_ph2_status:4;
    unsigned int ph2_irq_status:8;
    unsigned int ph1_irq_status:8;
    unsigned int c1_irq_status:4;
    unsigned int v1_irq_status:6;
    unsigned int tamper:2;
}stpm32_dsp_sr1_t;
#pragma pack(pop)

// DSP_SR2
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Sr2 {
    unsigned int ph1_ph2_status:4;
    unsigned int ph2_status:8;
    unsigned int ph1_status:8;
    unsigned int c1_status:4;
    unsigned int v1_status:6;
    unsigned int tamper:2;
}stpm32_dsp_sr2_t;
#pragma pack(pop)

// US_REG1
#pragma pack(push, 1)
typedef struct _Stpm32_Us_Reg1 {
    unsigned int crc_polynomial:8;
    unsigned int noise_detect_enable:1;
    unsigned int break_on_error:1;
    unsigned int reserved0:4;
    unsigned int crc_enable:1;
    unsigned int lsb_first:1;
    unsigned int time_out:8;
    unsigned int reserved1:8;
}stpm32_us_reg1_t;
#pragma pack(pop)

// US_REG2
#pragma pack(push, 1)
typedef struct _Stpm32_Us_Reg2 {
    unsigned int baud:16;
    unsigned int frame_delay:8;
    unsigned int reserved0:8;
}stpm32_us_reg2_t;
#pragma pack(pop)

// US_REG3
#pragma pack(push, 1)
typedef struct _Stpm32_Us_Reg3 {
    unsigned int uart_break_irq_enable:1;
    unsigned int uart_crc_err_irq_enable:1;
    unsigned int uart_timeout_err_irq_enable:1;
    unsigned int uart_framing_err_irq_enable:1;
    unsigned int uart_noise_err_irq_enable:1;
    unsigned int uart_rx_overrun_irq_enable:1;
    unsigned int uart_tx_overrun_irq_enable:1;
    unsigned int reserved0:3;
    unsigned int uart_spi_read_err_irq_enable:1;
    unsigned int uart_spi_write_err_irq_enable:1;
    unsigned int spi_crc_error_irq_enable:1;
    unsigned int spi_tx_underrun_irq_enable:1;
    unsigned int spi_rx_overrun_irq_enable:1;
    unsigned int reserved1:1;
    unsigned int uart_break_irq:1;
    unsigned int uart_crc_err_irq:1;
    unsigned int uart_timeout_err_irq:1;
    unsigned int uart_framing_err_irq:1;
    unsigned int uart_noise_err_irq:1;
    unsigned int uart_rx_overrun_irq:1;
    unsigned int uart_tx_overrun_irq:1;
    unsigned int reserved2:1;
    unsigned int spi_rx_full_irq:1;
    unsigned int spi_tx_empty_irq:1;
    unsigned int uart_spi_read_err_irq:1;
    unsigned int uart_spi_write_err_irq:1;
    unsigned int spi_crc_error_irq:1;
    unsigned int spi_tx_underrun_irq:1;
    unsigned int spi_rx_overrun_irq:1;
    unsigned int reserved3:1;
}stpm32_us_reg3_t;
#pragma pack(pop)

// DSP_EV1
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Ev1 {
    unsigned int ph1_ph2_events:4;
    unsigned int ph1_events:8;
    unsigned int c1_events:7;
    unsigned int v1_events:11;
    unsigned int reserved0:2;
}stpm32_dsp_ev1_t;
#pragma pack(pop)

// DSP_EV2
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Ev2 {
    unsigned int ph1_ph2_events:4;
    unsigned int ph2_events:8;
    unsigned int c2_events:7;
    unsigned int v2_events:11;
    unsigned int reserved0:2;
}stpm32_dsp_ev2_t;
#pragma pack(pop)

// DSP_REG1
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg1 {
    unsigned int ph1_period:12;
    unsigned int reserved0:4;
    unsigned int ph2_period:12;
    unsigned int reserved1:4;
}stpm32_dsp_reg1_t;
#pragma pack(pop)

// DSP_REG2
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg2 {
    unsigned int v1_data:24;
    unsigned int reserved0:8;
}stpm32_dsp_reg2_t;
#pragma pack(pop)

// DSP_REG3
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg3 {
    unsigned int c1_data:24;
    unsigned int reserved0:8;
}stpm32_dsp_reg3_t;
#pragma pack(pop)

// DSP_REG4
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg4 {
    unsigned int v2_data:24;
    unsigned int reserved0:8;
}stpm32_dsp_reg4_t;
#pragma pack(pop)

// DSP_REG5
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg5 {
    unsigned int c2_data:24;
    unsigned int reserved0:8;
}stpm32_dsp_reg5_t;
#pragma pack(pop)

// DSP_REG6
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg6 {
    unsigned int v1_fund:24;
    unsigned int reserved0:8;
}stpm32_dsp_reg6_t;
#pragma pack(pop)

// DSP_REG7
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg7 {
    unsigned int c1_fund:24;
    unsigned int reserved0:8;
}stpm32_dsp_reg7_t;
#pragma pack(pop)

// DSP_REG8
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg8 {
    unsigned int v2_fund:24;
    unsigned int reserved0:8;
}stpm32_dsp_reg8_t;
#pragma pack(pop)

// DSP_REG9
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg9 {
    unsigned int c2_fund:24;
    unsigned int reserved0:8;
}stpm32_dsp_reg9_t;
#pragma pack(pop)

// DSP_REG14
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg14 {
    unsigned int v1_rms_data:16;
    unsigned int c1_rms_data:16;
}stpm32_dsp_reg14_t;
#pragma pack(pop)

// DSP_REG15
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg15 {
    unsigned int v2_rms_data:16;
    unsigned int c2_rms_data:16;
}stpm32_dsp_reg15_t;
#pragma pack(pop)

// DSP_REG16
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg16 {
    unsigned int swv1_time:15;
    unsigned int reserved0:1;
    unsigned int sag1_time:15;
    unsigned int reserved1:1;
}stpm32_dsp_reg16_t;
#pragma pack(pop)

// DSP_REG17
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg17 {
    unsigned int swc1_time:15;
    unsigned int reserved0:1;
    unsigned int c1_pha:12;
    unsigned int reserved1:4;
}stpm32_dsp_reg17_t;
#pragma pack(pop)

// DSP_REG18
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg18 {
    unsigned int swv2_time:15;
    unsigned int reserved0:1;
    unsigned int sag2_time:15;
    unsigned int reserved1:1;
}stpm32_dsp_reg18_t;
#pragma pack(pop)

// DSP_REG19
#pragma pack(push, 1)
typedef struct _Stpm32_Dsp_Reg19 {
    unsigned int swc2_time:15;
    unsigned int reserved0:1;
    unsigned int c2_pha:12;
    unsigned int reserved1:4;
}stpm32_dsp_reg19_t;
#pragma pack(pop)

// PH1_REG1
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg1 {
    unsigned int ph1_active_energy:32;
}stpm32_ph1_reg1_t;
#pragma pack(pop)

// PH1_REG2
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg2 {
    unsigned int ph1_fundamental_energy:32;
}stpm32_ph1_reg2_t;
#pragma pack(pop)

// PH1_REG3
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg3 {
    unsigned int ph1_reactive_energy:32;
}stpm32_ph1_reg3_t;
#pragma pack(pop)

// PH1_REG4
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg4 {
    unsigned int ph1_apparent_energy:32;
}stpm32_ph1_reg4_t;
#pragma pack(pop)

// PH1_REG5
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg5 {
    unsigned int ph1_active_power:28;
    unsigned int reserved0:4;
}stpm32_ph1_reg5_t;
#pragma pack(pop)

// PH1_REG6
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg6 {
    unsigned int ph1_fundamental_power:28;
    unsigned int reserved0:4;
}stpm32_ph1_reg6_t;
#pragma pack(pop)

// PH1_REG7
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg7 {
    unsigned int ph1_reactive_power:28;
    unsigned int reserved0:4;
}stpm32_ph1_reg7_t;
#pragma pack(pop)

// PH1_REG8
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg8 {
    unsigned int ph1_apparent_rms_power:28;
    unsigned int reserved0:4;
}stpm32_ph1_reg8_t;
#pragma pack(pop)

// PH1_REG9
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg9 {
    unsigned int ph1_apparent_vectorial_power:28;
    unsigned int reserved0:4;
}stpm32_ph1_reg9_t;
#pragma pack(pop)

// PH1_REG10
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg10 {
    unsigned int ph1_momentary_active_power:28;
    unsigned int reserved0:4;
}stpm32_ph1_reg10_t;
#pragma pack(pop)

// PH1_REG11
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg11 {
    unsigned int ph1_momentary_fundamental_power:28;
    unsigned int reserved0:4;
}stpm32_ph1_reg11_t;
#pragma pack(pop)

// PH1_REG12
#pragma pack(push, 1)
typedef struct _Stpm32_Ph1_Reg12 {
    unsigned int ph1_ah_acc:32;
}stpm32_ph1_reg12_t;
#pragma pack(pop)

// PH2_REG1
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg1 {
    unsigned int ph2_active_energy:32;
}stpm32_ph2_reg1_t;
#pragma pack(pop)

// PH2_REG2
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg2 {
    unsigned int ph2_fundamental_energy:32;
}stpm32_ph2_reg2_t;
#pragma pack(pop)

// PH2_REG3
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg3 {
    unsigned int ph2_reactive_energy:32;
}stpm32_ph2_reg3_t;
#pragma pack(pop)

// PH2_REG4
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg4 {
    unsigned int ph2_apparent_energy:32;
}stpm32_ph2_reg4_t;
#pragma pack(pop)

// PH2_REG5
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg5 {
    unsigned int ph2_active_power:28;
    unsigned int reserved0:4;
}stpm32_ph2_reg5_t;
#pragma pack(pop)

// PH2_REG6
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg6 {
    unsigned int ph2_fundamental_power:28;
    unsigned int reserved0:4;
}stpm32_ph2_reg6_t;
#pragma pack(pop)

// PH2_REG7
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg7 {
    unsigned int ph2_reactive_power:28;
    unsigned int reserved0:4;
}stpm32_ph2_reg7_t;
#pragma pack(pop)

// PH2_REG8
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg8 {
    unsigned int ph2_apparent_rms_power:28;
    unsigned int reserved0:4;
}stpm32_ph2_reg8_t;
#pragma pack(pop)

// PH2_REG9
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg9 {
    unsigned int ph2_apparent_vectorial_power:28;
    unsigned int reserved0:4;
}stpm32_ph2_reg9_t;
#pragma pack(pop)

// PH2_REG10
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg10 {
    unsigned int ph2_momentary_active_power:28;
    unsigned int reserved0:4;
}stpm32_ph2_reg10_t;
#pragma pack(pop)

// PH2_REG11
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg11 {
    unsigned int ph2_momentary_fundamental_power:28;
    unsigned int reserved0:4;
}stpm32_ph2_reg11_t;
#pragma pack(pop)

// PH2_REG12
#pragma pack(push, 1)
typedef struct _Stpm32_Ph2_Reg12 {
    unsigned int ph2_ah_acc:32;
}stpm32_ph2_reg12_t;
#pragma pack(pop)

// TOT_REG1
#pragma pack(push, 1)
typedef struct _Stpm32_Tot_Reg1 {
    unsigned int total_active_energy:32;
}stpm32_tot_reg1_t;
#pragma pack(pop)

// TOT_REG2
#pragma pack(push, 1)
typedef struct _Stpm32_Tot_Reg2 {
    unsigned int total_fundamental_energy:32;
}stpm32_tot_reg2_t;
#pragma pack(pop)

// TOT_REG3
#pragma pack(push, 1)
typedef struct _Stpm32_Tot_Reg3 {
    unsigned int total_reactive_energy:32;
}stpm32_tot_reg3_t;
#pragma pack(pop)

// TOT_REG4
#pragma pack(push, 1)
typedef struct _Stpm32_Tot_Reg4 {
    unsigned int total_apparent_energy:32;
}stpm32_tot_reg4_t;
#pragma pack(pop)

/*
// 
#pragma pack(push, 1)
typedef struct _ {
}_t;
#pragma pack(pop)
*/

/**
 * Перевод значения регистра в целое число.
 * @param reg Регистр.
 */
#define STPM3X_REG_TO_U32(reg) (*((union {void* p; uint32_t* pu;})(void*)&reg).pu)

#define STPM3X_FIELD_GET_INT(I, BITS) ((I & (1 << (BITS - 1))) ? (I | ((-1) << BITS)) : (I))

/**
 * Сбрасывает регистр DSP_CR1
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr1(stpm3x_t* stpm3x, stpm32_dsp_cr1_t* reg);

/**
 * Считывает регистр DSP_CR1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr1(stpm3x_t* stpm3x, stpm32_dsp_cr1_t* reg);

/**
 * Записывает регистр DSP_CR1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr1(stpm3x_t* stpm3x, stpm32_dsp_cr1_t* reg);


/**
 * Сбрасывает регистр DSP_CR2
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr2(stpm3x_t* stpm3x, stpm32_dsp_cr2_t* reg);

/**
 * Считывает регистр DSP_CR2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr2(stpm3x_t* stpm3x, stpm32_dsp_cr2_t* reg);

/**
 * Записывает регистр DSP_CR2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr2(stpm3x_t* stpm3x, stpm32_dsp_cr2_t* reg);


/**
 * Сбрасывает регистр DSP_CR3
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr3(stpm3x_t* stpm3x, stpm32_dsp_cr3_t* reg);

/**
 * Считывает регистр DSP_CR3.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr3(stpm3x_t* stpm3x, stpm32_dsp_cr3_t* reg);

/**
 * Записывает регистр DSP_CR3.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr3(stpm3x_t* stpm3x, stpm32_dsp_cr3_t* reg);


/**
 * Сбрасывает регистр DSP_CR4
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr4(stpm3x_t* stpm3x, stpm32_dsp_cr4_t* reg);

/**
 * Считывает регистр DSP_CR4.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr4(stpm3x_t* stpm3x, stpm32_dsp_cr4_t* reg);

/**
 * Записывает регистр DSP_CR4.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr4(stpm3x_t* stpm3x, stpm32_dsp_cr4_t* reg);


/**
 * Сбрасывает регистр DSP_CR5
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr5(stpm3x_t* stpm3x, stpm32_dsp_cr5_t* reg);

/**
 * Считывает регистр DSP_CR5.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr5(stpm3x_t* stpm3x, stpm32_dsp_cr5_t* reg);

/**
 * Записывает регистр DSP_CR5.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr5(stpm3x_t* stpm3x, stpm32_dsp_cr5_t* reg);


/**
 * Сбрасывает регистр DSP_CR6
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr6(stpm3x_t* stpm3x, stpm32_dsp_cr6_t* reg);

/**
 * Считывает регистр DSP_CR6.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr6(stpm3x_t* stpm3x, stpm32_dsp_cr6_t* reg);

/**
 * Записывает регистр DSP_CR6.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr6(stpm3x_t* stpm3x, stpm32_dsp_cr6_t* reg);


/**
 * Сбрасывает регистр DSP_CR7
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr7(stpm3x_t* stpm3x, stpm32_dsp_cr7_t* reg);

/**
 * Считывает регистр DSP_CR7.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr7(stpm3x_t* stpm3x, stpm32_dsp_cr7_t* reg);

/**
 * Записывает регистр DSP_CR7.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr7(stpm3x_t* stpm3x, stpm32_dsp_cr7_t* reg);


/**
 * Сбрасывает регистр DSP_CR8
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr8(stpm3x_t* stpm3x, stpm32_dsp_cr8_t* reg);

/**
 * Считывает регистр DSP_CR8.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr8(stpm3x_t* stpm3x, stpm32_dsp_cr8_t* reg);

/**
 * Записывает регистр DSP_CR8.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr8(stpm3x_t* stpm3x, stpm32_dsp_cr8_t* reg);


/**
 * Сбрасывает регистр DSP_CR9
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr9(stpm3x_t* stpm3x, stpm32_dsp_cr9_t* reg);

/**
 * Считывает регистр DSP_CR9.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr9(stpm3x_t* stpm3x, stpm32_dsp_cr9_t* reg);

/**
 * Записывает регистр DSP_CR9.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr9(stpm3x_t* stpm3x, stpm32_dsp_cr9_t* reg);


/**
 * Сбрасывает регистр DSP_CR10
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr10(stpm3x_t* stpm3x, stpm32_dsp_cr10_t* reg);

/**
 * Считывает регистр DSP_CR10.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr10(stpm3x_t* stpm3x, stpm32_dsp_cr10_t* reg);

/**
 * Записывает регистр DSP_CR10.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr10(stpm3x_t* stpm3x, stpm32_dsp_cr10_t* reg);


/**
 * Сбрасывает регистр DSP_CR11
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr11(stpm3x_t* stpm3x, stpm32_dsp_cr11_t* reg);

/**
 * Считывает регистр DSP_CR11.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr11(stpm3x_t* stpm3x, stpm32_dsp_cr11_t* reg);

/**
 * Записывает регистр DSP_CR11.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr11(stpm3x_t* stpm3x, stpm32_dsp_cr11_t* reg);


/**
 * Сбрасывает регистр DSP_CR12
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_cr12(stpm3x_t* stpm3x, stpm32_dsp_cr12_t* reg);

/**
 * Считывает регистр DSP_CR12.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_cr12(stpm3x_t* stpm3x, stpm32_dsp_cr12_t* reg);

/**
 * Записывает регистр DSP_CR12.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_cr12(stpm3x_t* stpm3x, stpm32_dsp_cr12_t* reg);


/**
 * Сбрасывает регистр DFE_CR1
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dfe_cr1(stpm3x_t* stpm3x, stpm32_dfe_cr1_t* reg);

/**
 * Считывает регистр DFE_CR1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dfe_cr1(stpm3x_t* stpm3x, stpm32_dfe_cr1_t* reg);

/**
 * Записывает регистр DFE_CR1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dfe_cr1(stpm3x_t* stpm3x, stpm32_dfe_cr1_t* reg);


/**
 * Сбрасывает регистр DFE_CR2
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dfe_cr2(stpm3x_t* stpm3x, stpm32_dfe_cr2_t* reg);

/**
 * Считывает регистр DFE_CR2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dfe_cr2(stpm3x_t* stpm3x, stpm32_dfe_cr2_t* reg);

/**
 * Записывает регистр DFE_CR2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dfe_cr2(stpm3x_t* stpm3x, stpm32_dfe_cr2_t* reg);


/**
 * Сбрасывает регистр DSP_IRQ1
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_irq1(stpm3x_t* stpm3x, stpm32_dsp_irq1_t* reg);

/**
 * Считывает регистр DSP_IRQ1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_irq1(stpm3x_t* stpm3x, stpm32_dsp_irq1_t* reg);

/**
 * Записывает регистр DSP_IRQ1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_irq1(stpm3x_t* stpm3x, stpm32_dsp_irq1_t* reg);


/**
 * Сбрасывает регистр DSP_IRQ2
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_irq2(stpm3x_t* stpm3x, stpm32_dsp_irq2_t* reg);

/**
 * Считывает регистр DSP_IRQ2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_irq2(stpm3x_t* stpm3x, stpm32_dsp_irq2_t* reg);

/**
 * Записывает регистр DSP_IRQ2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_irq2(stpm3x_t* stpm3x, stpm32_dsp_irq2_t* reg);


/**
 * Сбрасывает регистр DSP_SR1
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_sr1(stpm3x_t* stpm3x, stpm32_dsp_sr1_t* reg);

/**
 * Считывает регистр DSP_SR1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_sr1(stpm3x_t* stpm3x, stpm32_dsp_sr1_t* reg);

/**
 * Записывает регистр DSP_SR1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_sr1(stpm3x_t* stpm3x, stpm32_dsp_sr1_t* reg);


/**
 * Сбрасывает регистр DSP_SR2
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_dsp_sr2(stpm3x_t* stpm3x, stpm32_dsp_sr2_t* reg);

/**
 * Считывает регистр DSP_SR2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_sr2(stpm3x_t* stpm3x, stpm32_dsp_sr2_t* reg);

/**
 * Записывает регистр DSP_SR2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_dsp_sr2(stpm3x_t* stpm3x, stpm32_dsp_sr2_t* reg);


/**
 * Сбрасывает регистр US_REG1
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_us_reg1(stpm3x_t* stpm3x, stpm32_us_reg1_t* reg);

/**
 * Считывает регистр US_REG1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_us_reg1(stpm3x_t* stpm3x, stpm32_us_reg1_t* reg);

/**
 * Записывает регистр US_REG1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_us_reg1(stpm3x_t* stpm3x, stpm32_us_reg1_t* reg);


/**
 * Сбрасывает регистр US_REG2
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_us_reg2(stpm3x_t* stpm3x, stpm32_us_reg2_t* reg);

/**
 * Считывает регистр US_REG2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_us_reg2(stpm3x_t* stpm3x, stpm32_us_reg2_t* reg);

/**
 * Записывает регистр US_REG2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_us_reg2(stpm3x_t* stpm3x, stpm32_us_reg2_t* reg);


/**
 * Сбрасывает регистр US_REG3
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN void stpm3x_reset_us_reg3(stpm3x_t* stpm3x, stpm32_us_reg3_t* reg);

/**
 * Считывает регистр US_REG3.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_us_reg3(stpm3x_t* stpm3x, stpm32_us_reg3_t* reg);

/**
 * Записывает регистр US_REG3.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_write_us_reg3(stpm3x_t* stpm3x, stpm32_us_reg3_t* reg);


/**
 * Считывает регистр DSP_EV1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_ev1(stpm3x_t* stpm3x, stpm32_dsp_ev1_t* reg);


/**
 * Считывает регистр DSP_EV2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_ev2(stpm3x_t* stpm3x, stpm32_dsp_ev2_t* reg);


/**
 * Считывает регистр DSP_REG1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg1(stpm3x_t* stpm3x, stpm32_dsp_reg1_t* reg);


/**
 * Считывает регистр DSP_REG2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg2(stpm3x_t* stpm3x, stpm32_dsp_reg2_t* reg);


/**
 * Считывает регистр DSP_REG3.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg3(stpm3x_t* stpm3x, stpm32_dsp_reg3_t* reg);


/**
 * Считывает регистр DSP_REG4.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg4(stpm3x_t* stpm3x, stpm32_dsp_reg4_t* reg);


/**
 * Считывает регистр DSP_REG5.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg5(stpm3x_t* stpm3x, stpm32_dsp_reg5_t* reg);


/**
 * Считывает регистр DSP_REG6.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg6(stpm3x_t* stpm3x, stpm32_dsp_reg6_t* reg);


/**
 * Считывает регистр DSP_REG7.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg7(stpm3x_t* stpm3x, stpm32_dsp_reg7_t* reg);


/**
 * Считывает регистр DSP_REG8.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg8(stpm3x_t* stpm3x, stpm32_dsp_reg8_t* reg);


/**
 * Считывает регистр DSP_REG9.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg9(stpm3x_t* stpm3x, stpm32_dsp_reg9_t* reg);


/**
 * Считывает регистр DSP_REG14.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg14(stpm3x_t* stpm3x, stpm32_dsp_reg14_t* reg);


/**
 * Считывает регистр DSP_REG15.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg15(stpm3x_t* stpm3x, stpm32_dsp_reg15_t* reg);


/**
 * Считывает регистр DSP_REG16.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg16(stpm3x_t* stpm3x, stpm32_dsp_reg16_t* reg);


/**
 * Считывает регистр DSP_REG17.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg17(stpm3x_t* stpm3x, stpm32_dsp_reg17_t* reg);


/**
 * Считывает регистр DSP_REG18.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg18(stpm3x_t* stpm3x, stpm32_dsp_reg18_t* reg);


/**
 * Считывает регистр DSP_REG19.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_dsp_reg19(stpm3x_t* stpm3x, stpm32_dsp_reg19_t* reg);


/**
 * Считывает регистр PH1_REG1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg1(stpm3x_t* stpm3x, stpm32_ph1_reg1_t* reg);


/**
 * Считывает регистр PH1_REG2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg2(stpm3x_t* stpm3x, stpm32_ph1_reg2_t* reg);


/**
 * Считывает регистр PH1_REG3.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg3(stpm3x_t* stpm3x, stpm32_ph1_reg3_t* reg);


/**
 * Считывает регистр PH1_REG4.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg4(stpm3x_t* stpm3x, stpm32_ph1_reg4_t* reg);


/**
 * Считывает регистр PH1_REG5.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg5(stpm3x_t* stpm3x, stpm32_ph1_reg5_t* reg);


/**
 * Считывает регистр PH1_REG6.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg6(stpm3x_t* stpm3x, stpm32_ph1_reg6_t* reg);


/**
 * Считывает регистр PH1_REG7.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg7(stpm3x_t* stpm3x, stpm32_ph1_reg7_t* reg);


/**
 * Считывает регистр PH1_REG8.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg8(stpm3x_t* stpm3x, stpm32_ph1_reg8_t* reg);


/**
 * Считывает регистр PH1_REG9.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg9(stpm3x_t* stpm3x, stpm32_ph1_reg9_t* reg);


/**
 * Считывает регистр PH1_REG10.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg10(stpm3x_t* stpm3x, stpm32_ph1_reg10_t* reg);


/**
 * Считывает регистр PH1_REG11.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg11(stpm3x_t* stpm3x, stpm32_ph1_reg11_t* reg);


/**
 * Считывает регистр PH1_REG12.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph1_reg12(stpm3x_t* stpm3x, stpm32_ph1_reg12_t* reg);


/**
 * Считывает регистр PH2_REG1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg1(stpm3x_t* stpm3x, stpm32_ph2_reg1_t* reg);


/**
 * Считывает регистр PH2_REG2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg2(stpm3x_t* stpm3x, stpm32_ph2_reg2_t* reg);


/**
 * Считывает регистр PH2_REG3.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg3(stpm3x_t* stpm3x, stpm32_ph2_reg3_t* reg);


/**
 * Считывает регистр PH2_REG4.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg4(stpm3x_t* stpm3x, stpm32_ph2_reg4_t* reg);


/**
 * Считывает регистр PH2_REG5.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg5(stpm3x_t* stpm3x, stpm32_ph2_reg5_t* reg);


/**
 * Считывает регистр PH2_REG6.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg6(stpm3x_t* stpm3x, stpm32_ph2_reg6_t* reg);


/**
 * Считывает регистр PH2_REG7.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg7(stpm3x_t* stpm3x, stpm32_ph2_reg7_t* reg);


/**
 * Считывает регистр PH2_REG8.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg8(stpm3x_t* stpm3x, stpm32_ph2_reg8_t* reg);


/**
 * Считывает регистр PH2_REG9.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg9(stpm3x_t* stpm3x, stpm32_ph2_reg9_t* reg);


/**
 * Считывает регистр PH2_REG10.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg10(stpm3x_t* stpm3x, stpm32_ph2_reg10_t* reg);


/**
 * Считывает регистр PH2_REG11.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg11(stpm3x_t* stpm3x, stpm32_ph2_reg11_t* reg);


/**
 * Считывает регистр PH2_REG12.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_ph2_reg12(stpm3x_t* stpm3x, stpm32_ph2_reg12_t* reg);


/**
 * Считывает регистр TOT_REG1.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_tot_reg1(stpm3x_t* stpm3x, stpm32_tot_reg1_t* reg);


/**
 * Считывает регистр TOT_REG2.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_tot_reg2(stpm3x_t* stpm3x, stpm32_tot_reg2_t* reg);


/**
 * Считывает регистр TOT_REG3.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_tot_reg3(stpm3x_t* stpm3x, stpm32_tot_reg3_t* reg);


/**
 * Считывает регистр TOT_REG4.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
EXTERN err_t stpm3x_read_tot_reg4(stpm3x_t* stpm3x, stpm32_tot_reg4_t* reg);


/**
 * Считывает регистр REGNAME.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
/*EXTERN err_t stpm3x_read_regname(stpm3x_t* stpm3x, regtype* reg);


err_t stpm3x_read_regname(stpm3x_t* stpm3x, regtype* reg)
{
    return stpm3x_read_register(stpm3x, STPM3X_REGNAME_ADDR, (uint32_t*)reg);
}*/



/**
 * Сбрасывает регистр REGNAME
 * в значение по-умолчанию.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
//EXTERN void stpm3x_reset_regname(stpm3x_t* stpm3x, regtype* reg);

/**
 * Считывает регистр REGNAME.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
//EXTERN err_t stpm3x_read_regname(stpm3x_t* stpm3x, regtype* reg);

/**
 * Записывает регистр REGNAME.
 * Синхронная операция.
 * @param stpm3x Измеритель.
 * @param reg Регистр.
 * @return Код ошибки.
 */
//EXTERN err_t stpm3x_write_regname(stpm3x_t* stpm3x, regtype* reg);


#endif	// STPM3X_H

