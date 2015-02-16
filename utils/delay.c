#include "delay.h"


#ifndef INLINE_DELAY

void delay_cycles(uint32_t cycles_count)
{
#define _DELAY_CYCLES_GETTING_LATENCY (14 + 2) // 2 - вызов функции.
#define _DELAY_CYCLES_COUNT_PER_LOOP_MIN 5
    
    register uint32_t cycles_per_loop = _DELAY_CYCLES_COUNT_PER_LOOP_MIN;
    
    if(FLASH->ACR & FLASH_ACR_PRFTBE){
        cycles_per_loop += FLASH->ACR & FLASH_ACR_LATENCY;
    }
    
    if(cycles_count > _DELAY_CYCLES_GETTING_LATENCY){
        
        cycles_count -= _DELAY_CYCLES_GETTING_LATENCY;
        
        asm volatile(
            "2:\n\t"
            "cmp %1, %0\n\t"
            "bhs 1f\n\t"
            "add %1, %2\n\t"
            "b 2b\n\t"
            "1:"
            ::"r"(cycles_count), "r"(0), "r"(cycles_per_loop)
        );
    }
#undef _DELAY_CYCLES_COUNT_PER_LOOP_MIN
#undef _DELAY_CYCLES_GETTING_LATENCY
}


void delay_ns(uint32_t ns)
{
#define _DELAY_NS_CALCULATING_CYCLES (35 + 2) // 2 - вызов функции.
    
    register uint32_t cycles_to_delay = (SystemCoreClock / 1000000UL) * ns / 1000UL;
    
    if(cycles_to_delay > _DELAY_NS_CALCULATING_CYCLES){
        cycles_to_delay -= _DELAY_NS_CALCULATING_CYCLES;
        delay_cycles(cycles_to_delay);
    }
#undef _DELAY_NS_CALCULATING_CYCLES
}


void delay_us(uint32_t us)
{
#define _DELAY_US_CALCULATING_CYCLES (21 + 2) // 2 - вызов функции.
    
    register uint32_t cycles_to_delay = SystemCoreClock / 1000000UL * us;
    
    if(cycles_to_delay > _DELAY_US_CALCULATING_CYCLES){
        cycles_to_delay -= _DELAY_US_CALCULATING_CYCLES;
        delay_cycles(cycles_to_delay);
    }
#undef _DELAY_US_CALCULATING_CYCLES
}


void delay_ms(uint32_t ms)
{
#define _DELAY_MS_CALCULATING_CYCLES (20 + 2) // 2 - вызов функции.
    
    register uint32_t cycles_to_delay = SystemCoreClock / 1000UL * ms;
    
    if(cycles_to_delay > _DELAY_MS_CALCULATING_CYCLES){
        cycles_to_delay -= _DELAY_MS_CALCULATING_CYCLES;
        delay_cycles(cycles_to_delay);
    }
#undef _DELAY_MS_CALCULATING_CYCLES
}

#endif
