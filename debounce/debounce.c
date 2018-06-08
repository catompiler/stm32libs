#include "debounce.h"


void debounce_init(debounce_t* debounce)
{
    debounce->timeout = 0;
    debounce->time = 0;
    debounce->state = false;
    debounce->changed = false;
}

void debounce_reset(debounce_t* debounce)
{
    debounce->time = 0;
    debounce->state = false;
    debounce->changed = false;
}

void debounce_init_timeout(debounce_t* debounce, fixed32_t timeout)
{
    debounce->timeout = timeout;
    debounce->time = 0;
    debounce->state = false;
    debounce->changed = false;
}

void debounce_set_timeout(debounce_t* debounce, fixed32_t timeout)
{
    debounce->timeout = timeout;
}

bool debounce_process(debounce_t* debounce, bool state, fixed32_t dt)
{
    if(state != debounce->state){
        debounce->time += dt;
        if(debounce->time >= debounce->timeout){
            debounce->state = state;
            debounce->time = 0;
            debounce->changed = true;
        }
    }else{
        debounce->time = 0;
        debounce->changed = false;
    }

    return debounce->changed;
}

bool debounce_state_changed(debounce_t* debounce)
{
    return debounce->changed;
}

bool debounce_state_stable(debounce_t* debounce)
{
    return debounce->time == 0;
}

bool debounce_state(debounce_t* debounce)
{
    return debounce->state;
}
