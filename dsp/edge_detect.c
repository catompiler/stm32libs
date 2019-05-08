#include "edge_detect.h"

//! Маска детектора фронта.
#define EDGE_DETECT_MASK 0x3



void edge_detect_init(edge_detect_t* edge_detect)
{
    edge_detect->data = EDGE_DETECT_LOW;
}

void edge_detect_reset(edge_detect_t* edge_detect)
{
    edge_detect->data = EDGE_DETECT_LOW;
}

void edge_detect_reset_value(edge_detect_t* edge_detect, bool value)
{
    edge_detect->data = value ? EDGE_DETECT_HIGH : EDGE_DETECT_LOW;
}

edge_detect_state_t edge_detect_put(edge_detect_t* edge_detect, bool value)
{
    uint32_t data = edge_detect->data;
    uint32_t bit = value & 0x1;

    data = ((data << 1) | bit);

    edge_detect->data = (uint8_t)(data & EDGE_DETECT_MASK);

    return (edge_detect_state_t)edge_detect->data;
}

edge_detect_state_t edge_detect_state(edge_detect_t* edge_detect)
{
    return (edge_detect_state_t)edge_detect->data;
}

