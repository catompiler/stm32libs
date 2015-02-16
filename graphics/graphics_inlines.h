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

static ALWAYS_INLINE void graphics_bw_1_v_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = x + (y >> 3) * graphics->width;
    if(bit)  *bit  = y & 0x7;
}

static ALWAYS_INLINE void graphics_bw_1_h_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = (x >> 3) + y * (graphics->width >> 3);
    if(bit)  *bit  = x & 0x7;
}

static ALWAYS_INLINE void graphics_gray_2_v_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = x + (y >> 2) * graphics->width;
    if(bit)  *bit  = (y & 0x3) << 1;
}

static ALWAYS_INLINE void graphics_gray_2_h_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = (x >> 2) + y * (graphics->width >> 2);
    if(bit)  *bit  = (x & 0x3) << 1;
}

static ALWAYS_INLINE void graphics_rgb_121_v_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = x + (y >> 1) * graphics->width;
    if(bit)  *bit  = (y & 0x1) << 2;
}

static ALWAYS_INLINE void graphics_rgb_121_h_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = (x >> 1) + y * (graphics->width >> 1);
    if(bit)  *bit  = (x & 0x1) << 2;
}

static ALWAYS_INLINE void graphics_rgb_332_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = x + y * graphics->width;
    if(bit)  *bit  = 0;
}

static ALWAYS_INLINE void graphics_rgb_565_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = (x + y * graphics->width) * 2;
    if(bit)  *bit  = 0;
}

static ALWAYS_INLINE void graphics_rgb_8_get_pixel_pos(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_size_t* byte, graphics_size_t* bit)
{
    if(byte) *byte = (x + y * graphics->width) * 3;
    if(bit)  *bit  = 0;
}

/*
 * GET.
 */

static ALWAYS_INLINE graphics_color_t graphics_bw_1_v_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0x1;
}

static ALWAYS_INLINE graphics_color_t graphics_bw_1_h_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0x1;
}

static ALWAYS_INLINE graphics_color_t graphics_gray_2_v_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0x3;
}

static ALWAYS_INLINE graphics_color_t graphics_gray_2_h_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0x3;
}

static ALWAYS_INLINE graphics_color_t graphics_rgb_121_v_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0xf;
}

static ALWAYS_INLINE graphics_color_t graphics_rgb_121_h_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    return (graphics->data[byte] >> bit) & 0xf;
}

static ALWAYS_INLINE graphics_color_t graphics_rgb_332_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_size_t byte = 0;

    graphics_rgb_332_get_pixel_pos(graphics, x, y, &byte, NULL);

    return graphics->data[byte];
}

static ALWAYS_INLINE graphics_color_t graphics_rgb_565_get_pixel(const graphics_t* graphics, graphics_pos_t x, graphics_pos_t y)
{
    graphics_color_t color = 0;
    graphics_size_t byte = 0;

    graphics_rgb_565_get_pixel_pos(graphics, x, y, &byte, NULL);

    color = (graphics->data[byte]) |
            (graphics->data[byte + 1] << 8);

    return color;
}

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

/*
 * SET.
 */

static ALWAYS_INLINE void graphics_bw_1_v_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0x1 << bit);
    graphics->data[byte] = c | ((color & 0x1) << bit);
}

static ALWAYS_INLINE void graphics_bw_1_h_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0x1 << bit);
    graphics->data[byte] = c | ((color & 0x1) << bit);
}

static ALWAYS_INLINE void graphics_gray_2_v_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0x3 << bit);
    graphics->data[byte] = c | ((color & 0x3) << bit);
}

static ALWAYS_INLINE void graphics_gray_2_h_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0x3 << bit);
    graphics->data[byte] = c | ((color & 0x3) << bit);
}

static ALWAYS_INLINE void graphics_rgb_121_v_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0xf << bit);
    graphics->data[byte] = c | ((color & 0xf) << bit);
}

static ALWAYS_INLINE void graphics_rgb_121_h_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    uint8_t c = graphics->data[byte] & ~(0xf << bit);
    graphics->data[byte] = c | ((color & 0xf) << bit);
}

static ALWAYS_INLINE void graphics_rgb_332_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_332_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte] = color & 0xff;
}

static ALWAYS_INLINE void graphics_rgb_565_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_565_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     = color & 0xff;
    graphics->data[byte + 1] = (color >> 8) & 0xff;
}

static ALWAYS_INLINE void graphics_rgb_8_set_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_8_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     = color & 0xff;
    graphics->data[byte + 1] = (color >> 8) & 0xff;
    graphics->data[byte + 2] = (color >> 16) & 0xff;
}

/*
 * OR.
 */

static ALWAYS_INLINE void graphics_bw_1_v_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color & 0x1) << bit;
}

static ALWAYS_INLINE void graphics_bw_1_h_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color & 0x1) << bit;
}

static ALWAYS_INLINE void graphics_gray_2_v_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color & 0x3) << bit;
}

static ALWAYS_INLINE void graphics_gray_2_h_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color & 0x3) << bit;
}

static ALWAYS_INLINE void graphics_rgb_121_v_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color & 0xf) << bit;
}

static ALWAYS_INLINE void graphics_rgb_121_h_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] |= (color & 0xf) << bit;
}

static ALWAYS_INLINE void graphics_rgb_332_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_332_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte] |= color & 0xff;
}

static ALWAYS_INLINE void graphics_rgb_565_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_565_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     |= color & 0xff;
    graphics->data[byte + 1] |= (color >> 8) & 0xff;
}

static ALWAYS_INLINE void graphics_rgb_8_or_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_8_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     |= color & 0xff;
    graphics->data[byte + 1] |= (color >> 8) & 0xff;
    graphics->data[byte + 2] |= (color >> 16) & 0xff;
}

/*
 * XOR.
 */

static ALWAYS_INLINE void graphics_bw_1_v_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color & 0x1) << bit;
}

static ALWAYS_INLINE void graphics_bw_1_h_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color & 0x1) << bit;
}

static ALWAYS_INLINE void graphics_gray_2_v_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color & 0x3) << bit;
}

static ALWAYS_INLINE void graphics_gray_2_h_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color & 0x3) << bit;
}

static ALWAYS_INLINE void graphics_rgb_121_v_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color & 0xf) << bit;
}

static ALWAYS_INLINE void graphics_rgb_121_h_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] ^= (color & 0xf) << bit;
}

static ALWAYS_INLINE void graphics_rgb_332_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_332_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte] ^= color & 0xff;
}

static ALWAYS_INLINE void graphics_rgb_565_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_565_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     ^= color & 0xff;
    graphics->data[byte + 1] ^= (color >> 8) & 0xff;
}

static ALWAYS_INLINE void graphics_rgb_8_xor_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_8_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     ^= color & 0xff;
    graphics->data[byte + 1] ^= (color >> 8) & 0xff;
    graphics->data[byte + 2] ^= (color >> 16) & 0xff;
}

/*
 * AND.
 */

static ALWAYS_INLINE void graphics_bw_1_v_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color & 0x1) << bit) | (~(1 << bit));
}

static ALWAYS_INLINE void graphics_bw_1_h_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_bw_1_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color & 0x1) << bit) | (~(1 << bit));
}

static ALWAYS_INLINE void graphics_gray_2_v_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color & 0x3) << bit) | (~(0x3 << bit));
}

static ALWAYS_INLINE void graphics_gray_2_h_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_gray_2_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color & 0x3) << bit) | (~(0x3 << bit));
}

static ALWAYS_INLINE void graphics_rgb_121_v_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_v_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color & 0xf) << bit) | (~(0xf << bit));
}

static ALWAYS_INLINE void graphics_rgb_121_h_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0, bit = 0;

    graphics_rgb_121_h_get_pixel_pos(graphics, x, y, &byte, &bit);

    graphics->data[byte] &= ((color & 0xf) << bit) | (~(0xf << bit));
}

static ALWAYS_INLINE void graphics_rgb_332_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_332_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte] &= color & 0xff;
}

static ALWAYS_INLINE void graphics_rgb_565_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_565_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     &= color & 0xff;
    graphics->data[byte + 1] &= (color >> 8) & 0xff;
}

static ALWAYS_INLINE void graphics_rgb_8_and_pixel(graphics_t* graphics, graphics_pos_t x, graphics_pos_t y, graphics_color_t color)
{
    graphics_size_t byte = 0;

    graphics_rgb_8_get_pixel_pos(graphics, x, y, &byte, NULL);

    graphics->data[byte]     &= color & 0xff;
    graphics->data[byte + 1] &= (color >> 8) & 0xff;
    graphics->data[byte + 2] &= (color >> 16) & 0xff;
}

/*
 * FROM COLOR.
 */
static ALWAYS_INLINE graphics_color_t graphics_bw_1_color_from(graphics_format_t from_format, graphics_color_t color)
{
    switch(from_format){
        case GRAPHICS_FORMAT_BW_1_V:
        case GRAPHICS_FORMAT_BW_1_H:
            return color;

        case GRAPHICS_FORMAT_GRAY_2_V:
        case GRAPHICS_FORMAT_GRAY_2_H:
            return color >= 0x2;

        case GRAPHICS_FORMAT_RGB_121_V:
        case GRAPHICS_FORMAT_RGB_121_H:
            return GRAPHICS_RGB_121_TO_GRAY(color) >= (GRAPHICS_RGB_121_TO_GRAY_MAX / 2);

        case GRAPHICS_FORMAT_RGB_332:
            return GRAPHICS_RGB_332_TO_GRAY(color) >= (GRAPHICS_RGB_332_TO_GRAY_MAX / 2);

        case GRAPHICS_FORMAT_RGB_565:
            return GRAPHICS_RGB_565_TO_GRAY(color) >= (GRAPHICS_RGB_565_TO_GRAY_MAX / 2);

        case GRAPHICS_FORMAT_RGB_8:
            return GRAPHICS_RGB_8_TO_GRAY(color) >= (GRAPHICS_RGB_8_TO_GRAY_MAX / 2);
    }

    return 0;
}

static ALWAYS_INLINE graphics_color_t graphics_gray_2_color_from(graphics_format_t from_format, graphics_color_t color)
{
    switch(from_format){
        case GRAPHICS_FORMAT_BW_1_V:
        case GRAPHICS_FORMAT_BW_1_H:
            return (color == 0x0) ? 0x0 : 0x3;

        case GRAPHICS_FORMAT_GRAY_2_V:
        case GRAPHICS_FORMAT_GRAY_2_H:
            return color;

        case GRAPHICS_FORMAT_RGB_121_V:
        case GRAPHICS_FORMAT_RGB_121_H:
            return GRAPHICS_RGB_121_TO_GRAY(color) * 0x3 / GRAPHICS_RGB_121_TO_GRAY_MAX;

        case GRAPHICS_FORMAT_RGB_332:
            return GRAPHICS_RGB_332_TO_GRAY(color) * 0x3 / GRAPHICS_RGB_332_TO_GRAY_MAX;

        case GRAPHICS_FORMAT_RGB_565:
            return GRAPHICS_RGB_565_TO_GRAY(color) * 0x3 / GRAPHICS_RGB_565_TO_GRAY_MAX;

        case GRAPHICS_FORMAT_RGB_8:
            return GRAPHICS_RGB_8_TO_GRAY(color) * 0x3 / GRAPHICS_RGB_8_TO_GRAY_MAX;
    }

    return 0;
}

static ALWAYS_INLINE graphics_color_t graphics_rgb_121_color_from(graphics_format_t from_format, graphics_color_t color)
{
    switch(from_format){
        case GRAPHICS_FORMAT_BW_1_V:
        case GRAPHICS_FORMAT_BW_1_H:
            return (color == 0x0) ? 0x0 : 0xf;

        case GRAPHICS_FORMAT_GRAY_2_V:
        case GRAPHICS_FORMAT_GRAY_2_H:
            color &= 0x3;
            return GRAPHICS_COLOR_RGB_121(color > 0x1, color, color > 0x1);

        case GRAPHICS_FORMAT_RGB_121_V:
        case GRAPHICS_FORMAT_RGB_121_H:
            return color;

        case GRAPHICS_FORMAT_RGB_332:
            return GRAPHICS_COLOR_RGB_121(GRAPHICS_RED_RGB_332(color) > 0x3, GRAPHICS_GREEN_RGB_332(color) * 0x3 / 0x7, GRAPHICS_BLUE_RGB_332(color) > 0x3);

        case GRAPHICS_FORMAT_RGB_565:
            return GRAPHICS_COLOR_RGB_121(GRAPHICS_RED_RGB_565(color) > 0xf, GRAPHICS_GREEN_RGB_565(color) * 0x3 / 0x3f, GRAPHICS_BLUE_RGB_565(color) > 0xf);

        case GRAPHICS_FORMAT_RGB_8:
            return GRAPHICS_COLOR_RGB_121(GRAPHICS_RED_RGB_8(color) > 0x7f, GRAPHICS_GREEN_RGB_8(color) * 0x3 / 0xff, GRAPHICS_BLUE_RGB_8(color) > 0x7f);
    }

    return 0;
}

static ALWAYS_INLINE graphics_color_t graphics_rgb_332_color_from(graphics_format_t from_format, graphics_color_t color)
{
    switch(from_format){
        case GRAPHICS_FORMAT_BW_1_V:
        case GRAPHICS_FORMAT_BW_1_H:
            return (color == 0x0) ? 0x0 : 0xff;

        case GRAPHICS_FORMAT_GRAY_2_V:
        case GRAPHICS_FORMAT_GRAY_2_H:{
            color &= 0x3;
            uint32_t c = color * 0x7 / 0x3;
            return GRAPHICS_COLOR_RGB_332(c, c, color);
        }

        case GRAPHICS_FORMAT_RGB_121_V:
        case GRAPHICS_FORMAT_RGB_121_H:
            return GRAPHICS_COLOR_RGB_332(GRAPHICS_RED_RGB_121(color) * 0x7, GRAPHICS_GREEN_RGB_121(color) * 0x7 / 0x3, GRAPHICS_BLUE_RGB_121(color) * 0x3);

        case GRAPHICS_FORMAT_RGB_332:
            return color;

        case GRAPHICS_FORMAT_RGB_565:
            return GRAPHICS_COLOR_RGB_332(GRAPHICS_RED_RGB_565(color) * 0x7 / 0x1f, GRAPHICS_GREEN_RGB_565(color) * 0x7 / 0x3f, GRAPHICS_BLUE_RGB_565(color) * 0x3 / 0x1f);

        case GRAPHICS_FORMAT_RGB_8:
            return GRAPHICS_COLOR_RGB_332(GRAPHICS_RED_RGB_8(color) * 0x7 / 0xff, GRAPHICS_GREEN_RGB_8(color) * 0x7 / 0xff, GRAPHICS_BLUE_RGB_8(color) * 0x3 / 0xff);
    }

    return 0;
}

static ALWAYS_INLINE graphics_color_t graphics_rgb_565_color_from(graphics_format_t from_format, graphics_color_t color)
{
    switch(from_format){
        case GRAPHICS_FORMAT_BW_1_V:
        case GRAPHICS_FORMAT_BW_1_H:
            return (color == 0x0) ? 0x0 : 0xffff;

        case GRAPHICS_FORMAT_GRAY_2_V:
        case GRAPHICS_FORMAT_GRAY_2_H:{
            color &= 0x3;
            uint32_t c = color * 0x1f / 0x3;
            return GRAPHICS_COLOR_RGB_565(c, color * 0x3f / 0x3, c);
        }

        case GRAPHICS_FORMAT_RGB_121_V:
        case GRAPHICS_FORMAT_RGB_121_H:
            return GRAPHICS_COLOR_RGB_565(GRAPHICS_RED_RGB_121(color) * 0x1f, GRAPHICS_GREEN_RGB_121(color) * 0x3f / 0x3, GRAPHICS_BLUE_RGB_121(color) * 0x1f);

        case GRAPHICS_FORMAT_RGB_332:
            return GRAPHICS_COLOR_RGB_565(GRAPHICS_RED_RGB_332(color) * 0x1f / 0x7, GRAPHICS_GREEN_RGB_332(color) * 0x3f / 0x7, GRAPHICS_BLUE_RGB_332(color) * 0x1f / 0x3);

        case GRAPHICS_FORMAT_RGB_565:
            return color;

        case GRAPHICS_FORMAT_RGB_8:
            return GRAPHICS_COLOR_RGB_565(GRAPHICS_RED_RGB_8(color) * 0x1f / 0xff, GRAPHICS_GREEN_RGB_8(color) * 0x3f / 0xff, GRAPHICS_BLUE_RGB_8(color) * 0x1f / 0xff);
    }

    return 0;
}

static ALWAYS_INLINE graphics_color_t graphics_rgb_8_color_from(graphics_format_t from_format, graphics_color_t color)
{
    switch(from_format){
        case GRAPHICS_FORMAT_BW_1_V:
        case GRAPHICS_FORMAT_BW_1_H:
            return (color == 0x0) ? 0x0 : 0x00ffffff;

        case GRAPHICS_FORMAT_GRAY_2_V:
        case GRAPHICS_FORMAT_GRAY_2_H:{
            color = (color & 0x3) * 0xff / 0x3;
            return GRAPHICS_COLOR_RGB_565(color, color, color);
        }

        case GRAPHICS_FORMAT_RGB_121_V:
        case GRAPHICS_FORMAT_RGB_121_H:
            return GRAPHICS_COLOR_RGB_8(GRAPHICS_RED_RGB_121(color) * 0xff, GRAPHICS_GREEN_RGB_121(color) * 0xff / 0x3, GRAPHICS_BLUE_RGB_121(color) * 0xff);

        case GRAPHICS_FORMAT_RGB_332:
            return GRAPHICS_COLOR_RGB_8(GRAPHICS_RED_RGB_332(color) * 0xff / 0x7, GRAPHICS_GREEN_RGB_332(color) * 0xff / 0x7, GRAPHICS_BLUE_RGB_332(color) * 0xff / 0x3);

        case GRAPHICS_FORMAT_RGB_565:
            return GRAPHICS_COLOR_RGB_8(GRAPHICS_RED_RGB_565(color) * 0xff / 0x1f, GRAPHICS_GREEN_RGB_565(color) * 0xff / 0x3f, GRAPHICS_BLUE_RGB_565(color) * 0xff / 0x1f);

        case GRAPHICS_FORMAT_RGB_8:
            return color;
    }

    return 0;
}


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
