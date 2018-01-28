#include "fixed_vec2.h"
#include "utils/utils.h"



bool vec2_equal(const vec2_t* v1, const vec2_t* v2)
{
    return v1->x == v2->x && v1->y == v2->y;
}

void vec2_add(vec2_t* v, const vec2_t* v1, const vec2_t* v2)
{
    v->x = add_f32(v1->x, v2->x);
    v->y = add_f32(v1->y, v2->y);
}

void vec2_sub(vec2_t* v, const vec2_t* v1, const vec2_t* v2)
{
    v->x = sub_f32(v1->x, v2->x);
    v->y = sub_f32(v1->y, v2->y);
}

void vec2_mul(vec2_t* v, const vec2_t* v1, const vec2_t* v2)
{
    v->x = mul_f32(v1->x, v2->x);
    v->y = mul_f32(v1->y, v2->y);
}

void vec2_mul_n(vec2_t* v, const vec2_t* v1, f32_t n)
{
    v->x = mul_f32(v1->x, n);
    v->y = mul_f32(v1->y, n);
}

void vec2_div(vec2_t* v, const vec2_t* v1, const vec2_t* v2)
{
    v->x = div_f32(v1->x, v2->x);
    v->y = div_f32(v1->y, v2->y);
}

void vec2_div_n(vec2_t* v, const vec2_t* v1, f32_t n)
{
    v->x = div_f32(v1->x, n);
    v->y = div_f32(v1->y, n);
}

f32_t vec2_length(const vec2_t* v)
{
    f32_t xx = mul_f32(v->x, v->x);
    f32_t yy = mul_f32(v->y, v->y);
    
    f32_t s = add_f32(xx, yy);
    f32_t l = sqrt_f32(s);
    
    return l;
}

f32_t vec2_distance(const vec2_t* v1, const vec2_t* v2)
{
    vec2_t v;
    
    vec2_sub(&v, v1, v2);
    
    return vec2_length(&v);
}

f32_t vec2_normalize(vec2_t* v, const vec2_t* v1)
{
    f32_t l = vec2_length(v1);
    
    if(l != 0) vec2_div_n(v, v1, l);
    
    return l;
}

f32_t vec2_dot(const vec2_t* v1, const vec2_t* v2)
{
    f32_t xx = mul_f32(v1->x, v2->x);
    f32_t yy = mul_f32(v1->y, v2->y);
    
    f32_t s = add_f32(xx, yy);
    
    return s;
}

void vec2_clamp(vec2_t* v, const vec2_t* v1, const vec2_t* v_min, const vec2_t* v_max)
{
    v->x = CLAMP(v1->x, v_min->x, v_max->x);
    v->y = CLAMP(v1->y, v_min->y, v_max->y);
}
