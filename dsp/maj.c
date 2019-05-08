#include "maj.h"


err_t maj_init(maj_t* maj)
{
    maj->ones = 0;
    maj->zeros = 0;
    maj->value = false;

    return E_NO_ERROR;
}

void maj_reset(maj_t* maj)
{
    maj->ones = 0;
    maj->zeros = 0;
    maj->value = false;
}

void maj_put(maj_t* maj, bool value)
{
    if(value) maj->ones ++;
    else maj->zeros ++;
}

bool maj_calc(maj_t* maj)
{
    maj->value = maj->ones > maj->zeros;
    
    maj->ones = 0;
    maj->zeros = 0;
    
    return maj->value;
}
