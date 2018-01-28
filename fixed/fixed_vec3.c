#include "fixed_vec3.h"
#include "utils/utils.h"



bool vec3_equal(const vec3_t* v1, const vec3_t* v2)
{
    return v1->x == v2->x && v1->y == v2->y && v1->z == v2->z;
}

void vec3_add(vec3_t* v, const vec3_t* v1, const vec3_t* v2)
{
    v->x = add_f32(v1->x, v2->x);
    v->y = add_f32(v1->y, v2->y);
    v->z = add_f32(v1->z, v2->z);
}

void vec3_sub(vec3_t* v, const vec3_t* v1, const vec3_t* v2)
{
    v->x = sub_f32(v1->x, v2->x);
    v->y = sub_f32(v1->y, v2->y);
    v->z = sub_f32(v1->z, v2->z);
}

void vec3_mul(vec3_t* v, const vec3_t* v1, const vec3_t* v2)
{
    v->x = mul_f32(v1->x, v2->x);
    v->y = mul_f32(v1->y, v2->y);
    v->z = mul_f32(v1->z, v2->z);
}

void vec3_mul_n(vec3_t* v, const vec3_t* v1, f32_t n)
{
    v->x = mul_f32(v1->x, n);
    v->y = mul_f32(v1->y, n);
    v->z = mul_f32(v1->z, n);
}

void vec3_div(vec3_t* v, const vec3_t* v1, const vec3_t* v2)
{
    v->x = div_f32(v1->x, v2->x);
    v->y = div_f32(v1->y, v2->y);
    v->z = div_f32(v1->z, v2->z);
}

void vec3_div_n(vec3_t* v, const vec3_t* v1, f32_t n)
{
    v->x = div_f32(v1->x, n);
    v->y = div_f32(v1->y, n);
    v->z = div_f32(v1->z, n);
}

f32_t vec3_length(const vec3_t* v)
{
    f32_t xx = mul_f32(v->x, v->x);
    f32_t yy = mul_f32(v->y, v->y);
    f32_t zz = mul_f32(v->z, v->z);
    
    f32_t s = add_f32(xx, yy);
          s = add_f32(s, zz);
    f32_t l = sqrt_f32(s);
    
    return l;
}

f32_t vec3_distance(const vec3_t* v1, const vec3_t* v2)
{
    vec3_t v;
    
    vec3_sub(&v, v1, v2);
    
    return vec3_length(&v);
}

f32_t vec3_normalize(vec3_t* v, const vec3_t* v1)
{
    f32_t l = vec3_length(v1);
    
    if(l != 0) vec3_div_n(v, v1, l);
    
    return l;
}

f32_t vec3_dot(const vec3_t* v1, const vec3_t* v2)
{
    f32_t xx = mul_f32(v1->x, v2->x);
    f32_t yy = mul_f32(v1->y, v2->y);
    f32_t zz = mul_f32(v1->z, v2->z);
    
    f32_t s = add_f32(xx, yy);
          s = add_f32(s, zz);
    
    return s;
}

void vec3_cross(vec3_t* v, const vec3_t* v1, const vec3_t* v2)
{
    f32_t yz = mul_f32(v1->y, v2->z);
    f32_t zy = mul_f32(v1->z, v2->y);
    f32_t x = sub_f32(yz, zy);
    
    f32_t zx = mul_f32(v1->z, v2->x);
    f32_t xz = mul_f32(v1->x, v2->z);
    f32_t y = sub_f32(zx, xz);
    
    f32_t xy = mul_f32(v1->x, v2->y);
    f32_t yx = mul_f32(v1->y, v2->x);
    f32_t z = sub_f32(xy, yx);
    
    v->x = x;
    v->y = y;
    v->z = z;
}

void vec3_clamp(vec3_t* v, const vec3_t* v1, const vec3_t* v_min, const vec3_t* v_max)
{
    v->x = CLAMP(v1->x, v_min->x, v_max->x);
    v->y = CLAMP(v1->y, v_min->y, v_max->y);
    v->z = CLAMP(v1->z, v_min->z, v_max->z);
}
