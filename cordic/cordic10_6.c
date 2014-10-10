#include "cordic10_6.h"
#include <stddef.h>
#include <stdbool.h>

#define CORDIC10_6_TAN_TABLE_LEN 14
static const fixed10_6_t cordic10_6_table[CORDIC10_6_TAN_TABLE_LEN] = {
        2880, 1700, 898, 456, 229, 115, 57, 29, 14, 7, 4, 2, 1, 0
};
#define CORDIC10_6_GAIN 39 //fixed10_6_t(0.607253)

void cordic10_6_atan2_hyp(fixed10_6_t x, fixed10_6_t y, fixed10_6_t* angle, fixed10_6_t* hyp)
{
    if(angle == NULL && hyp == NULL) return;
    
    fixed10_6_t sum_angle = 0;
    uint8_t loop_n = 0;
    
    fixed10_6_t new_x, new_y;
    
    if(x < 0){
        x = -x;
        y = -y;
        sum_angle = CORDIC10_6_ANGLE_180;
    }else if(y < 0){
        sum_angle = CORDIC10_6_ANGLE_360;
    }
    
    while((y != 0) && (loop_n < CORDIC10_6_TAN_TABLE_LEN)){
        if(y > 0){
            new_x = x + (y >> loop_n);
            new_y = y - (x >> loop_n);
            sum_angle += cordic10_6_table[loop_n];
        }else{
            new_x = x - (y >> loop_n);
            new_y = y + (x >> loop_n);
            sum_angle -= cordic10_6_table[loop_n];
        }
        loop_n ++;
        x = new_x;
        y = new_y;
    }
    
    if(angle) { *angle = sum_angle; }
    if(hyp)   { *hyp = x / 8 * CORDIC10_6_GAIN / 8; }
}

void cordic10_6_sincos(fixed10_6_t angle, fixed10_6_t* sin, fixed10_6_t* cos)
{
    if(sin == NULL && cos == NULL) return;
    
    fixed10_6_t sum_angle = 0;
    uint8_t loop_n = 0;
    
    fixed10_6_t x = CORDIC10_6_GAIN;
    fixed10_6_t y = 0;
    
    fixed10_6_t new_x, new_y;
    
    if(angle >= CORDIC10_6_ANGLE_360) angle -= CORDIC10_6_ANGLE_360;
    else if(angle <= -CORDIC10_6_ANGLE_360) angle += CORDIC10_6_ANGLE_360;
    
    if(angle < 0) angle = CORDIC10_6_ANGLE_360 + angle;
    
    if(angle > CORDIC10_6_ANGLE_270) sum_angle = CORDIC10_6_ANGLE_360;
    else if(angle > CORDIC10_6_ANGLE_90) sum_angle = CORDIC10_6_ANGLE_180;
    
    while(loop_n < CORDIC10_6_TAN_TABLE_LEN){
        if(sum_angle < angle){
            new_x = x - (y >> loop_n);
            new_y = y + (x >> loop_n);
            sum_angle += cordic10_6_table[loop_n];
        }else{
            new_x = x + (y >> loop_n);
            new_y = y - (x >> loop_n);
            sum_angle -= cordic10_6_table[loop_n];
        }
        loop_n ++;
        x = new_x;
        y = new_y;
    }
    
    if(angle > CORDIC10_6_ANGLE_90 && angle < CORDIC10_6_ANGLE_270){ x = -x; y = -y; }
    
    if(sin) { *sin = y; }
    if(cos) { *cos = x; }
}
