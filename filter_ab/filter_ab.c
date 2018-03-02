#include "filter_ab.h"



void filter_ab_init(filter_ab_t* filter)
{
    filter->value = 0;
    filter->weight = 0;
}

void filter_ab_reset(filter_ab_t* filter)
{
    filter->value = 0;
}

void filter_ab_set_weight(filter_ab_t* filter, filter_ab_weight_t weight)
{
    filter->weight = weight;
}

void filter_ab_put(filter_ab_t* filter, filter_ab_value_t value)
{
    filter_ab_value_t delta = filter->value - value;
    delta = ((int64_t)delta * (int32_t)((uint32_t)filter->weight)) >> FILTER_AB_WEIGHT_BITS;
    filter->value = value + delta;
}

filter_ab_value_t filter_ab_value(filter_ab_t* filter)
{
    return filter->value;
}





