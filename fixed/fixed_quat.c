#include "fixed_quat.h"
#include "utils/utils.h"



bool quat_equal(const quat_t* q1, const quat_t* q2)
{
    return q1->x == q2->x && q1->y == q2->y &&
           q1->z == q2->z && q1->w == q2->w;
}

f32_t quat_angle(const quat_t* q)
{
    return muli_f32(acos_f32(q->w), 2);
}

bool quat_axis(const quat_t* q, vec3_t* v)
{
    // sin_a2 = sqrt(1.0 - w * w);
    f32_t ww = mul_f32(q->w, q->w);
    f32_t _1ww = sub_f32(F32I(1), ww);
    f32_t sin_a2 = sqrt_f32(_1ww);
    
    if(sin_a2 == 0) return false;
    
    v->x = div_f32(q->x, sin_a2);
    v->y = div_f32(q->y, sin_a2);
    v->z = div_f32(q->z, sin_a2);
    
    return true;
}

void quat_rotation(quat_t* q, f32_t x, f32_t y, f32_t z, f32_t angle)
{
    f32_t a2 = divi_f32(angle, 2);
    f32_t sina2 = sin_f32(a2);
    f32_t cosa2 = cos_f32(a2);
    
    q->x = mul_f32(x, sina2);
    q->y = mul_f32(y, sina2);
    q->z = mul_f32(z, sina2);
    q->w = cosa2;
}

void quat_rotation2(quat_t* q, const vec3_t* v1, const vec3_t* v2)
{
    vec3_t axis;
    f32_t angle;
    
    vec3_cross(&axis, v1, v2);
    vec3_normalize(&axis, &axis);
    
    angle = acos_f32(vec3_dot(v1, v2));
    
    quat_rotationv(q, &axis, angle);
}

void quat_from_eulers(quat_t* q, f32_t roll, f32_t pitch, f32_t yaw)
{
    /*
     * https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
     */
    
    f32_t ax2 = divi_f32(roll, 2);
    f32_t ay2 = divi_f32(pitch, 2);
    f32_t az2 = divi_f32(yaw, 2);
    
    f32_t cx = cos_f32(ax2);
    f32_t cy = cos_f32(ay2);
    f32_t cz = cos_f32(az2);
    
    f32_t sx = sin_f32(ax2);
    f32_t sy = sin_f32(ay2);
    f32_t sz = sin_f32(az2);
    
    f32_t cx_cy = mul_f32(cx, cy);
    f32_t sx_sy = mul_f32(sx, sy);
    f32_t sx_cy = mul_f32(sx, cy);
    f32_t cx_sy = mul_f32(cx, sy);
    
    f32_t cx_cy_cz = mul_f32(cx_cy, cz); f32_t sx_sy_sz = mul_f32(sx_sy, sz);
    f32_t sx_cy_cz = mul_f32(sx_cy, cz); f32_t cx_sy_sz = mul_f32(cx_sy, sz);
    f32_t cx_sy_cz = mul_f32(cx_sy, cz); f32_t sx_cy_sz = mul_f32(sx_cy, sz);
    f32_t cx_cy_sz = mul_f32(cx_cy, sz); f32_t sx_sy_cz = mul_f32(sx_sy, cz);
    
    q->w = add_f32(cx_cy_cz, sx_sy_sz);
    q->x = sub_f32(sx_cy_cz, cx_sy_sz);
    q->y = add_f32(cx_sy_cz, sx_cy_sz);
    q->z = sub_f32(cx_cy_sz, sx_sy_cz);
}

void quat_to_eulers(const quat_t* q, f32_t* roll, f32_t* pitch, f32_t* yaw)
{
    /*
     * https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
     */
    
    f32_t ax, ay, az;
    
    f32_t xy = mul_f32(q->x, q->y);
    f32_t zw = mul_f32(q->z, q->w);
    
    f32_t xz = mul_f32(q->x, q->z);
    f32_t yw = mul_f32(q->y, q->w);
    
    f32_t xw = mul_f32(q->x, q->w);
    f32_t yz = mul_f32(q->y, q->z);
    
    f32_t xx = mul_f32(q->x, q->x);
    f32_t yy = mul_f32(q->y, q->y);
    f32_t zz = mul_f32(q->z, q->z);
    //f32_t ww = mul_f32(q->w, q->w);
    
    
    f32_t xw_yz = add_f32(xw, yz);
    f32_t xw_yz2 = add_f32(xw_yz, xw_yz);
    
    f32_t xx_yy = add_f32(xx, yy);
    f32_t xx_yy2 = add_f32(xx_yy, xx_yy);
    f32_t _1xx_yy2 = sub_f32(F32I(1), xx_yy2);
    
    ax = atan2_f32(_1xx_yy2, xw_yz2);
    
    f32_t yw_xz = sub_f32(yw, xz);
    f32_t yw_xz2 = add_f32(yw_xz, yw_xz);
    
    ay = asin_f32(yw_xz2);
    
    f32_t xy_zw = add_f32(xy, zw);
    f32_t xy_zw2 = add_f32(xy_zw, xy_zw);
    
    f32_t yy_zz = add_f32(yy, zz);
    f32_t yy_zz2 = add_f32(yy_zz, yy_zz);
    f32_t _1yy_zz2 = sub_f32(F32I(1), yy_zz2);
    
    az = atan2_f32(_1yy_zz2, xy_zw2);
    
    *roll = ax;
    *pitch = ay;
    *yaw = az;
}

void quat_add(quat_t* q, const quat_t* q1, const quat_t* q2)
{
    q->x = add_f32(q1->x, q2->x);
    q->y = add_f32(q1->y, q2->y);
    q->z = add_f32(q1->z, q2->z);
    q->w = add_f32(q1->w, q2->w);
}

void quat_sub(quat_t* q, const quat_t* q1, const quat_t* q2)
{
    q->x = sub_f32(q1->x, q2->x);
    q->y = sub_f32(q1->y, q2->y);
    q->z = sub_f32(q1->z, q2->z);
    q->w = sub_f32(q1->w, q2->w);
}

void quat_mul(quat_t* q, const quat_t* q1, const quat_t* q2)
{
    vec3_t v1;
    vec3_t v2;
    
    f32_t w1;
    f32_t w2;
    
    vec3_t resv;
    f32_t resw;
    
    quat_xyz(q1, &v1);
    quat_xyz(q2, &v2);
    
    w1 = quat_w(q1);
    w2 = quat_w(q2);
    
    //resw = w1 * w2 - dot(v1, v2);
    f32_t w1w2     = mul_f32(w1, w2);
    f32_t dot_v1v2 = vec3_dot(&v1, &v2);
          resw     = sub_f32(w1w2, dot_v1v2);
    
    // resv = cross(v1, v2) + v1 * w2 + v2 * w1;
    vec3_cross(&resv, &v1, &v2);
    vec3_mul_n(&v1, &v1, w2);
    vec3_mul_n(&v2, &v2, w1);
    vec3_add(&resv, &resv, &v1);
    vec3_add(&resv, &resv, &v2);
    
    q->x = resv.x;
    q->y = resv.y;
    q->z = resv.z;
    q->w = resw;
}

void quat_mul_n(quat_t* q, const quat_t* q1, f32_t n)
{
    q->x = mul_f32(q1->x, n);
    q->y = mul_f32(q1->y, n);
    q->z = mul_f32(q1->z, n);
    q->w = mul_f32(q1->w, n);
}

void quat_div_n(quat_t* q, const quat_t* q1, f32_t n)
{
    q->x = div_f32(q1->x, n);
    q->y = div_f32(q1->y, n);
    q->z = div_f32(q1->z, n);
    q->w = div_f32(q1->w, n);
}

f32_t quat_norm(const quat_t* q)
{
    f32_t xx = mul_f32(q->x, q->x);
    f32_t yy = mul_f32(q->y, q->y);
    f32_t zz = mul_f32(q->z, q->z);
    f32_t ww = mul_f32(q->w, q->w);
    
    f32_t n = add_f32(xx, yy);
          n = add_f32(n, zz);
          n = add_f32(n, ww);
    
    return n;
}

f32_t quat_magnitude(const quat_t* q)
{
    f32_t xx = mul_f32(q->x, q->x);
    f32_t yy = mul_f32(q->y, q->y);
    f32_t zz = mul_f32(q->z, q->z);
    f32_t ww = mul_f32(q->w, q->w);
    
    f32_t s = add_f32(xx, yy);
          s = add_f32(s, zz);
          s = add_f32(s, ww);
    f32_t l = sqrt_f32(s);
    
    return l;
}

void quat_conjugate(quat_t* q, const quat_t* q1)
{
    q->x = -q1->x;
    q->y = -q1->y;
    q->z = -q1->z;
    q->w = q1->w;
}

f32_t quat_invert(quat_t* q, const quat_t* q1)
{
    quat_t qc;
    f32_t n = quat_magnitude(q1);
    
    if(n != 0){
        quat_conjugate(&qc, q1);
        quat_div_n(q, &qc, n);
    }
    
    return n;
}

f32_t quat_inner(const quat_t* q1, const quat_t* q2)
{
    f32_t xx = mul_f32(q1->x, q2->x);
    f32_t yy = mul_f32(q1->y, q2->y);
    f32_t zz = mul_f32(q1->z, q2->z);
    f32_t ww = mul_f32(q1->w, q2->w);
    
    f32_t i = add_f32(xx, yy);
          i = add_f32(i, zz);
          i = add_f32(i, ww);
    
    return i;
}

f32_t quat_normalize(quat_t* q, const quat_t* q1)
{
    f32_t m = quat_magnitude(q1);
    
    if(m != 0) quat_div_n(q, q1, m);
    
    return m;
}

void quat_rotate_vec3(const quat_t* q, const vec3_t* v_in, vec3_t* v_out)
{
    quat_t qv;
    quat_set(&qv, v_in->x, v_in->y, v_in->z, 0);
    
    quat_t qinv;
    quat_invert(&qinv, q);
    
    quat_t qres;
    quat_mul(&qres, q, &qv);
    quat_mul(&qres, &qres, &qinv);
    
    quat_xyz(&qres, v_out);
}

void quat_rotate_vec4(const quat_t* q, const vec4_t* v_in, vec4_t* v_out)
{
    quat_t qv;
    quat_set(&qv, v_in->x, v_in->y, v_in->z, v_in->w);
    
    quat_t qinv;
    quat_invert(&qinv, q);
    
    quat_t qres;
    quat_mul(&qres, q, &qv);
    quat_mul(&qres, &qres, &qinv);
    
    quat_xyzw(&qres, v_out);
}

void quat_slerp(quat_t* q, const quat_t* q1, const quat_t* q2, f32_t t)
{
    #define SLERP_TO_LERP F32F(1, 100)
    
    /*
    iq = (q*sin((1-t)*omega) + q'*sin(t*omega))/sin(omega),
    где cos(omega) = inner_product(q,q')
    */
    
    quat_t qa;
    quat_t qb;
    quat_t qres;
    
    f32_t _1t = sub_f32(F32I(1), t);
    
    f32_t inner_prod = quat_inner(q1, q2);
    
    if(inner_prod > SLERP_TO_LERP){
        f32_t omega = acos_f32(inner_prod);

        f32_t a_omega = mul_f32(_1t, omega);
        f32_t b_omega = mul_f32(t, omega);

        f32_t sin_a_omega = sin_f32(a_omega);
        f32_t sin_b_omega = sin_f32(b_omega);
        f32_t sin_omega = sin_f32(omega);

        quat_mul_n(&qa, q1, sin_a_omega);
        quat_mul_n(&qb, q2, sin_b_omega);
        quat_add(&qres, &qa, &qb);

        quat_div_n(&qres, &qres, sin_omega);
    }else{
        quat_mul_n(&qa, q1, _1t);
        quat_mul_n(&qb, q2, t);
        quat_add(&qres, &qa, &qb);
    }
    
    q->x = qres.x;
    q->y = qres.y;
    q->z = qres.z;
    q->w = qres.w;
    
    #undef SLERP_TO_LERP
}

void quat_clamp(quat_t* q, const quat_t* q1, const quat_t* q_min, const quat_t* q_max)
{
    q->x = CLAMP(q1->x, q_min->x, q_max->x);
    q->y = CLAMP(q1->y, q_min->y, q_max->y);
    q->z = CLAMP(q1->z, q_min->z, q_max->z);
    q->w = CLAMP(q1->w, q_min->w, q_max->w);
}
