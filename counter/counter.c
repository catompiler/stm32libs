#include "counter.h"



/*void counter_init(counter_t* counter)
{
    *counter = 0;
}

void counter_tick(counter_t* counter)
{
    (*counter) ++;
}*/

counter_diff_t counter_diff(counter_t* counter_next, counter_t* counter_prev)
{
    if(*counter_prev > *counter_next){
        return *counter_next - (counter_int_t)*counter_prev;
    }
    return *counter_next - *counter_prev;
}


typedef struct _Sys_Counter_State {
    counter_t counter;
    counter_t ticks_per_sec;
} system_counter_state_t;

static system_counter_state_t sys_counter_state;


void system_counter_init(counter_t ticks_per_sec)
{
    sys_counter_state.counter = 0;
    sys_counter_state.ticks_per_sec = ticks_per_sec;
}

void system_counter_tick()
{
    sys_counter_state.counter ++;
}

counter_diff_t system_counter_diff(counter_t* counter)
{
    return counter_diff(&sys_counter_state.counter, counter);
}

counter_t system_counter_ticks()
{
    return sys_counter_state.counter;
}

counter_t system_counter_ticks_per_sec()
{
    return sys_counter_state.ticks_per_sec;
}

void system_counter_set_ticks_per_sec(counter_t ticks_per_sec)
{
    sys_counter_state.ticks_per_sec = ticks_per_sec;
}
