/**
 * @file graphics_inline.h Библиотека инлайн формато-зависимых функций по работе с графикой.
 */

#ifndef _GRAPHICS_INLINE_H_
#define _GRAPHICS_INLINE_H_

// ютф8!

#include "defs/defs.h"
#include "bits/bits.h"


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#else
#endif


/*
 * POS.
 */

#ifdef USE_GRAPHICS_FORMAT_BW_1_V
static ALWAYS_INLINE void graphics_bw_1_v_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = x + (y >> 3) * graphics->width;
    if(bit)  *bit  = y & 0x7;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_BW_1_H
static ALWAYS_INLINE void graphics_bw_1_h_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = (x >> 3) + y * (graphics->width >> 3);
    if(bit)  *bit  = x & 0x7;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
static ALWAYS_INLINE void graphics_gray_2_v_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = x + (y >> 2) * graphics->width;
    if(bit)  *bit  = (y & 0x3) << 1;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
static ALWAYS_INLINE void graphics_gray_2_h_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = (x >> 2) + y * (graphics->width >> 2);
    if(bit)  *bit  = (x & 0x3) << 1;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
static ALWAYS_INLINE void graphics_rgb_121_v_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = x + (y >> 1) * graphics->width;
    if(bit)  *bit  = (y & 0x1) << 2;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
static ALWAYS_INLINE void graphics_rgb_121_h_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = (x >> 1) + y * (graphics->width >> 1);
    if(bit)  *bit  = (x & 0x1) << 2;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
static ALWAYS_INLINE void graphics_rgb_332_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = x + y * graphics->width;
    if(bit)  *bit  = 0;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
static ALWAYS_INLINE void graphics_rgb_565_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = (x + y * graphics->width) * 2;
    if(bit)  *bit  = 0;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
static ALWAYS_INLINE void graphics_rgb_8_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = (x + y * graphics->width) * 3;
    if(bit)  *bit  = 0;
}
#endif

/*
 * GET.
 */

#ifdef USE_GRAPHICS_FORMAT_BW_1_V
static ALWAYS_INLINE graphics_color_t graphics_bw_1_v_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0x1;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_BW_1_H
static ALWAYS_INLINE graphics_color_t graphics_bw_1_h_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0x1;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
static ALWAYS_INLINE graphics_color_t graphics_gray_2_v_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0x3;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
static ALWAYS_INLINE graphics_color_t graphics_gray_2_h_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0x3;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
static ALWAYS_INLINE graphics_color_t graphics_rgb_121_v_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0xf;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
static ALWAYS_INLINE graphics_color_t graphics_rgb_121_h_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0xf;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
static ALWAYS_INLINE graphics_color_t graphics_rgb_332_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0;

    graphics_rgb_332_get_pixel_pos(graphics, x, y, &byte, NULL);

    return graphics->data[byte];
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
static ALWAYS_INLINE graphics_color_t graphics_rgb_565_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_color_t color = 0;
    graphics_size_t byte = 0;

    graphics_rgb_565_get_pixel_pos(graphics, x, y, &byte, NULL);

    color = (graphics->data[byte]) |
            (graphics->data[byte + 1] << 8);

    return color;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
static ALWAYS_INLINE graphics_color_t graphics_rgb_8_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_color_t color = 0;
    graphics_size_t byte = 0;

    graphics_rgb_8_get_pixel_pos(graphics, x, y, &byte, NULL);

    color = (graphics->data[byte]) |
            (graphics->data[byte + 1] << 8) |
            (graphics->data[byte + 2] << 16);

    return color;
}
#endif

/*
 * SET.
 */

#ifdef USE_GRAPHICS_FORMAT_BW_1_V
static ALWAYS_INLINE void graphics_bw_1_v_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0x1 << bit);
    graphics->data[byte] = c | ((color /*& 0x1*/) << bit);
}
#endif

#ifdef USE_GRAPHICS_FORMAT_BW_1_H
static ALWAYS_INLINE void graphics_bw_1_h_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0x1 << bit);
    graphics->data[byte] = c | ((color /*& 0x1*/) << bit);
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
static ALWAYS_INLINE void graphics_gray_2_v_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0x3 << bit);
    graphics->data[byte] = c | ((color /*& 0x3*/) << bit);
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
static ALWAYS_INLINE void graphics_gray_2_h_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0x3 << bit);
    graphics->data[byte] = c | ((color /*& 0x3*/) << bit);
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
static ALWAYS_INLINE void graphics_rgb_121_v_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0xf << bit);
    graphics->data[byte] = c | ((color /*& 0xf*/) << bit);
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
static ALWAYS_INLINE void graphics_rgb_121_h_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0xf << bit);
    graphics->data[byte] = c | ((color /*& 0xf*/) << bit);
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
static ALWAYS_INLINE void graphics_rgb_332_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_332_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte] = color /*& 0xff*/;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
static ALWAYS_INLINE void graphics_rgb_565_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_565_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     = color & 0xff;
    graphics->data[byte + 1] = (color >> 8) /*& 0xff*/;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
static ALWAYS_INLINE void graphics_rgb_8_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_8_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     = color & 0xff;
    graphics->data[byte + 1] = (color >> 8) & 0xff;
    graphics->data[byte + 2] = (color >> 16) /*& 0xff*/;
}
#endif

/*
 * OR.
 */

#ifdef USE_GRAPHICS_FORMAT_BW_1_V
static ALWAYS_INLINE void graphics_bw_1_v_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color /*& 0x1*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_BW_1_H
static ALWAYS_INLINE void graphics_bw_1_h_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color /*& 0x1*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
static ALWAYS_INLINE void graphics_gray_2_v_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color /*& 0x3*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
static ALWAYS_INLINE void graphics_gray_2_h_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color /*& 0x3*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
static ALWAYS_INLINE void graphics_rgb_121_v_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color /*& 0xf*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
static ALWAYS_INLINE void graphics_rgb_121_h_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color /*& 0xf*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
static ALWAYS_INLINE void graphics_rgb_332_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_332_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte] |= color /*& 0xff*/;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
static ALWAYS_INLINE void graphics_rgb_565_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_565_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     |= color & 0xff;
    graphics->data[byte + 1] |= (color >> 8) /*& 0xff*/;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
static ALWAYS_INLINE void graphics_rgb_8_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_8_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     |= color & 0xff;
    graphics->data[byte + 1] |= (color >> 8) & 0xff;
    graphics->data[byte + 2] |= (color >> 16) /*& 0xff*/;
}
#endif

/*
 * XOR.
 */

#ifdef USE_GRAPHICS_FORMAT_BW_1_V
static ALWAYS_INLINE void graphics_bw_1_v_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color /*& 0x1*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_BW_1_H
static ALWAYS_INLINE void graphics_bw_1_h_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color /*& 0x1*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
static ALWAYS_INLINE void graphics_gray_2_v_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color /*& 0x3*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
static ALWAYS_INLINE void graphics_gray_2_h_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color /*& 0x3*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
static ALWAYS_INLINE void graphics_rgb_121_v_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color /*& 0xf*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
static ALWAYS_INLINE void graphics_rgb_121_h_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color /*& 0xf*/) << bit;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
static ALWAYS_INLINE void graphics_rgb_332_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_332_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte] ^= color /*& 0xff*/;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
static ALWAYS_INLINE void graphics_rgb_565_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_565_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     ^= color & 0xff;
    graphics->data[byte + 1] ^= (color >> 8) /*& 0xff*/;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
static ALWAYS_INLINE void graphics_rgb_8_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_8_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     ^= color & 0xff;
    graphics->data[byte + 1] ^= (color >> 8) & 0xff;
    graphics->data[byte + 2] ^= (color >> 16) /*& 0xff*/;
}
#endif

/*
 * AND.
 */

#ifdef USE_GRAPHICS_FORMAT_BW_1_V
static ALWAYS_INLINE void graphics_bw_1_v_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color /*& 0x1*/) << bit) | (~(1 << bit));
}
#endif

#ifdef USE_GRAPHICS_FORMAT_BW_1_H
static ALWAYS_INLINE void graphics_bw_1_h_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color /*& 0x1*/) << bit) | (~(1 << bit));
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
static ALWAYS_INLINE void graphics_gray_2_v_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color /*& 0x3*/) << bit) | (~(0x3 << bit));
}
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
static ALWAYS_INLINE void graphics_gray_2_h_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color /*& 0x3*/) << bit) | (~(0x3 << bit));
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
static ALWAYS_INLINE void graphics_rgb_121_v_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color /*& 0xf*/) << bit) | (~(0xf << bit));
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
static ALWAYS_INLINE void graphics_rgb_121_h_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color /*& 0xf*/) << bit) | (~(0xf << bit));
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
static ALWAYS_INLINE void graphics_rgb_332_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_332_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte] &= color /*& 0xff*/;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
static ALWAYS_INLINE void graphics_rgb_565_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_565_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     &= color & 0xff;
    graphics->data[byte + 1] &= (color >> 8) /*& 0xff*/;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
static ALWAYS_INLINE void graphics_rgb_8_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_8_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     &= color & 0xff;
    graphics->data[byte + 1] &= (color >> 8) & 0xff;
    graphics->data[byte + 2] &= (color >> 16) /*& 0xff*/;
}
#endif

/*
 * FROM COLOR.
 */

#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
static ALWAYS_INLINE graphics_color_t graphics_bw_1_color_from(graphics_format_t from_format, graphics_color_t color)
{
    switch(from_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return color;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            return color >= 0x2;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            return GRAPHICS_RGB_121_TO_GRAY(color) >= (GRAPHICS_RGB_121_TO_GRAY_MAX >> 1);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            return GRAPHICS_RGB_332_TO_GRAY(color) >= (GRAPHICS_RGB_332_TO_GRAY_MAX >> 1);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            return GRAPHICS_RGB_565_TO_GRAY(color) >= (GRAPHICS_RGB_565_TO_GRAY_MAX >> 1);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            return GRAPHICS_RGB_8_TO_GRAY(color) >= (GRAPHICS_RGB_8_TO_GRAY_MAX >> 1);
#endif
    }

    return 0;
}
#endif

#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
static ALWAYS_INLINE graphics_color_t graphics_gray_2_color_from(graphics_format_t from_format, graphics_color_t color)
{
    switch(from_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return (color == 0x0) ? 0x0 : 0x3;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            return color;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            return GRAPHICS_RGB_121_TO_GRAY(color) * 0x3 / GRAPHICS_RGB_121_TO_GRAY_MAX;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            return GRAPHICS_RGB_332_TO_GRAY(color) * 0x3 / GRAPHICS_RGB_332_TO_GRAY_MAX;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            return GRAPHICS_RGB_565_TO_GRAY(color) * 0x3 / GRAPHICS_RGB_565_TO_GRAY_MAX;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            return GRAPHICS_RGB_8_TO_GRAY(color) * 0x3 / GRAPHICS_RGB_8_TO_GRAY_MAX;
#endif
    }

    return 0;
}
#endif

#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
static ALWAYS_INLINE graphics_color_t graphics_rgb_121_color_from(graphics_format_t from_format, graphics_color_t color)
{
    uint32_t color_r, color_g, color_b;
    switch(from_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return (color == 0x0) ? 0x0 : 0xf;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            color_r = color >> 1;
            color_g = color;
            color_b = color_r;
            return GRAPHICS_COLOR_RGB_121(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            return color;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            color_r = GRAPHICS_RED_RGB_332(color) >> 2;
            color_g = GRAPHICS_GREEN_RGB_332(color) * 0x3; color_g = GRAPHICS_FAST_DIV_7(color_g);
            color_b = GRAPHICS_BLUE_RGB_332(color) >> 1;
            return GRAPHICS_COLOR_RGB_121(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            color_r = GRAPHICS_RED_RGB_565(color) >> 4;
            color_g = GRAPHICS_GREEN_RGB_565(color) * 0x3; color_g = GRAPHICS_FAST_DIV_63(color_g);
            color_b = GRAPHICS_BLUE_RGB_565(color) >> 4;
            return GRAPHICS_COLOR_RGB_121(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            color_r = GRAPHICS_RED_RGB_8(color) >> 7;
            color_g = GRAPHICS_GREEN_RGB_8(color) * 0x3; color_g = GRAPHICS_FAST_DIV_255(color_g);
            color_b = GRAPHICS_BLUE_RGB_8(color) >> 7;
            return GRAPHICS_COLOR_RGB_121(color_r, color_g, color_b);
#endif
    }

    return 0;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
static ALWAYS_INLINE graphics_color_t graphics_rgb_332_color_from(graphics_format_t from_format, graphics_color_t color)
{
    uint32_t color_r, color_g, color_b;
    switch(from_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return (color == 0x0) ? 0x0 : 0xff;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            color_r = color * 0x7; color_r = GRAPHICS_FAST_DIV_3(color_r);
            color_g = color_r;
            color_b = color;
            return GRAPHICS_COLOR_RGB_332(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            color_r = GRAPHICS_RED_RGB_121(color) * 0x7;
            color_g = GRAPHICS_GREEN_RGB_121(color) * 0x7; color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = GRAPHICS_BLUE_RGB_121(color) * 0x3;
            return GRAPHICS_COLOR_RGB_332(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            return color;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            color_r = GRAPHICS_RED_RGB_565(color) * 0x7; color_r = GRAPHICS_FAST_DIV_31(color_r);
            color_g = GRAPHICS_GREEN_RGB_565(color) * 0x7; color_g = GRAPHICS_FAST_DIV_63(color_g);
            color_b = GRAPHICS_BLUE_RGB_565(color) * 0x3; color_b = GRAPHICS_FAST_DIV_31(color_b);
            return GRAPHICS_COLOR_RGB_332(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            color_r = GRAPHICS_RED_RGB_8(color) * 0x7; color_r = GRAPHICS_FAST_DIV_255(color_r);
            color_g = GRAPHICS_GREEN_RGB_8(color) * 0x7; color_g = GRAPHICS_FAST_DIV_255(color_g);
            color_b = GRAPHICS_BLUE_RGB_8(color) * 0x3; color_b = GRAPHICS_FAST_DIV_255(color_b);
            return GRAPHICS_COLOR_RGB_332(color_r, color_g, color_b);
#endif
    }

    return 0;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
static ALWAYS_INLINE graphics_color_t graphics_rgb_565_color_from(graphics_format_t from_format, graphics_color_t color)
{
    uint32_t color_r, color_g, color_b;
    switch(from_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return (color == 0x0) ? 0x0 : 0xffff;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            color_r = color * 0x1f; color_r = GRAPHICS_FAST_DIV_3(color_r);
            color_g = color * 0x3f; color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = color_r;
            return GRAPHICS_COLOR_RGB_565(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            color_r = GRAPHICS_RED_RGB_121(color) * 0x1f;
            color_g = GRAPHICS_GREEN_RGB_121(color) * 0x3f; color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = GRAPHICS_BLUE_RGB_121(color) * 0x1f;
            return GRAPHICS_COLOR_RGB_565(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            color_r = GRAPHICS_RED_RGB_332(color) * 0x1f; color_r = GRAPHICS_FAST_DIV_7(color_r);
            color_g = GRAPHICS_GREEN_RGB_332(color) * 0x3f; color_g = GRAPHICS_FAST_DIV_7(color_g);
            color_b = GRAPHICS_BLUE_RGB_332(color) * 0x1f; color_b = GRAPHICS_FAST_DIV_3(color_b);
            return GRAPHICS_COLOR_RGB_565(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            return color;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            color_r = GRAPHICS_RED_RGB_8(color) * 0x1f; color_r = GRAPHICS_FAST_DIV_255(color_r);
            color_g = GRAPHICS_GREEN_RGB_8(color) * 0x3f; color_g = GRAPHICS_FAST_DIV_255(color_g);
            color_b = GRAPHICS_BLUE_RGB_8(color) * 0x1f; color_b = GRAPHICS_FAST_DIV_255(color_b);
            return GRAPHICS_COLOR_RGB_565(color_r, color_g, color_b);
#endif
    }

    return 0;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
static ALWAYS_INLINE graphics_color_t graphics_rgb_8_color_from(graphics_format_t from_format, graphics_color_t color)
{
    uint32_t color_r, color_g, color_b;
    switch(from_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return (color == 0x0) ? 0x0 : 0x00ffffff;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            color_r = color * 0xff; color_r = GRAPHICS_FAST_DIV_3(color_r);
            color_g = color_r;
            color_b = color_r;
            return GRAPHICS_COLOR_RGB_8(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            color_r = GRAPHICS_RED_RGB_121(color) * 0xff;
            color_g = GRAPHICS_GREEN_RGB_121(color) * 0xff; color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = GRAPHICS_BLUE_RGB_121(color) * 0xff;
            return GRAPHICS_COLOR_RGB_8(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            color_r = GRAPHICS_RED_RGB_332(color) * 0xff; color_r = GRAPHICS_FAST_DIV_7(color_r);
            color_g = GRAPHICS_GREEN_RGB_332(color) * 0xff; color_g = GRAPHICS_FAST_DIV_7(color_g);
            color_b = GRAPHICS_BLUE_RGB_332(color) * 0xff; color_b = GRAPHICS_FAST_DIV_3(color_b);
            return GRAPHICS_COLOR_RGB_8(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            color_r = GRAPHICS_RED_RGB_565(color) * 0xff; color_r = GRAPHICS_FAST_DIV_31(color_r);
            color_g = GRAPHICS_GREEN_RGB_565(color) * 0xff; color_g = GRAPHICS_FAST_DIV_63(color_g);
            color_b = GRAPHICS_BLUE_RGB_565(color) * 0xff; color_b = GRAPHICS_FAST_DIV_31(color_b);
            return GRAPHICS_COLOR_RGB_8(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            return color;
#endif
    }

    return 0;
}
#endif

/*
 * COLOR AS MASK.
 */

#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
static ALWAYS_INLINE graphics_color_t graphics_bw_1_apply_mask(graphics_color_t color, graphics_format_t mask_format, graphics_color_t mask)
{
    //if(color == 0 || mask == 0) return 0;
    switch(mask_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return color * mask;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            return (mask >= 0x2) ? color : 0x0;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            return (GRAPHICS_RGB_121_TO_GRAY(mask) >= (GRAPHICS_RGB_121_TO_GRAY_MAX >> 1)) ? color : 0x0;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            return (GRAPHICS_RGB_332_TO_GRAY(mask) >= (GRAPHICS_RGB_332_TO_GRAY_MAX >> 1)) ? color : 0x0;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            return (GRAPHICS_RGB_565_TO_GRAY(mask) >= (GRAPHICS_RGB_565_TO_GRAY_MAX >> 1)) ? color : 0x0;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            return (GRAPHICS_RGB_8_TO_GRAY(mask) >= (GRAPHICS_RGB_8_TO_GRAY_MAX >> 1)) ? color : 0x0;
#endif
    }

    return color;
}
#endif

#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
static ALWAYS_INLINE graphics_color_t graphics_gray_2_apply_mask(graphics_color_t color, graphics_format_t mask_format, graphics_color_t mask)
{
    //if(color == 0 || mask == 0) return 0;
    switch(mask_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return color * mask;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            color *= mask;
            return GRAPHICS_FAST_DIV_3(color);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            return GRAPHICS_RGB_121_TO_GRAY(mask) * color / GRAPHICS_RGB_121_TO_GRAY_MAX;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            return GRAPHICS_RGB_332_TO_GRAY(mask) * color / GRAPHICS_RGB_332_TO_GRAY_MAX;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            return GRAPHICS_RGB_565_TO_GRAY(mask) * color / GRAPHICS_RGB_565_TO_GRAY_MAX;
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            return GRAPHICS_RGB_8_TO_GRAY(mask) * color / GRAPHICS_RGB_8_TO_GRAY_MAX;
#endif
    }

    return color;
}
#endif

#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
static ALWAYS_INLINE graphics_color_t graphics_rgb_121_apply_mask(graphics_color_t color, graphics_format_t mask_format, graphics_color_t mask)
{
    //if(color == 0 || mask == 0) return 0;
    uint32_t color_r, color_g, color_b;
    switch(mask_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return color * mask;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            color_r = (GRAPHICS_RED_RGB_121(color) * (mask >> 1));
            color_g = (GRAPHICS_GREEN_RGB_121(color) * mask); color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = (GRAPHICS_BLUE_RGB_121(color) * (mask >> 1));
            return GRAPHICS_COLOR_RGB_121(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            color_r = GRAPHICS_RED_RGB_121(color) * GRAPHICS_RED_RGB_121(mask);
            color_g = GRAPHICS_GREEN_RGB_121(color) * GRAPHICS_GREEN_RGB_121(mask); color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = GRAPHICS_BLUE_RGB_121(color) * GRAPHICS_BLUE_RGB_121(mask);
            return GRAPHICS_COLOR_RGB_121(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            color_r = GRAPHICS_RED_RGB_121(color) * (GRAPHICS_RED_RGB_332(mask) >> 2);
            color_g = GRAPHICS_GREEN_RGB_121(color) * GRAPHICS_GREEN_RGB_332(mask); color_g = GRAPHICS_FAST_DIV_7(color_g);
            color_b = GRAPHICS_BLUE_RGB_121(color) * (GRAPHICS_BLUE_RGB_332(mask) >> 1);
            return GRAPHICS_COLOR_RGB_121(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            color_r = GRAPHICS_RED_RGB_121(color) * (GRAPHICS_RED_RGB_565(mask) >> 4);
            color_g = GRAPHICS_GREEN_RGB_121(color) * GRAPHICS_GREEN_RGB_565(mask); color_g = GRAPHICS_FAST_DIV_63(color_g);
            color_b = GRAPHICS_BLUE_RGB_121(color) * (GRAPHICS_BLUE_RGB_565(mask) >> 4);
            return GRAPHICS_COLOR_RGB_121(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            color_r = GRAPHICS_RED_RGB_121(color) * (GRAPHICS_RED_RGB_8(mask) >> 7);
            color_g = GRAPHICS_GREEN_RGB_121(color) * GRAPHICS_GREEN_RGB_8(mask); color_g = GRAPHICS_FAST_DIV_255(color_g);
            color_b = GRAPHICS_BLUE_RGB_121(color) * (GRAPHICS_BLUE_RGB_8(mask) >> 7);
            return GRAPHICS_COLOR_RGB_121(color_r, color_g, color_b);
#endif
    }

    return color;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
static ALWAYS_INLINE graphics_color_t graphics_rgb_332_apply_mask(graphics_color_t color, graphics_format_t mask_format, graphics_color_t mask)
{
    //if(color == 0 || mask == 0) return 0;
    uint32_t color_r, color_g, color_b;
    switch(mask_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return color * mask;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            color_r = (GRAPHICS_RED_RGB_332(color) * mask); color_r = GRAPHICS_FAST_DIV_3(color_r);
            color_g = (GRAPHICS_GREEN_RGB_332(color) * mask); color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = (GRAPHICS_BLUE_RGB_332(color) * mask); color_b = GRAPHICS_FAST_DIV_3(color_b);
            return GRAPHICS_COLOR_RGB_332(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            color_r = GRAPHICS_RED_RGB_332(color) * GRAPHICS_RED_RGB_121(mask);
            color_g = GRAPHICS_GREEN_RGB_332(color) * GRAPHICS_GREEN_RGB_121(mask); color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = GRAPHICS_BLUE_RGB_332(color) * GRAPHICS_BLUE_RGB_121(mask);
            return GRAPHICS_COLOR_RGB_332(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            color_r = GRAPHICS_RED_RGB_332(color) * GRAPHICS_RED_RGB_332(mask); color_r = GRAPHICS_FAST_DIV_7(color_r);
            color_g = GRAPHICS_GREEN_RGB_332(color) * GRAPHICS_GREEN_RGB_332(mask); color_g = GRAPHICS_FAST_DIV_7(color_g);
            color_b = GRAPHICS_BLUE_RGB_332(color) * GRAPHICS_BLUE_RGB_332(mask); color_b = GRAPHICS_FAST_DIV_3(color_b);
            return GRAPHICS_COLOR_RGB_332(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            color_r = GRAPHICS_RED_RGB_332(color) * GRAPHICS_RED_RGB_565(mask); color_r = GRAPHICS_FAST_DIV_31(color_r);
            color_g = GRAPHICS_GREEN_RGB_332(color) * GRAPHICS_GREEN_RGB_565(mask); color_g = GRAPHICS_FAST_DIV_63(color_g);
            color_b = GRAPHICS_BLUE_RGB_332(color) * GRAPHICS_BLUE_RGB_565(mask); color_b = GRAPHICS_FAST_DIV_31(color_b);
            return GRAPHICS_COLOR_RGB_332(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            color_r = GRAPHICS_RED_RGB_332(color) * GRAPHICS_RED_RGB_8(mask); color_r = GRAPHICS_FAST_DIV_255(color_r);
            color_g = GRAPHICS_GREEN_RGB_332(color) * GRAPHICS_GREEN_RGB_8(mask); color_g = GRAPHICS_FAST_DIV_255(color_g);
            color_b = GRAPHICS_BLUE_RGB_332(color) * GRAPHICS_BLUE_RGB_8(mask); color_b = GRAPHICS_FAST_DIV_255(color_b);
            return GRAPHICS_COLOR_RGB_332(color_r, color_g, color_b);
#endif
    }

    return color;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
static ALWAYS_INLINE graphics_color_t graphics_rgb_565_apply_mask(graphics_color_t color, graphics_format_t mask_format, graphics_color_t mask)
{
    //if(color == 0 || mask == 0) return 0;
    uint32_t color_r, color_g, color_b;
    switch(mask_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return color * mask;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            color_r = (GRAPHICS_RED_RGB_565(color) * mask); color_r = GRAPHICS_FAST_DIV_3(color_r);
            color_g = (GRAPHICS_GREEN_RGB_565(color) * mask); color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = (GRAPHICS_BLUE_RGB_565(color) * mask); color_b = GRAPHICS_FAST_DIV_3(color_b);
            return GRAPHICS_COLOR_RGB_565(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            color_r = GRAPHICS_RED_RGB_565(color) * GRAPHICS_RED_RGB_121(mask);
            color_g = GRAPHICS_GREEN_RGB_565(color) * GRAPHICS_GREEN_RGB_121(mask); color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = GRAPHICS_BLUE_RGB_565(color) * GRAPHICS_BLUE_RGB_121(mask);
            return GRAPHICS_COLOR_RGB_565(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            color_r = GRAPHICS_RED_RGB_565(color) * GRAPHICS_RED_RGB_332(mask); color_r = GRAPHICS_FAST_DIV_7(color_r);
            color_g = GRAPHICS_GREEN_RGB_565(color) * GRAPHICS_GREEN_RGB_332(mask); color_g = GRAPHICS_FAST_DIV_7(color_g);
            color_b = GRAPHICS_BLUE_RGB_565(color) * GRAPHICS_BLUE_RGB_332(mask); color_b = GRAPHICS_FAST_DIV_3(color_b);
            return GRAPHICS_COLOR_RGB_565(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            color_r = GRAPHICS_RED_RGB_565(color) * GRAPHICS_RED_RGB_565(mask); color_r = GRAPHICS_FAST_DIV_31(color_r);
            color_g = GRAPHICS_GREEN_RGB_565(color) * GRAPHICS_GREEN_RGB_565(mask); color_g = GRAPHICS_FAST_DIV_63(color_g);
            color_b = GRAPHICS_BLUE_RGB_565(color) * GRAPHICS_BLUE_RGB_565(mask); color_b = GRAPHICS_FAST_DIV_31(color_b);
            return GRAPHICS_COLOR_RGB_565(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            color_r = GRAPHICS_RED_RGB_565(color) * GRAPHICS_RED_RGB_8(mask); color_r = GRAPHICS_FAST_DIV_255(color_r);
            color_g = GRAPHICS_GREEN_RGB_565(color) * GRAPHICS_GREEN_RGB_8(mask); color_g = GRAPHICS_FAST_DIV_255(color_g);
            color_b = GRAPHICS_BLUE_RGB_565(color) * GRAPHICS_BLUE_RGB_8(mask); color_b = GRAPHICS_FAST_DIV_255(color_b);
            return GRAPHICS_COLOR_RGB_565(color_r, color_g, color_b);
#endif
    }

    return color;
}
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
static ALWAYS_INLINE graphics_color_t graphics_rgb_8_apply_mask(graphics_color_t color, graphics_format_t mask_format, graphics_color_t mask)
{
    //if(color == 0 || mask == 0) return 0;
    uint32_t color_r, color_g, color_b;
    switch(mask_format){
#ifdef USE_GRAPHICS_FORMAT_BW_1_V
        case GRAPHICS_FORMAT_BW_1_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_BW_1_H
        case GRAPHICS_FORMAT_BW_1_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_BW_1_V) || defined(USE_GRAPHICS_FORMAT_BW_1_H)
            return color * mask;
#endif

#ifdef USE_GRAPHICS_FORMAT_GRAY_2_V
        case GRAPHICS_FORMAT_GRAY_2_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_GRAY_2_H
        case GRAPHICS_FORMAT_GRAY_2_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_GRAY_2_V) || defined(USE_GRAPHICS_FORMAT_GRAY_2_H)
            color_r = (GRAPHICS_RED_RGB_8(color) * mask); color_r = GRAPHICS_FAST_DIV_3(color_r);
            color_g = (GRAPHICS_GREEN_RGB_8(color) * mask); color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = (GRAPHICS_BLUE_RGB_8(color) * mask); color_b = GRAPHICS_FAST_DIV_3(color_b);
            return GRAPHICS_COLOR_RGB_8(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_121_V
        case GRAPHICS_FORMAT_RGB_121_V:
#endif
#ifdef USE_GRAPHICS_FORMAT_RGB_121_H
        case GRAPHICS_FORMAT_RGB_121_H:
#endif
#if defined(USE_GRAPHICS_FORMAT_RGB_121_V) || defined(USE_GRAPHICS_FORMAT_RGB_121_H)
            color_r = GRAPHICS_RED_RGB_8(color) * GRAPHICS_RED_RGB_121(mask);
            color_g = GRAPHICS_GREEN_RGB_8(color) * GRAPHICS_GREEN_RGB_121(mask); color_g = GRAPHICS_FAST_DIV_3(color_g);
            color_b = GRAPHICS_BLUE_RGB_8(color) * GRAPHICS_BLUE_RGB_121(mask);
            return GRAPHICS_COLOR_RGB_8(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_332
        case GRAPHICS_FORMAT_RGB_332:
            color_r = GRAPHICS_RED_RGB_8(color) * GRAPHICS_RED_RGB_332(mask); color_r = GRAPHICS_FAST_DIV_7(color_r);
            color_g = GRAPHICS_GREEN_RGB_8(color) * GRAPHICS_GREEN_RGB_332(mask); color_g = GRAPHICS_FAST_DIV_7(color_g);
            color_b = GRAPHICS_BLUE_RGB_8(color) * GRAPHICS_BLUE_RGB_332(mask); color_b = GRAPHICS_FAST_DIV_3(color_b);
            return GRAPHICS_COLOR_RGB_8(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_565
        case GRAPHICS_FORMAT_RGB_565:
            color_r = GRAPHICS_RED_RGB_8(color) * GRAPHICS_RED_RGB_565(mask); color_r = GRAPHICS_FAST_DIV_31(color_r);
            color_g = GRAPHICS_GREEN_RGB_8(color) * GRAPHICS_GREEN_RGB_565(mask); color_g = GRAPHICS_FAST_DIV_63(color_g);
            color_b = GRAPHICS_BLUE_RGB_8(color) * GRAPHICS_BLUE_RGB_565(mask); color_b = GRAPHICS_FAST_DIV_31(color_b);
            return GRAPHICS_COLOR_RGB_8(color_r, color_g, color_b);
#endif

#ifdef USE_GRAPHICS_FORMAT_RGB_8
        case GRAPHICS_FORMAT_RGB_8:
            color_r = GRAPHICS_RED_RGB_8(color) * GRAPHICS_RED_RGB_8(mask); color_r = GRAPHICS_FAST_DIV_255(color_r);
            color_g = GRAPHICS_GREEN_RGB_8(color) * GRAPHICS_GREEN_RGB_8(mask); color_g = GRAPHICS_FAST_DIV_255(color_g);
            color_b = GRAPHICS_BLUE_RGB_8(color) * GRAPHICS_BLUE_RGB_8(mask); color_b = GRAPHICS_FAST_DIV_255(color_b);
            return GRAPHICS_COLOR_RGB_8(color_r, color_g, color_b);
#endif
    }

    return color;
}
#endif


/*
static ALWAYS_INLINE void graphics_bw_1_v_
static ALWAYS_INLINE void graphics_bw_1_h_
static ALWAYS_INLINE void graphics_gray_2_v_
static ALWAYS_INLINE void graphics_gray_2_h_
static ALWAYS_INLINE void graphics_rgb_121_v_
static ALWAYS_INLINE void graphics_rgb_121_h_
static ALWAYS_INLINE void graphics_rgb_332_
static ALWAYS_INLINE void graphics_rgb_565_
static ALWAYS_INLINE void graphics_rgb_8_
 */


#endif  //_GRAPHICS_INLINE_H_
