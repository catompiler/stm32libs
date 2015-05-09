#include "orientation.h"
#include "cordic/cordic32.h"
#include "utils/utils.h"
#include <string.h>



void orientation_init(orientation_t* orientation, const gyro6050_t* gyro, const hmc5883l_t* compass)
{
    memset(orientation, 0x0, sizeof(orientation_t));
    orientation->gyro = gyro;
    orientation->compass = compass;
    orientation->accel_angle_weight = ORIENTATION_ACCEL_ANGLE_WEIGHT_DEFAULT;
    orientation->counter = system_counter_ticks();
}

uint8_t orientation_accel_angle_weight(orientation_t* orientation)
{
    return orientation->accel_angle_weight;
}

void orientation_set_accel_angle_weight(orientation_t* orientation, uint8_t weight)
{
    if(weight > ORIENTATION_ACCEL_ANGLE_WEIGHT_MAX) weight = ORIENTATION_ACCEL_ANGLE_WEIGHT_MAX;
    
    orientation->accel_angle_weight = weight;
}

static fixed32_t orientation_alpha_beta_filter(orientation_t* orientation, fixed32_t a, fixed32_t b)
{
    int64_t a64 = (int64_t)a;
    int64_t b64 = (int64_t)b;
    
    return (fixed32_t)((a64 * orientation->accel_angle_weight +
            b64 * (ORIENTATION_ACCEL_ANGLE_WEIGHT_MAX - orientation->accel_angle_weight)) /
            ORIENTATION_ACCEL_ANGLE_WEIGHT_MAX);
}

static ALWAYS_INLINE fixed32_t orientation_clamp_angle(fixed32_t angle)
{
    if(angle < 0) angle += CORDIC32_ANGLE_360;
    else if(angle >= CORDIC32_ANGLE_360) angle -= CORDIC32_ANGLE_360;
    return angle;
}

static ALWAYS_INLINE void orientation_fix_border(fixed32_t* gyro_angle, fixed32_t* accel_angle)
{
    if(*gyro_angle > CORDIC32_ANGLE_270 && *accel_angle < CORDIC32_ANGLE_90)
        *accel_angle += CORDIC32_ANGLE_360;
    if(*gyro_angle < CORDIC32_ANGLE_90 && *accel_angle > CORDIC32_ANGLE_270 )
        *gyro_angle += CORDIC32_ANGLE_360;
}

// Предельное значение проекции ускорения,
// вплоть до которого акселерометр ещё способен
// выдавать корректные углы вдоль этой оси,
// после которого он не учитывается.
#define ACCEL_MAX 0xfd70 //(0.99)

void orientation_calculate(orientation_t* orientation)
{
    // Время интегрирования.
    int32_t dt = (int32_t)system_counter_diff(&orientation->counter);
    // Установим время последнего обновления.
    orientation->counter = system_counter_ticks();

    // Угол по каждой оси по данным акселерометра.
    fixed32_t accel_angle_x, accel_angle_y, accel_angle_z;
    // Ускорения по осям.
    fixed32_t accel_x = gyro6050_accel_x(orientation->gyro);
    fixed32_t accel_y = gyro6050_accel_y(orientation->gyro);
    fixed32_t accel_z = gyro6050_accel_z(orientation->gyro);
    // X.
    cordic32_atan2_hyp(accel_z, accel_y, &accel_angle_x, NULL);
    // Y.
    cordic32_atan2_hyp(accel_z, accel_x, &accel_angle_y, NULL);
    // Z.
    cordic32_atan2_hyp(accel_x, accel_y, &accel_angle_z, NULL);
    
    accel_angle_x = orientation_clamp_angle(accel_angle_x);
    accel_angle_y = orientation_clamp_angle(accel_angle_y);
    accel_angle_z = orientation_clamp_angle(accel_angle_z);
    
    // Азимут на север.
    if(orientation->compass){
        cordic32_atan2_hyp(hmc5883l_compass_x(orientation->compass),
                           hmc5883l_compass_y(orientation->compass),
                           &orientation->north_azimuth, NULL);
    }
    
    
    // Углы по осям гироскопа.
    fixed32_t gyro_angle_x = 0; //X.
    fixed32_t gyro_angle_y = 0; //Y.
    fixed32_t gyro_angle_z = 0; //Z.
    // Если это не первое вычисление.
    if(orientation->angle_x != 0 || orientation->angle_y != 0 || orientation->angle_z != 0){
        // Проинтегрируем скорость вращения по углам гироскопа.
        // Число тиков в секунду.
        int32_t tps2 = (int32_t)system_counter_ticks_per_sec() * 2;
        // X.
        gyro_angle_x = orientation->angle_x + (gyro6050_gyro_w_x(orientation->gyro) + orientation->gyro_w_x_prev) * dt / tps2;
        // Y.
        gyro_angle_y = orientation->angle_y + (gyro6050_gyro_w_y(orientation->gyro) + orientation->gyro_w_y_prev) * dt / tps2;
        // Z.
        gyro_angle_z = orientation->angle_z + (gyro6050_gyro_w_z(orientation->gyro) + orientation->gyro_w_z_prev) * dt / tps2;
    // Иначе проинициализируем углы по акселерометру.
    }else{
        gyro_angle_x = accel_angle_x;
        gyro_angle_y = accel_angle_y;
        gyro_angle_z = accel_angle_z;
    }
    // Сохраним текущие значения скоростей вращения по осям гироскопа.
    orientation->gyro_w_x_prev = gyro6050_gyro_w_x(orientation->gyro);
    orientation->gyro_w_y_prev = gyro6050_gyro_w_y(orientation->gyro);
    orientation->gyro_w_z_prev = gyro6050_gyro_w_z(orientation->gyro);
    
    gyro_angle_x = orientation_clamp_angle(gyro_angle_x);
    gyro_angle_y = orientation_clamp_angle(gyro_angle_y);
    gyro_angle_z = orientation_clamp_angle(gyro_angle_z);
    
    orientation_fix_border(&gyro_angle_x, &accel_angle_x);
    orientation_fix_border(&gyro_angle_y, &accel_angle_y);
    orientation_fix_border(&gyro_angle_z, &accel_angle_z);

    
    // Отфильтруем углы.
    // Вес угла гироскопа.
    //int8_t gyro_angle_weight = ORIENTATION_ACCEL_ANGLE_WEIGHT_MAX - orientation->accel_angle_weight;
    // X.
    /*orientation->angle_x = (accel_angle_x * orientation->accel_angle_weight +
                            gyro_angle_x  * gyro_angle_weight)
                                    / ORIENTATION_ACCEL_ANGLE_WEIGHT_MAX;*/
    orientation->angle_x = orientation_clamp_angle(
                orientation_alpha_beta_filter(orientation,
                    ABS(accel_x) <= ACCEL_MAX ? accel_angle_x : orientation->angle_x,
                    gyro_angle_x)
            );
    // Y.
    /*orientation->angle_y = (accel_angle_y * orientation->accel_angle_weight +
                            gyro_angle_y  * gyro_angle_weight)
                                    / ORIENTATION_ACCEL_ANGLE_WEIGHT_MAX;*/
    orientation->angle_y = orientation_clamp_angle(
                orientation_alpha_beta_filter(orientation,
                    ABS(accel_y) <= ACCEL_MAX ? accel_angle_y : orientation->angle_y,
                    gyro_angle_y)
            );
    // Z.
    /*orientation->angle_z = (accel_angle_z * orientation->accel_angle_weight +
                            gyro_angle_z  * gyro_angle_weight)
                                    / ORIENTATION_ACCEL_ANGLE_WEIGHT_MAX;*/
    orientation->angle_z = orientation_clamp_angle(
                orientation_alpha_beta_filter(orientation,
                    ABS(accel_z) <= ACCEL_MAX ? accel_angle_z : orientation->angle_z,
                    gyro_angle_z)
            );
}
