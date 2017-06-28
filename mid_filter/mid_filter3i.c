#include "mid_filter3i.h"
#include <string.h>



void mid_filter3i_init(mid_filter3i_t* filter)
{
    memset(filter, 0x0, sizeof(mid_filter3i_t));
}

void mid_filter3i_reset(mid_filter3i_t* filter)
{
    filter->index = 0;
    filter->count = 0;
}

bool mid_filter3i_full(mid_filter3i_t* filter)
{
    return filter->count == MID_FILTER3I_SIZE;
}

bool mid_filter3i_empty(mid_filter3i_t* filter)
{
    return filter->count == 0;
}

void mid_filter3i_put(mid_filter3i_t* filter, mid_filter3i_value_t value)
{
    filter->buffer[filter->index] = value;
    
    if(++ filter->index >= MID_FILTER3I_SIZE) filter->index = 0;
    if(filter->count < MID_FILTER3I_SIZE) filter->count ++;
}

mid_filter3i_value_t mid_filter3i_value(mid_filter3i_t* filter)
{
    if(filter->count == 0) return 0;
    
    mid_filter3i_value_t a = filter->buffer[0];
    
    if(filter->count == 1) return a;
    
    mid_filter3i_value_t b = filter->buffer[1];
    
    if(filter->count == 2) return (a + b) >> 1;
    
    mid_filter3i_value_t c = filter->buffer[2];
    
    if(a >= b){
        if(b >= c){
            return b;
        }else if(a >= c){
            return c;
        }else{
            return a;
        }
    }else{//b > a
        if(a >= c){
            return a;
        }else if(b >= c){
            return c;
        }else{
            return b;
        }
    }
}





