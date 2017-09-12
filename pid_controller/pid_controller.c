#include <stdint.h>
#include "utils/utils.h"
#include "pid_controller.h"


void pid_controller_init(pid_controller_t* controller, fixed32_t kp, fixed32_t ki, fixed32_t kd)
{
    controller->kp = kp;
    controller->ki = ki;
    controller->kd = kd;
    controller->prev_e = 0;
    controller->prev_i = 0;
    controller->value = 0;
    controller->clamp_min = INT32_MIN;
    controller->clamp_max = INT32_MAX;
    controller->clamp_width = INT32_MAX;
}

void pid_controller_reset(pid_controller_t* controller)
{
    controller->prev_e = 0;
    controller->prev_i = 0;
    controller->value = 0;
}

bool pid_controller_calculate(pid_controller_t* controller, fixed32_t e, fixed32_t dt)
{
    if(dt == 0) return false;
    // P = Kp * e;
    fixed32_t p = ((int64_t)controller->kp * e) >> FIXED32_FRACT_BITS;
    
    // I = Ki * e(t) * dt + I(t-1);
    fixed32_t i = ((int64_t)controller->ki * e) >> FIXED32_FRACT_BITS;
              i = ((int64_t)i * dt) >> FIXED32_FRACT_BITS;
              i = i + controller->prev_i;
    
    // D = Kd * (e(t) - e(t-1)) / dt;
    fixed32_t d = ((int64_t)controller->kd * (e - controller->prev_e)) / dt;
    
    int32_t delta_min = -controller->clamp_width;
    int32_t delta_max = controller->clamp_width;
    
    p = CLAMP(p, delta_min, delta_max);
    d = CLAMP(d, delta_min, delta_max);
    i = CLAMP(i, controller->clamp_min, controller->clamp_max);
    
    controller->prev_e = e;
    controller->prev_i = i;
    
    controller->value = p + i + d;
    
    controller->value = CLAMP(controller->value, controller->clamp_min, controller->clamp_max);
    
    return true;
}
