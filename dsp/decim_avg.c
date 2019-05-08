#include "decim_avg.h"


err_t decim_avg_init(decim_avg_t* decim, size_t scale)
{
    if(scale == 0) return E_INVALID_VALUE;

    decim->scale = scale;
    decim->index = 0;
    decim->data = 0;
    decim->sum = 0;

    return E_NO_ERROR;
}

void decim_avg_reset(decim_avg_t* decim)
{
    decim->sum = 0;
    decim->data = 0;
    decim->index = 0;
}

void decim_avg_put(decim_avg_t* decim, decim_avg_data_t data)
{
    size_t next = decim->index + 1;

    decim->sum = iq15_add(decim->sum, data);

    if(next >= decim->scale){
        if(decim->scale > 1){
            decim->data = iq15_idiv(decim->sum, (int32_t)decim->scale);
        }else if(decim->scale == 1){
            decim->data = decim->sum;
        }else{
            decim->data = 0;
        }
        decim->sum = 0;
        next = 0;
    }

    decim->index = next;
}
