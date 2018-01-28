#include "fixed_vec4.h"
#include "utils/utils.h"



bool vec4_equal(const vec4_t* v1, const vec4_t* v2)
{
    return v1->x == v2->x && v1->y == v2->y &&
           v1->z == v2->z && v1->w == v2->w;
}

void vec4_add(vec4_t* v, const vec4_t* v1, const vec4_t* v2)
{
    v->x = add_f32(v1->x, v2->x);
    v->y = add_f32(v1->y, v2->y);
    v->z = add_f32(v1->z, v2->z);
    v->w = add_f32(v1->w, v2->w);
}

void vec4_sub(vec4_t* v, const vec4_t* v1, const vec4_t* v2)
{
    v->x = sub_f32(v1->x, v2->x);
    v->y = sub_f32(v1->y, v2->y);
    v->z = sub_f32(v1->z, v2->z);
    v->w = sub_f32(v1->w, v2->w);
}

void vec4_mul(vec4_t* v, const vec4_t* v1, const vec4_t* v2)
{
    v->x = mul_f32(v1->x, v2->x);
    v->y = mul_f32(v1->y, v2->y);
    v->z = mul_f32(v1->z, v2->z);
    v->w = mul_f32(v1->w, v2->w);
}

void vec4_mul_n(vec4_t* v, const vec4_t* v1, f32_t n)
{
    v->x = mul_f32(v1->x, n);
    v->y = mul_f32(v1->y, n);
    v->z = mul_f32(v1->z, n);
    v->w = mul_f32(v1->w, n);
}

void vec4_div(vec4_t* v, const vec4_t* v1, const vec4_t* v2)
{
    v->x = div_f32(v1->x, v2->x);
    v->y = div_f32(v1->y, v2->y);
    v->z = div_f32(v1->z, v2->z);
    v->w = div_f32(v1->w, v2->w);
}

void vec4_div_n(vec4_t* v, const vec4_t* v1, f32_t n)
{
    v->x = div_f32(v1->x, n);
    v->y = div_f32(v1->y, n);
    v->z = div_f32(v1->z, n);
    v->w = div_f32(v1->w, n);
}

f32_t vec4_length(const vec4_t* v)
{
    f32_t xx = mul_f32(v->x, v->x);
    f32_t yy = mul_f32(v->y, v->y);
    f32_t zz = mul_f32(v->z, v->z);
    f32_t ww = mul_f32(v->w, v->w);
    
    f32_t s = add_f32(xx, yy);
          s = add_f32(s, zz);
          s = add_f32(s, ww);
    f32_t l = sqrt_f32(s);
    
    return l;
}

f32_t vec4_distance(const vec4_t* v1, const vec4_t* v2)
{
    vec4_t v;
    
    vec4_sub(&v, v1, v2);
    
    return vec4_length(&v);
}

f32_t vec4_normalize(vec4_t* v, const vec4_t* v1)
{
    f32_t l = vec4_length(v1);
    
    if(l != 0) vec4_div_n(v, v1, l);
    
    return l;
}

f32_t vec4_dot(const vec4_t* v1, const vec4_t* v2)
{
    f32_t xx = mul_f32(v1->x, v2->x);
    f32_t yy = mul_f32(v1->y, v2->y);
    f32_t zz = mul_f32(v1->z, v2->z);
    f32_t ww = mul_f32(v1->w, v2->w);
    
    f32_t s = add_f32(xx, yy);
          s = add_f32(s, zz);
          s = add_f32(s, ww);
    
    return s;
}

void vec4_clamp(vec4_t* v, const vec4_t* v1, const vec4_t* v_min, const vec4_t* v_max)
{
    v->x = CLAMP(v1->x, v_min->x, v_max->x);
    v->y = CLAMP(v1->y, v_min->y, v_max->y);
    v->z = CLAMP(v1->z, v_min->z, v_max->z);
    v->w = CLAMP(v1->w, v_min->w, v_max->w);
}
