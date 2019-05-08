#include "decim.h"


err_t decim_init(decim_t* decim, size_t scale)
{
    if(scale == 0) return E_INVALID_VALUE;

    decim->scale = scale;
    decim->index = 0;
    decim->data = 0;

    return E_NO_ERROR;
}

void decim_reset(decim_t* decim)
{
    decim->data = 0;
    decim->index = 0;
}

void decim_put(decim_t* decim, decim_data_t data)
{
    size_t next = decim->index + 1;

    if(next >= decim->scale){
        decim->data = data;
        next = 0;
    }

    decim->index = next;
}
