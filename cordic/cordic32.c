#include "cordic32.h"
#include <stddef.h>
#include <stdbool.h>

#define CORDIC32_TAN_TABLE_LEN 24
static const fixed32_t cordic32_table[CORDIC32_TAN_TABLE_LEN] = {
    2949120, 1740967, 919879, 466945, 234379, 117304, 58666, 29335, 14668, 
    7334, 3667, 1833, 917, 458, 229, 115, 57, 29, 14, 7, 4, 2, 1, 0
};
#define CORDIC32_GAIN 39797 //fixed32_t(0.607253)

void cordic32_atan2_hyp(fixed32_t x, fixed32_t y, fixed32_t* angle, fixed32_t* hyp)
{
    if(angle == NULL && hyp == NULL) return;
    
    fixed32_t sum_angle = 0;
    uint8_t loop_n = 0;
    
    fixed32_t new_x, new_y;
    
    if(x < 0){
        x = -x;
        y = -y;
        sum_angle = CORDIC32_ANGLE_180;
    }else if(y < 0){
        sum_angle = CORDIC32_ANGLE_360;
    }
    
    while((y != 0) && (loop_n < CORDIC32_TAN_TABLE_LEN)){
        if(y > 0){
            new_x = x + (y >> loop_n);
            new_y = y - (x >> loop_n);
            sum_angle += cordic32_table[loop_n];
        }else{
            new_x = x - (y >> loop_n);
            new_y = y + (x >> loop_n);
            sum_angle -= cordic32_table[loop_n];
        }
        loop_n ++;
        x = new_x;
        y = new_y;
    }
    
    if(angle) { *angle = sum_angle; }
    if(hyp)   { *hyp = x / 256 * CORDIC32_GAIN / 256; }
}

void cordic32_sincos(fixed32_t angle, fixed32_t* sin, fixed32_t* cos)
{
    if(sin == NULL && cos == NULL) return;
    
    fixed32_t sum_angle = 0;
    uint8_t loop_n = 0;
    
    fixed32_t x = CORDIC32_GAIN;
    fixed32_t y = 0;
    
    fixed32_t new_x, new_y;
    
    if(angle >= CORDIC32_ANGLE_360) angle -= CORDIC32_ANGLE_360;
    else if(angle <= -CORDIC32_ANGLE_360) angle += CORDIC32_ANGLE_360;
    
    if(angle < 0) angle = CORDIC32_ANGLE_360 + angle;
    
    if(angle > CORDIC32_ANGLE_270) sum_angle = CORDIC32_ANGLE_360;
    else if(angle > CORDIC32_ANGLE_90) sum_angle = CORDIC32_ANGLE_180;
    
    while(loop_n < CORDIC32_TAN_TABLE_LEN){
        if(sum_angle < angle){
            new_x = x - (y >> loop_n);
            new_y = y + (x >> loop_n);
            sum_angle += cordic32_table[loop_n];
        }else{
            new_x = x + (y >> loop_n);
            new_y = y - (x >> loop_n);
            sum_angle -= cordic32_table[loop_n];
        }
        loop_n ++;
        x = new_x;
        y = new_y;
    }
    
    if(angle > CORDIC32_ANGLE_90 && angle < CORDIC32_ANGLE_270){ x = -x; y = -y; }
    
    if(sin) { *sin = y; }
    if(cos) { *cos = x; }
}
