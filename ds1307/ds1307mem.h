/**
 * @file ds1307mem.h Структура памяти часов DS1307.
 */

#ifndef DS1307MEM_H
#define	DS1307MEM_H

#include <stdint.h>

//Структура памяти в ds1307.

//Побайтно.
#pragma pack(push, 1)
typedef struct _SecondsByte {
    uint8_t seconds:4;
    uint8_t seconds10:3;
    uint8_t clock_halt:1;
}seconds_byte_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _MinutesByte {
    uint8_t minutes:4;
    uint8_t minutes10:3;
    uint8_t reserved1:1;
}minutes_byte_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _HoursByte {
    uint8_t hours:4;
    uint8_t hours10:2;
    uint8_t ampm:1;
    uint8_t reserved2:1;
}hours_byte_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _DayByte {
    uint8_t day:3;
    uint8_t reserved3:5;
}day_byte_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _DateByte {
    uint8_t date:4;
    uint8_t date10:2;
    uint8_t reserved4:2;
}date_byte_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _MonthByte {
    uint8_t month:4;
    uint8_t month10:1;
    uint8_t reserved5:3;
}month_byte_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _YearByte {
    uint8_t year:4;
    uint8_t year10:4;
}year_byte_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _SqweByte {
    uint8_t rs:2;
    uint8_t reserved7_1:2;
    uint8_t sqwe:1;
    uint8_t reserved7_2:2;
    uint8_t out:1;
}sqwe_byte_t;
#pragma pack(pop)

//Полная память.
#pragma pack(push, 1)
typedef struct _Ds1307mem {
    //byte 0
    seconds_byte_t seconds_byte;
    //byte 1
    minutes_byte_t minutes_byte;
    //byte 2
    hours_byte_t hours_byte;
    //byte 3
    day_byte_t day_byte;
    //byte 4
    date_byte_t date_byte;
    //byte 5
    month_byte_t month_byte;
    //byte 6
    year_byte_t year_byte;
    //byte 7;
    sqwe_byte_t sqwe_byte;
} ds1307mem_t;
#pragma pack(pop)

#endif	/* DS1307MEM_H */

