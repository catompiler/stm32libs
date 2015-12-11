/**
 * @file keys.h Константы кодов кнопок клавиатуры.
 */

#ifndef KEYS_H
#define KEYS_H

#include <stdint.h>

typedef char charcode_t;

//! Максимальное значение кода ascii символа.
#define CHARCODE_MAX 0x7f

//! Проверяет символ на входжение в первые 127 символов (ascii)
#define IS_CHARCODE(c) ((unsigned)c <= CHARCODE_MAX)

// ASCII.
#define CHARCODE_NUL    0x0 // 0 - '\x00'
#define CHARCODE_SOH    0x1 // 1 - '\x01'
#define CHARCODE_STX    0x2 // 2 - '\x02'
#define CHARCODE_ETX    0x3 // 3 - '\x03'
#define CHARCODE_EOT    0x4 // 4 - '\x04'
#define CHARCODE_ENO    0x5 // 5 - '\x05'
#define CHARCODE_ACK    0x6 // 6 - '\x06'
#define CHARCODE_BEL    0x7 // 7 - '\x07'
#define CHARCODE_BS     0x8 // 8 - '\x08'
#define CHARCODE_CT     0x9 // 9 - '\x09'
#define CHARCODE_LF     0xa // 10 - '\x0a'
#define CHARCODE_VT     0xb // 11 - '\x0b'
#define CHARCODE_FF     0xc // 12 - '\x0c'
#define CHARCODE_CR     0xd // 13 - '\x0d'
#define CHARCODE_SO     0xe // 14 - '\x0e'
#define CHARCODE_SI     0xf // 15 - '\x0f'
#define CHARCODE_DLE    0x10 // 16 - '\x10'
#define CHARCODE_DC1    0x11 // 17 - '\x11'
#define CHARCODE_DC2    0x12 // 18 - '\x12'
#define CHARCODE_DC3    0x13 // 19 - '\x13'
#define CHARCODE_DC4    0x14 // 20 - '\x14'
#define CHARCODE_NAK    0x15 // 21 - '\x15'
#define CHARCODE_SYN    0x16 // 22 - '\x16'
#define CHARCODE_ETB    0x17 // 23 - '\x17'
#define CHARCODE_CAN    0x18 // 24 - '\x18'
#define CHARCODE_EM     0x19 // 25 - '\x19'
#define CHARCODE_SUB    0x1a // 26 - '\x1a'
#define CHARCODE_ESC    0x1b // 27 - '\x1b'
#define CHARCODE_FS     0x1c // 28 - '\x1c'
#define CHARCODE_GS     0x1d // 29 - '\x1d'
#define CHARCODE_RS     0x1e // 30 - '\x1e'
#define CHARCODE_US     0x1f // 31 - '\x1f'
#define CHARCODE_SPACE      0x20 // 32 - ' '
#define CHARCODE_EXCLAM     0x21 // 33 - '!'
#define CHARCODE_QUOTE      0x22 // 34 - '"'
#define CHARCODE_NUMBER     0x23 // 35 - '#'
#define CHARCODE_DOLLAR     0x24 // 36 - '$'
#define CHARCODE_PERCENT    0x25 // 37 - '%'
#define CHARCODE_AMPERSAND  0x26 // 38 - '&'
#define CHARCODE_APOSTROPHE 0x27 // 39 - '''
#define CHARCODE_LPARENTHESES   0x28 // 40 - '('
#define CHARCODE_RPARENTHESES   0x29 // 41 - ')'
#define CHARCODE_ASTERISK   0x2a // 42 - '*'
#define CHARCODE_PLUS       0x2b // 43 - '+'
#define CHARCODE_COMMA      0x2c // 44 - ','
#define CHARCODE_MINUS      0x2d // 45 - '-'
#define CHARCODE_DOT        0x2e // 46 - '.'
#define CHARCODE_SLASH      0x2f // 47 - '/'
#define CHARCODE_0    0x30 // 48 - '0'
#define CHARCODE_1    0x31 // 49 - '1'
#define CHARCODE_2    0x32 // 50 - '2'
#define CHARCODE_3    0x33 // 51 - '3'
#define CHARCODE_4    0x34 // 52 - '4'
#define CHARCODE_5    0x35 // 53 - '5'
#define CHARCODE_6    0x36 // 54 - '6'
#define CHARCODE_7    0x37 // 55 - '7'
#define CHARCODE_8    0x38 // 56 - '8'
#define CHARCODE_9    0x39 // 57 - '9'
#define CHARCODE_COLON      0x3a // 58 - ':'
#define CHARCODE_SEMICOLON  0x3b // 59 - ';'
#define CHARCODE_LESS       0x3c // 60 - '<'
#define CHARCODE_EQUAL      0x3d // 61 - '='
#define CHARCODE_GREATER    0x3e // 62 - '>'
#define CHARCODE_QUESTION   0x3f // 63 - '?'
#define CHARCODE_AT   0x40 // 64 - '@'
#define CHARCODE_A    0x41 // 65 - 'A'
#define CHARCODE_B    0x42 // 66 - 'B'
#define CHARCODE_C    0x43 // 67 - 'C'
#define CHARCODE_D    0x44 // 68 - 'D'
#define CHARCODE_E    0x45 // 69 - 'E'
#define CHARCODE_F    0x46 // 70 - 'F'
#define CHARCODE_G    0x47 // 71 - 'G'
#define CHARCODE_H    0x48 // 72 - 'H'
#define CHARCODE_I    0x49 // 73 - 'I'
#define CHARCODE_J    0x4a // 74 - 'J'
#define CHARCODE_K    0x4b // 75 - 'K'
#define CHARCODE_L    0x4c // 76 - 'L'
#define CHARCODE_M    0x4d // 77 - 'M'
#define CHARCODE_N    0x4e // 78 - 'N'
#define CHARCODE_O    0x4f // 79 - 'O'
#define CHARCODE_P    0x50 // 80 - 'P'
#define CHARCODE_Q    0x51 // 81 - 'Q'
#define CHARCODE_R    0x52 // 82 - 'R'
#define CHARCODE_S    0x53 // 83 - 'S'
#define CHARCODE_T    0x54 // 84 - 'T'
#define CHARCODE_U    0x55 // 85 - 'U'
#define CHARCODE_V    0x56 // 86 - 'V'
#define CHARCODE_W    0x57 // 87 - 'W'
#define CHARCODE_X    0x58 // 88 - 'X'
#define CHARCODE_Y    0x59 // 89 - 'Y'
#define CHARCODE_Z    0x5a // 90 - 'Z'
#define CHARCODE_LSQBRACKET     0x5b // 91 - '['
#define CHARCODE_BACKSLASH      0x5c // 92 - '\'
#define CHARCODE_RSQBRACKET     0x5d // 93 - ']'
#define CHARCODE_CARET          0x5e // 94 - '^'
#define CHARCODE_UNDERSCORE     0x5f // 95 - '_'
#define CHARCODE_GRAVEACCENT    0x60 // 96 - '`'
#define CHARCODE_a   0x61 // 97 - 'a'
#define CHARCODE_b   0x62 // 98 - 'b'
#define CHARCODE_c   0x63 // 99 - 'c'
#define CHARCODE_d   0x64 // 100 - 'd'
#define CHARCODE_e   0x65 // 101 - 'e'
#define CHARCODE_f   0x66 // 102 - 'f'
#define CHARCODE_g   0x67 // 103 - 'g'
#define CHARCODE_h   0x68 // 104 - 'h'
#define CHARCODE_i   0x69 // 105 - 'i'
#define CHARCODE_j   0x6a // 106 - 'j'
#define CHARCODE_k   0x6b // 107 - 'k'
#define CHARCODE_l   0x6c // 108 - 'l'
#define CHARCODE_m   0x6d // 109 - 'm'
#define CHARCODE_n   0x6e // 110 - 'n'
#define CHARCODE_o   0x6f // 111 - 'o'
#define CHARCODE_p   0x70 // 112 - 'p'
#define CHARCODE_q   0x71 // 113 - 'q'
#define CHARCODE_r   0x72 // 114 - 'r'
#define CHARCODE_s   0x73 // 115 - 's'
#define CHARCODE_t   0x74 // 116 - 't'
#define CHARCODE_u   0x75 // 117 - 'u'
#define CHARCODE_v   0x76 // 118 - 'v'
#define CHARCODE_w   0x77 // 119 - 'w'
#define CHARCODE_x   0x78 // 120 - 'x'
#define CHARCODE_y   0x79 // 121 - 'y'
#define CHARCODE_z   0x7a // 122 - 'z'
#define CHARCODE_LCURLYBRACKET  0x7b // 123 - '{'
#define CHARCODE_VBAR           0x7c // 124 - '|'
#define CHARCODE_RCURLYBRACKET  0x7d // 125 - '}'
#define CHARCODE_TILDE          0x7e // 126 - '~'
#define CHARCODE_DEL            0x7f // 127 - '\x7f'

typedef uint8_t keycode_t;

//! Максимальное значение кода клавиши.
#define KEY_MAX 0x9b

// Keys.
#define KEY_BACKSPACE    CHARCODE_BS // <- 0x8
#define KEY_TAB          CHARCODE_CT // -> 0x9
#define KEY_ENTER        CHARCODE_LF // Enter 0xa
#define KEY_SHIFT        0x10 // Shift
#define KEY_CTRL         0x11 // Ctrl
#define KEY_ALT          0x12 // Alt
#define KEY_PAUSE        0x13 // Pause / break
#define KEY_CAPSLOCK     0x14 // Caps lock
#define KEY_SCROLL       0x15 // Scroll lock
#define KEY_NUMLOCK      0x16 // Num lock
#define KEY_ESC          CHARCODE_ESC // Esc 0x1b
#define KEY_SPACE        CHARCODE_SPACE // 
#define KEY_PAGEUP       0x21 // Page up
#define KEY_PAGEDOWN     0x22 // Page down
#define KEY_LEFT         0x23 // Left arrow
#define KEY_RIGHT        0x24 // Right arrow
#define KEY_UP           0x25 // Up arrow
#define KEY_DOWN         0x26 // Down arrow
#define KEY_APOSTROPHE   CHARCODE_APOSTROPHE // '
#define KEY_PRINTSCREEN  0x28 // Print screen
#define KEY_INSERT       0x29 // Insert
#define KEY_HOME         0x2a // Home
#define KEY_END          0x2b // End
#define KEY_COMMA        CHARCODE_COMMA // ,
#define KEY_MINUS        CHARCODE_MINUS // -
#define KEY_DOT          CHARCODE_DOT // .
#define KEY_SLASH        CHARCODE_SLASH // /
#define KEY_0   CHARCODE_0 // 0
#define KEY_1   CHARCODE_1 // 1
#define KEY_2   CHARCODE_2 // 2
#define KEY_3   CHARCODE_3 // 3
#define KEY_4   CHARCODE_4 // 4
#define KEY_5   CHARCODE_5 // 5
#define KEY_6   CHARCODE_6 // 6
#define KEY_7   CHARCODE_7 // 7
#define KEY_8   CHARCODE_8 // 8
#define KEY_9   CHARCODE_9 // 9
#define KEY_SEMICOLON    CHARCODE_SEMICOLON // ;
#define KEY_EQUAL        CHARCODE_EQUAL // =
#define KEY_A   CHARCODE_A // A
#define KEY_B   CHARCODE_B // B
#define KEY_C   CHARCODE_C // C
#define KEY_D   CHARCODE_D // D
#define KEY_E   CHARCODE_E // E
#define KEY_F   CHARCODE_F // F
#define KEY_G   CHARCODE_G // G
#define KEY_H   CHARCODE_H // H
#define KEY_I   CHARCODE_I // I
#define KEY_J   CHARCODE_J // J
#define KEY_K   CHARCODE_K // K
#define KEY_L   CHARCODE_L // L
#define KEY_M   CHARCODE_M // M
#define KEY_N   CHARCODE_N // N
#define KEY_O   CHARCODE_O // O
#define KEY_P   CHARCODE_P // P
#define KEY_Q   CHARCODE_Q // Q
#define KEY_R   CHARCODE_R // R
#define KEY_S   CHARCODE_S // S
#define KEY_T   CHARCODE_T // T
#define KEY_U   CHARCODE_U // U
#define KEY_V   CHARCODE_V // V
#define KEY_W   CHARCODE_W // W
#define KEY_X   CHARCODE_X // X
#define KEY_Y   CHARCODE_Y // Y
#define KEY_Z   CHARCODE_Z // Z
#define KEY_LSQBRACKET   CHARCODE_LSQBRACKET // [
#define KEY_BACKSLASH    CHARCODE_BACKSLASH // /* \ */ 
#define KEY_RSQBRACKET   CHARCODE_RSQBRACKET // ]
#define KEY_GRAVEACCENT  CHARCODE_GRAVEACCENT // `
#define KEY_DEL          CHARCODE_DEL // DEL
#define KEY_NUM_SLASH    0x80 // Num /
#define KEY_NUM_ASTERISK 0x81 // Num *
#define KEY_NUM_DEL      0x82 // Num del
#define KEY_NUM_0    0x83 // Num 0
#define KEY_NUM_1    0x84 // Num 1
#define KEY_NUM_2    0x85 // Num 2
#define KEY_NUM_3    0x86 // Num 3
#define KEY_NUM_4    0x87 // Num 4
#define KEY_NUM_5    0x88 // Num 5
#define KEY_NUM_6    0x89 // Num 6
#define KEY_NUM_7    0x8a // Num 7
#define KEY_NUM_8    0x8b // Num 8
#define KEY_NUM_9    0x8c // Num 9
#define KEY_NUM_MINUS   0x8d // Num minus
#define KEY_NUM_PLUS    0x8e // Num plus
#define KEY_NUM_ENTER   0x8f // Num enter
#define KEY_F1    0x90 // F1
#define KEY_F2    0x91 // F2
#define KEY_F3    0x92 // F3
#define KEY_F4    0x93 // F4
#define KEY_F5    0x94 // F5
#define KEY_F6    0x95 // F6
#define KEY_F7    0x96 // F7
#define KEY_F8    0x97 // F8
#define KEY_F9    0x98 // F9
#define KEY_F10   0x99 // F10
#define KEY_F11   0x9a // F11
#define KEY_F12   0x9b // F12

#endif	//KEYS_H
