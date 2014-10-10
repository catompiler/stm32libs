#include "cordic16.h"
#include <stddef.h>
#include <stdbool.h>

#define CORDIC16_TAN_TABLE_LEN 16
static const fixed16_t cordic16_table[CORDIC16_TAN_TABLE_LEN] = {
        11520, 6801, 3593, 1824, 916, 458, 229, 115, 57, 29, 14, 7, 4, 2, 1, 0
};
#define CORDIC16_GAIN 155 //fixed16_t(0.607253)

void cordic16_atan2_hyp(fixed16_t x, fixed16_t y, fixed16_t* angle, fixed16_t* hyp)
{
    if(angle == NULL && hyp == NULL) return;
    
    fixed16_t sum_angle = 0;
    uint8_t loop_n = 0;
    
    fixed16_t new_x, new_y;
    
    while((y != 0) && (loop_n < CORDIC16_TAN_TABLE_LEN)){
        if(y > 0){
            new_x = x + (y >> loop_n);
            new_y = y - (x >> loop_n);
            sum_angle += cordic16_table[loop_n];
        }else{
            new_x = x - (y >> loop_n);
            new_y = y + (x >> loop_n);
            sum_angle -= cordic16_table[loop_n];
        }
        loop_n ++;
        x = new_x;
        y = new_y;
    }
    
    if(angle) { *angle = sum_angle; }
    if(hyp)   { *hyp = x / 16 * CORDIC16_GAIN / 16; }
}

void cordic16_sincos(fixed16_t angle, fixed16_t* sin, fixed16_t* cos)
{
    if(sin == NULL && cos == NULL) return;
    
    fixed16_t sum_angle = 0;
    uint8_t loop_n = 0;
    
    fixed16_t x = CORDIC16_GAIN;
    fixed16_t y = 0;
    
    fixed16_t new_x, new_y;
    
    while(loop_n < CORDIC16_TAN_TABLE_LEN){
        if(sum_angle < angle){
            new_x = x - (y >> loop_n);
            new_y = y + (x >> loop_n);
            sum_angle += cordic16_table[loop_n];
        }else{
            new_x = x + (y >> loop_n);
            new_y = y - (x >> loop_n);
            sum_angle -= cordic16_table[loop_n];
        }
        loop_n ++;
        x = new_x;
        y = new_y;
    }
    
    if(sin) { *sin = y; }
    if(cos) { *cos = x; }
}
