#include "utf8.h"



size_t utf8_str_char_size(const char* str_c)
{
    size_t res = 0;
    unsigned char c = str_c[0];
    
    while(c & 0x80){
        res ++;
        c <<= 0x1;
    }
    
    return res == 0 ? 1 : res;
}

size_t utf8_char_size(wchar_t c)
{
    if(c > 0x001fffff) return 0;
    
    if(c >= 0x00010000) return 4;
    if(c >= 0x00000800) return 3;
    if(c >= 0x00000080) return 2;
    
    return 1;
}

/*
   0000 0000-0000 007F   0xxxxxxx
   0000 0080-0000 07FF   110xxxxx 10xxxxxx
   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 */
wchar_t utf8_char_decode(const char* str_c)
{
    wchar_t res = 0;
    size_t s = utf8_str_char_size(str_c);

    switch(s){
        case 1:
        default:
            res = (wchar_t)(*str_c);
            break;
        case 2:
            res = (res | (str_c[0] & 0x1f)) << 6;
            res = (res | (str_c[1] & 0x3f));
            break;
        case 3:
            res = (res | (str_c[0] & 0xf )) << 6;
            res = (res | (str_c[1] & 0x3f)) << 6;
            res = (res | (str_c[2] & 0x3f));
            break;
        case 4:
            res = (res | (str_c[0] & 0x7 )) << 6;
            res = (res | (str_c[1] & 0x3f)) << 6;
            res = (res | (str_c[2] & 0x3f)) << 6;
            res = (res | (str_c[3] & 0x3f));
            break;
    }
    
    return res;
}

size_t utf8_char_encode(char* str_c, wchar_t c)
{
    size_t s = utf8_char_size(c);
    
    switch(s){
        case 1:
        default:
            *str_c = (char)(c);
            break;
        case 2://0x1f 0x3f <<6
            str_c[0] = (char)(((c >> 6 )         | 0xc0));
            str_c[1] = (char)(((c      ) & 0x3f) | 0x80);
            break;
        case 3://0xf
            str_c[0] = (char)(((c >> 12)         | 0xe0));
            str_c[1] = (char)(((c >> 6 ) & 0x3f) | 0x80);
            str_c[2] = (char)(((c      ) & 0x3f) | 0x80);
            break;
        case 4://0x7
            str_c[0] = (char)(((c >> 18)         | 0xf0));
            str_c[1] = (char)(((c >> 12) & 0x3f) | 0x80);
            str_c[2] = (char)(((c >> 6 ) & 0x3f) | 0x80);
            str_c[3] = (char)(((c      ) & 0x3f) | 0x80);
            break;
    }
    
    return s;
}

bool utf8_char_validate(const char* str_c)
{
    size_t s = utf8_str_char_size(str_c);
    
    if(s > 4) return false;
    
    switch(s){
        case 1:
            if(str_c[0] & 0x80) return false;
            break;
        case 2:
            if((str_c[0] & 0xe0) != 0xc0) return false;
            if((str_c[1] & 0xc0) != 0x80) return false;
            break;
        case 3:
            if((str_c[0] & 0xf0) != 0xe0) return false;
            if((str_c[1] & 0xc0) != 0x80) return false;
            if((str_c[2] & 0xc0) != 0x80) return false;
            break;
        case 4:
            if((str_c[0] & 0xf8) != 0xf0) return false;
            if((str_c[1] & 0xc0) != 0x80) return false;
            if((str_c[2] & 0xc0) != 0x80) return false;
            if((str_c[3] & 0xc0) != 0x80) return false;
            break;
        default:
            return false;
    }
    
    return true;
}
