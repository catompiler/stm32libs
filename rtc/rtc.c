#include "rtc.h"
#include <errno.h>
#include "utils/utils.h"


//! Тип состояния часов реального времени.
typedef struct _Rtc_State {
    int tz_minuteswest; //!< Смещение отностительно UTC.
    rtc_second_callback_t second_callback; //!< Ежесекундный каллбэк.
    rtc_alarm_callback_t alarm_callback;  //!< Каллбэк сигнализации.
    rtc_get_usec_callback_t get_usec_callback; //!< Каллбэк получения микросекунд.
} rtc_state_t;

static rtc_state_t rtc;


err_t rtc_init(void)
{
    rtc.tz_minuteswest = 0;
    rtc.second_callback = NULL;
    rtc.alarm_callback = NULL;
    rtc.get_usec_callback = NULL;
    return E_NO_ERROR;
}

FunctionalState rtc_state(void)
{
    return (RCC->BDCR & RCC_BDCR_RTCEN) ? ENABLE : DISABLE;
}

/**
 * Ждёт завершения текущей операции запили в регистры RTC.
 */
ALWAYS_INLINE static void rtc_wait_current_op(void)
{
    WAIT_WHILE_FALSE(RTC->CRL & RTC_CRL_RTOFF);
}

/**
 * Входит в режим конфигурирования RTC.
 */
ALWAYS_INLINE static void rtc_enter_config_mode(void)
{
    RTC->CRL |= RTC_CRL_CNF;
}
/**
 * Выходит из режима конфигурирования RTC.
 */
ALWAYS_INLINE static void rtc_exit_config_mode(void)
{
    RTC->CRL &= ~RTC_CRL_CNF;
}

void rtc_setup(uint32_t prescaler, const time_t* time)
{
    uint16_t time_h = 0;
    uint16_t time_l = 0;
    uint16_t prescaler_h = prescaler >> 16;
    uint16_t prescaler_l = prescaler & 0xffff;
    
    if(time != NULL){
        time_h = (*time) >> 16;
        time_l = (*time) & 0xffff;
    }
    
    uint32_t pwr_dbp = PWR->CR & PWR_CR_DBP;
    if(!pwr_dbp) PWR->CR |= PWR_CR_DBP;
    
    rtc_wait_current_op();
    rtc_enter_config_mode();
    
    RTC->CNTH = time_h;
    RTC->CNTL = time_l;
    RTC->PRLH = prescaler_h;
    RTC->PRLL = prescaler_l;
    RTC->ALRH = 0;
    RTC->ALRL = 0;

    rtc_exit_config_mode();
    rtc_wait_current_op();
    
    if(!pwr_dbp) PWR->CR &= ~PWR_CR_DBP;
}

time_t rtc_time(time_t* time)
{
    time_t t = ((time_t)RTC->CNTH << 16) | RTC->CNTL;
    if(time) *time = t;
    return t;
}

void rtc_set_time(const time_t* time)
{
    uint16_t time_h = 0;
    uint16_t time_l = 0;
    
    if(time != NULL){
        time_h = (*time) >> 16;
        time_l = (*time) & 0xffff;
    }
    
    uint32_t pwr_dbp = PWR->CR & PWR_CR_DBP;
    if(!pwr_dbp) PWR->CR |= PWR_CR_DBP;
    
    rtc_wait_current_op();
    rtc_enter_config_mode();
    
    RTC->CNTH = time_h;
    RTC->CNTL = time_l;

    rtc_exit_config_mode();
    rtc_wait_current_op();
    
    if(!pwr_dbp) PWR->CR &= ~PWR_CR_DBP;
}

time_t rtc_alarm(time_t* time)
{
    time_t t = ((time_t)RTC->ALRH << 16) | RTC->ALRL;
    if(time) *time = t;
    return t;
}

void rtc_set_alarm(const time_t* time)
{
    uint16_t time_h = 0;
    uint16_t time_l = 0;
    
    if(time != NULL){
        time_h = (*time) >> 16;
        time_l = (*time) & 0xffff;
    }
    
    uint32_t pwr_dbp = PWR->CR & PWR_CR_DBP;
    if(!pwr_dbp) PWR->CR |= PWR_CR_DBP;
    
    rtc_wait_current_op();
    rtc_enter_config_mode();
    
    RTC->ALRH = time_h;
    RTC->ALRL = time_l;

    rtc_exit_config_mode();
    rtc_wait_current_op();
    
    if(!pwr_dbp) PWR->CR &= ~PWR_CR_DBP;
}

uint32_t rtc_prescaler(void)
{
    return ((uint32_t)RTC->DIVH << 16) | RTC->DIVL;
}

void rtc_set_prescaler(uint32_t prescaler)
{
    uint16_t prescaler_h = prescaler >> 16;
    uint16_t prescaler_l = prescaler & 0xffff;
    
    uint32_t pwr_dbp = PWR->CR & PWR_CR_DBP;
    if(!pwr_dbp) PWR->CR |= PWR_CR_DBP;
    
    rtc_wait_current_op();
    rtc_enter_config_mode();
    
    RTC->PRLH = prescaler_h;
    RTC->PRLL = prescaler_l;

    rtc_exit_config_mode();
    rtc_wait_current_op();
    
    if(!pwr_dbp) PWR->CR &= ~PWR_CR_DBP;
}

void rtc_synchronize(void)
{
    rtc_wait_current_op();
    RTC->CRL &= ~RTC_CRL_RSF;
    rtc_wait_current_op();
    WAIT_WHILE_FALSE(RTC->CRL & RTC_CRL_RSF);
}

void rtc_clear_sync(void)
{
    rtc_wait_current_op();
    RTC->CRL &= ~RTC_CRL_RSF;
    rtc_wait_current_op();
}

void rtc_wait_sync(void)
{
    WAIT_WHILE_FALSE(RTC->CRL & RTC_CRL_RSF);
}

bool rtc_synchronized(void)
{
    return (RTC->CRL & RTC_CRL_RSF) ? true : false;
}

void rtc_interrupt_handler(void)
{
    rtc_wait_current_op();
    if(RTC->CRL & RTC_CRL_OWF){
        RTC->CRL &= ~RTC_CRL_OWF;
    }
    if(RTC->CRL & RTC_CRL_SECF){
        RTC->CRL &= ~RTC_CRL_SECF;
        if(rtc.second_callback) rtc.second_callback();
    }
}

void rtc_alarm_interrupt_handler(void)
{
    rtc_wait_current_op();
    if(RTC->CRL & RTC_CRL_ALRF){
        RTC->CRL &= ~RTC_CRL_ALRF;
        if(rtc.alarm_callback) rtc.alarm_callback();
    }
}

rtc_second_callback_t rtc_second_callback(void)
{
    return rtc.second_callback;
}

void rtc_set_second_callback(rtc_second_callback_t callback)
{
    rtc_wait_current_op();
    if(callback){
        rtc.second_callback = callback;
        RTC->CRH |= RTC_CRH_SECIE;
    }else{
        RTC->CRH &= ~RTC_CRH_SECIE;
        rtc.second_callback = callback;
    }
}

rtc_alarm_callback_t rtc_alarm_callback(void)
{
    return rtc.alarm_callback;
}

void rtc_set_alarm_callback(rtc_alarm_callback_t callback)
{
    rtc.alarm_callback = callback;
}

void rtc_set_alarm_interrupt_enabled(bool enabled)
{
    rtc_wait_current_op();
    if(enabled){
        RTC->CRH |= RTC_CRH_ALRIE;
    }else{
        RTC->CRH &= ~RTC_CRH_ALRIE;
    }
}

rtc_get_usec_callback_t rtc_get_usec_callback(void)
{
    return rtc.get_usec_callback;
}

void rtc_set_get_usec_callback(rtc_get_usec_callback_t callback)
{
    rtc.get_usec_callback = callback;
}

#ifdef RTC_TIMEOFDAY

int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
    if(tv == NULL){
        errno = EFAULT;
        return -1;
    }
    
    tv->tv_sec = rtc_time(NULL);
    tv->tv_usec = rtc.get_usec_callback ? rtc.get_usec_callback() : 0;
    
    if(tz != NULL){
        tz->tz_minuteswest = rtc.tz_minuteswest;
        tz->tz_dsttime = DST_NONE;
    }
    
    return 0;
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    if(tv == NULL){
        errno = EFAULT;
        return -1;
    }
    
    if(tz != NULL){
        if(tz->tz_dsttime != DST_NONE){
            errno = EINVAL;
            return -1;
        }
        rtc.tz_minuteswest = tz->tz_minuteswest;
    }
    
    rtc_set_time(&tv->tv_sec);
    
    return 0;
}

#endif
