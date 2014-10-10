#include "mutex.h"
#include <stm32f10x.h>

void mutex_init(mutex_t* mutex)
{
    *mutex = MUTEX_UNLOCKED;
}

bool mutex_trylock(mutex_t* mutex)
{
    if(__LDREXB(mutex) == MUTEX_LOCKED) return false;
    if(__STREXB(MUTEX_LOCKED, mutex) == 1) return false;
    __DMB();
    return true;
}

void mutex_lock(mutex_t* mutex)
{
    for(;;){
        if(__LDREXB(mutex) == MUTEX_LOCKED) continue;
        if(__STREXB(MUTEX_LOCKED, mutex) == 0) break;
    }
    __DMB();
}

void mutex_unlock(mutex_t* mutex)
{
    __DMB();
    *mutex = MUTEX_UNLOCKED;
}
