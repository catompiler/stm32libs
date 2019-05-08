#include "avg.h"


err_t avg_init(avg_t* avg)
{
    avg->count = 0;
    avg->data = 0;
    avg->sum = 0;

    return E_NO_ERROR;
}

void avg_reset(avg_t* avg)
{
    avg->sum = 0;
    avg->data = 0;
    avg->count = 0;
}

void avg_put(avg_t* avg, avg_data_t data)
{
    avg->sum = iq15_add(avg->sum, data);
    avg->count ++;
}

avg_data_t avg_calc(avg_t* avg)
{
    if(avg->count > 1){
        avg->data = iq15_idiv(avg->sum, (int32_t)avg->count);
    }else if(avg->count == 1){
        avg->data = avg->sum;
    }else{
        avg->data = 0;
    }
    avg->sum = 0;
    avg->count = 0;
    
    return avg->data;
}
