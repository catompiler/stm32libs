#include "font.h"

// ютф8!

err_t font_bitmap_init(font_bitmap_t* font_bitmap, font_char_t first_char, font_char_t last_char,
                       const uint8_t* data, graphics_size_t width, graphics_size_t height, graphics_format_t format)
{
    font_bitmap->first_char = first_char;
    font_bitmap->last_char = last_char;

    return graphics_init((graphics_t*)&font_bitmap->graphics, (uint8_t*)data, width, height, format);
}

err_t font_init(font_t* font, const font_bitmap_t* bitmaps, size_t bitmaps_count,
                graphics_size_t char_width, graphics_size_t char_height, graphics_pos_t hspace, graphics_pos_t vspace)
{
    if(bitmaps == NULL) return E_NULL_POINTER;
    if(bitmaps_count == 0) return E_INVALID_VALUE;

    font->bitmaps = bitmaps;
    font->bitmaps_count = bitmaps_count;
    font->char_width = char_width;
    font->char_height = char_height;
    font->hspace = hspace;
    font->vspace = vspace;

    return E_NO_ERROR;
}

const font_bitmap_t* font_bitmap_by_char(const font_t* font, font_char_t c)
{
    const font_bitmap_t* font_bitmap;

    size_t i = 0;

    for(; i < font->bitmaps_count; i ++){

        font_bitmap = &font->bitmaps[i];

        if(c < font_bitmap_first_char(font_bitmap)) break;
        if(c > font_bitmap_last_char(font_bitmap)) continue;

        return font_bitmap;
    }

    return NULL;
}

bool font_bitmap_get_char_coords(const font_t* font, const font_bitmap_t* font_bitmap, font_char_t c, graphics_pos_t* x, graphics_pos_t* y)
{
    if(font_bitmap == NULL) return false;
    if(c < font_bitmap_first_char(font_bitmap) || c > font_bitmap_last_char(font_bitmap)) return false;
    if(x == NULL && y == NULL) return false;

    size_t char_pos = (c - font_bitmap_first_char(font_bitmap)) * font_char_width(font);

    if(x){
        *x = char_pos % graphics_width(font_bitmap_graphics(font_bitmap));
    }
    if(y){
        *y = char_pos / graphics_width(font_bitmap_graphics(font_bitmap));
    }

    return true;
}

bool font_get_char_bitmap(const font_t* font, font_char_t c, const font_bitmap_t** font_bitmap, graphics_pos_t* x, graphics_pos_t* y)
{
    const font_bitmap_t* fbmp = font_bitmap_by_char(font, c);
    
    if(!fbmp){
        c = font_default_char(font);
        if(!(fbmp = font_bitmap_by_char(font, c))) return false;
    }
    
    graphics_pos_t cx = 0, cy = 0;
    
    if(!font_bitmap_get_char_coords(font, fbmp, c, &cx, &cy)) return false;
    
    *font_bitmap = fbmp;
    *x = cx;
    *y = cy;
    
    return true;
}

size_t font_utf8_size(const char* c)
{
    unsigned char first_c = *c;

    if(first_c < 0x80) return 1;

    size_t res = 0;

    while(first_c & 0x80){
        res ++;
        first_c <<= 1;
    }

    // Нет поддержки символов длиной более 4 байт.
    if(res > 4) res = 1;

    return res;
}

/*
   0000 0000-0000 007F   0xxxxxxx
   0000 0080-0000 07FF   110xxxxx 10xxxxxx
   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
font_char_t font_utf8_decode(const char* c, size_t* c_size)
{
    font_char_t res = 0;
    size_t s = font_utf8_size(c);
    switch(s){
        default:
        case 1:
            res = (font_char_t)(*c);
            break;
        case 2:
            res = (res | (c[0] & 0x1f)) << 6;
            res = (res | (c[1] & 0x3f));
            break;
        case 3:
            res = (res | (c[0] & 0xf )) << 6;
            res = (res | (c[1] & 0x3f)) << 6;
            res = (res | (c[2] & 0x3f));
            break;
        case 4:
            res = (res | (c[0] & 0x7 )) << 6;
            res = (res | (c[1] & 0x3f)) << 6;
            res = (res | (c[2] & 0x3f)) << 6;
            res = (res | (c[3] & 0x3f));
            break;
    }

    if(c_size) *c_size = s;

    return res;
}

