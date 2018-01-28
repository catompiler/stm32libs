#include "fixed_math.h"
#include <stddef.h>


// Таблица синусов углов [1; 89].
#define SIN_TABLE_ANGLE_FIRST 1
#define SIN_TABLE_ANGLE_LAST 89
#define SIN_TABLE_ANGLE_STEP 1
#define SIN_TABLE_SIZE 89
static const uint16_t sin_table[SIN_TABLE_SIZE] = {
    0x0478, 0x08ef, 0x0d66, 0x11dc, 0x1650, 0x1ac2, 0x1f33, 0x23a1, 0x280c, 0x2c74,
    0x30d9, 0x353a, 0x3996, 0x3def, 0x4242, 0x4690, 0x4ad9, 0x4f1c, 0x5358, 0x578f,
    0x5bbe, 0x5fe6, 0x6407, 0x6820, 0x6c31, 0x7039, 0x7439, 0x782f, 0x7c1c, 0x8000,
    0x83da, 0x87a9, 0x8b6d, 0x8f27, 0x92d6, 0x9679, 0x9a11, 0x9d9c, 0xa11b, 0xa48e,
    0xa7f3, 0xab4c, 0xae97, 0xb1d5, 0xb505, 0xb827, 0xbb3a, 0xbe3f, 0xc135, 0xc41b,
    0xc6f3, 0xc9bb, 0xcc73, 0xcf1c, 0xd1b4, 0xd43c, 0xd6b3, 0xd91a, 0xdb6f, 0xddb4,
    0xdfe7, 0xe209, 0xe419, 0xe617, 0xe804, 0xe9de, 0xeba6, 0xed5c, 0xeeff, 0xf090,
    0xf20e, 0xf378, 0xf4d0, 0xf615, 0xf747, 0xf865, 0xf970, 0xfa68, 0xfb4c, 0xfc1c,
    0xfcd9, 0xfd82, 0xfe18, 0xfe99, 0xff07, 0xff60, 0xffa6, 0xffd8, 0xfff6,
};

// Таблица арксинусов.
#define ASIN_FRACT_BITS 16
#define ASIN_TABLE_INDEX_BITS 8
#define ASIN_TABLE_INDEX_MASK ((1 << ASIN_TABLE_INDEX_BITS) - 1)
#define ASIN_PART_BITS (ASIN_FRACT_BITS - ASIN_TABLE_INDEX_BITS)
#define ASIN_PART_MASK ((1 << ASIN_PART_BITS) - 1)
#define ASIN_TABLE_SIZE 255
static const int32_t asin_table[ASIN_TABLE_SIZE] = {
    0x394b, 0x7297, 0xabe4, 0xe531, 0x11e7f, 0x157ce, 0x1911e, 0x1ca70, 0x203c4, 0x23d1a, 
    0x27672, 0x2afcd, 0x2e92a, 0x3228a, 0x35bed, 0x39554, 0x3cebe, 0x4082c, 0x4419f, 0x47b15, 
    0x4b490, 0x4ee10, 0x52795, 0x5611e, 0x59aae, 0x5d443, 0x60dde, 0x6477f, 0x68126, 0x6bad5, 
    0x6f489, 0x72e46, 0x76809, 0x7a1d4, 0x7dba7, 0x81581, 0x84f65, 0x88950, 0x8c345, 0x8fd42, 
    0x93749, 0x97159, 0x9ab74, 0x9e598, 0xa1fc6, 0xa59ff, 0xa9443, 0xace92, 0xb08ec, 0xb4352, 
    0xb7dc4, 0xbb842, 0xbf2cc, 0xc2d63, 0xc6807, 0xca2b8, 0xcdd76, 0xd1843, 0xd531d, 0xd8e06, 
    0xdc8fd, 0xe0403, 0xe3f19, 0xe7a3e, 0xeb572, 0xef0b7, 0xf2c0c, 0xf6772, 0xfa2e9, 0xfde71, 
    0x101a0b, 0x1055b6, 0x109174, 0x10cd45, 0x110928, 0x11451f, 0x118129, 0x11bd46, 0x11f979, 0x1235bf, 
    0x12721b, 0x12ae8c, 0x12eb12, 0x1327ae, 0x136461, 0x13a12a, 0x13de0a, 0x141b02, 0x145812, 0x149539, 
    0x14d27a, 0x150fd3, 0x154d45, 0x158ad2, 0x15c878, 0x160639, 0x164415, 0x16820c, 0x16c01f, 0x16fe4f, 
    0x173c9b, 0x177b04, 0x17b98b, 0x17f82f, 0x1836f3, 0x1875d5, 0x18b4d7, 0x18f3f9, 0x19333b, 0x19729e, 
    0x19b222, 0x19f1c9, 0x1a3192, 0x1a717e, 0x1ab18d, 0x1af1c1, 0x1b3219, 0x1b7297, 0x1bb33a, 0x1bf404, 
    0x1c34f4, 0x1c760c, 0x1cb74d, 0x1cf8b6, 0x1d3a48, 0x1d7c05, 0x1dbded, 0x1e0000, 0x1e423e, 0x1e84aa, 
    0x1ec744, 0x1f0a0b, 0x1f4d02, 0x1f9028, 0x1fd37f, 0x201707, 0x205ac1, 0x209ead, 0x20e2ce, 0x212723, 
    0x216bae, 0x21b06e, 0x21f566, 0x223a96, 0x228000, 0x22c5a3, 0x230b81, 0x23519b, 0x2397f2, 0x23de87, 
    0x24255b, 0x246c6f, 0x24b3c4, 0x24fb5c, 0x254338, 0x258b58, 0x25d3bf, 0x261c6d, 0x266563, 0x26aea3, 
    0x26f82f, 0x274207, 0x278c2e, 0x27d6a4, 0x28216b, 0x286c84, 0x28b7f2, 0x2903b6, 0x294fd1, 0x299c45, 
    0x29e915, 0x2a3641, 0x2a83cd, 0x2ad1b8, 0x2b2007, 0x2b6eba, 0x2bbdd4, 0x2c0d58, 0x2c5d46, 0x2cada2, 
    0x2cfe6e, 0x2d4fad, 0x2da161, 0x2df38d, 0x2e4633, 0x2e9957, 0x2eecfb, 0x2f4122, 0x2f95d0, 0x2feb08, 
    0x3040cd, 0x309723, 0x30ee0d, 0x314590, 0x319daf, 0x31f66e, 0x324fd3, 0x32a9e0, 0x33049c, 0x33600a, 
    0x33bc31, 0x341915, 0x3476bc, 0x34d52c, 0x35346b, 0x359480, 0x35f571, 0x365746, 0x36ba05, 0x371db7, 
    0x378265, 0x37e817, 0x384ed6, 0x38b6ad, 0x391fa5, 0x3989cb, 0x39f529, 0x3a61cc, 0x3acfc2, 0x3b3f19, 
    0x3bafe0, 0x3c2228, 0x3c9603, 0x3d0b83, 0x3d82bd, 0x3dfbc7, 0x3e76ba, 0x3ef3af, 0x3f72c2, 0x3ff414, 
    0x4077c6, 0x40fdfe, 0x4186e6, 0x4212ac, 0x42a183, 0x4333a6, 0x43c955, 0x4462db, 0x45008b, 0x45a2c8, 
    0x464a00, 0x46f6b9, 0x47a98d, 0x486337, 0x492498, 0x49eec8, 0x4ac325, 0x4ba373, 0x4c9209, 0x4d921c, 
    0x4ea84c, 0x4fdbb3, 0x513846, 0x52d556, 0x54ef1f, //0x5A0000
};


#define CORDIC32_TAN_TABLE_LEN 24
static const f32_t cordic32_table[CORDIC32_TAN_TABLE_LEN] = {
    2949120, 1740967, 919879, 466945, 234379, 117304, 58666, 29335, 14668,
    7334, 3667, 1833, 917, 458, 229, 115, 57, 29, 14, 7, 4, 2, 1, 0
};




f32_t lerp_f32(f32_t a, f32_t b, f32_t t)
{
    // a * (1 - t) + b * t == a - a * t + b * t == a + (b - a) * t.
    f64_t c = sub64_f32_f32(b, a);
    f64_t d = mul64_f64_f32(c, t);
    f64_t e = add64_f64_f64(a, d);
    f32_t res = sat32_f64(e);
    
    return res;
}

/**
 * Производит основное вычисление квадратного корня
 * по алгоритму "Binary Square Root".
 * @param value Число.
 * @return Квадратный корень.
 */
static u32_t sqrt_u32_impl(u32_t value)
{
    uint32_t rem = 0;
    uint32_t root = 0;
    
    uint32_t i = 0;
    
    for(; i < 16; i ++){
        root <<= 1;
        rem = ((rem << 2) + (value >> 30));
        value <<= 2;
        root ++;
        if(root <= rem){
            rem -= root;
            root ++;
        }else{
            root --;
        }
    }
    return root;
}

i32_t sqrt_i32(i32_t n)
{
    if(n < 0) return -1;
    
    u32_t res = sqrt_u32_impl((u32_t)n) >> 1;
    
    return (i32_t)res;
}

f32_t sqrt_f32(f32_t n)
{
    if(n < 0) return -1;
    
    u32_t res = sqrt_u32_impl((u32_t)n) << 7;
    
    return (f32_t)res;
}

static f32_t normalize_angle_f32(f32_t angle)
{
    if(angle >= F32_ANGLE_360 || angle <= -F32_ANGLE_360) angle %= F32_ANGLE_360;
    if(angle < F32_ANGLE_0) angle += F32_ANGLE_360;

    return angle;
}

static f32_t normalize_angle_i32(i32_t angle)
{
    if(angle >= 360 || angle <= -360) angle %= 360;
    if(angle < 0) angle += 360;

    return angle;
}

f32_t sin_i32(i32_t angle)
{
    angle = normalize_angle_i32(angle);

    if(angle < 90){
        if(angle == 0) return 0;
        return (f32_t)sin_table[angle - 1];
    }else if(angle < 180){
        if(angle == 90) return F32I(1);
        return (f32_t)sin_table[179 - angle];
    }else if(angle < 270){
        if(angle == 180) return 0;
        return -(f32_t)sin_table[angle - 181];
    }else if(angle < 360){
        if(angle == 270) return F32I(-1);
        return -(f32_t)sin_table[359 - angle];
    }
    return 0; // 360.
}

f32_t cos_i32(i32_t angle)
{
    angle = normalize_angle_i32(angle);

    if(angle < 90){
        if(angle == 0) return F32I(1);
        return (f32_t)sin_table[89 - angle];
    }else if(angle < 180){
        if(angle == 90) return 0;
        return -(f32_t)sin_table[angle - 91];
    }else if(angle < 270){
        if(angle == 180) return F32I(-1);
        return -(f32_t)sin_table[269 - angle];
    }else if(angle < 360){
        if(angle == 270) return 0;
        return (f32_t)sin_table[angle - 271];
    }
    return F32I(1); // 360.
}

f32_t sin_f32(f32_t angle)
{
    f32_t norm_angle = normalize_angle_f32(angle);
    
    i32_t angle_a = int_f32(floor_f32(norm_angle));
    i32_t angle_b = angle_a + SIN_TABLE_ANGLE_STEP;
    
    f32_t a = sin_i32(angle_a);
    f32_t b = sin_i32(angle_b);
    
    f32_t t = fract_f32(norm_angle);
    
    return lerp_f32(a, b, t);
}

f32_t cos_f32(f32_t angle)
{
    f32_t norm_angle = normalize_angle_f32(angle);
    
    i32_t angle_a = int_f32(floor_f32(norm_angle));
    i32_t angle_b = angle_a + SIN_TABLE_ANGLE_STEP;
    
    f32_t a = cos_i32(angle_a);
    f32_t b = cos_i32(angle_b);
    
    f32_t t = fract_f32(norm_angle);
    
    return lerp_f32(a, b, t);
}

ALWAYS_INLINE static f32_t asin_table_get(size_t index)
{
    if(index == 0) return 0;
    if(index > ASIN_TABLE_SIZE) return F32I(90);
    
    return asin_table[index - 1];
}

static f32_t asin_impl_f32(f32_t value)
{
    f32_t abs_val = abs_f32(value);
    
    int32_t index = (abs_val >> (ASIN_FRACT_BITS - ASIN_TABLE_INDEX_BITS)) & ASIN_TABLE_INDEX_MASK;
    f32_t part = (abs_val & ASIN_PART_MASK) << (ASIN_FRACT_BITS - ASIN_PART_BITS);
    
    f32_t angle_first = asin_table_get(index);
    f32_t angle_second = asin_table_get(index + 1);
    f32_t angle =  lerp_f32(angle_first, angle_second, part);
    
    if(value < 0) angle = -angle;
    
    return angle;
}

f32_t asin_f32(f32_t value)
{
    if(value >=  F32I(1)) return F32_ANGLE_90;
    if(value <= -F32I(1)) return F32_ANGLE_270;
    if(value == 0) return F32_ANGLE_0;
    
    f32_t angle =  asin_impl_f32(value);
    
    return normalize_angle_f32(angle);
}

f32_t acos_f32(f32_t value)
{
    if(value >=  F32I(1)) return F32_ANGLE_0;
    if(value <= -F32I(1)) return F32_ANGLE_180;
    if(value == 0) return F32_ANGLE_90;
    
    f32_t angle = F32_ANGLE_90 - asin_impl_f32(value);
    
    return normalize_angle_f32(angle);
}

f32_t atan2_f32(f32_t x, f32_t y)
{
    f32_t sum_angle = 0;
    uint32_t loop_n = 0;
    
    f32_t new_x, new_y;
    
    if(x < 0){
        x = -x;
        y = -y;
        sum_angle = F32_ANGLE_180;
    }else if(y < 0){
        sum_angle = F32_ANGLE_360;
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
    
    return sum_angle;
}

