#include "ds18x20.h"


//Маска битов разрешения в регистре конфигурации.
#define DS18X20_RESOLUTION_BITS_MASK (3 << DS18X20_RESOLUTION_BITS_OFFSET)
#define DS18X20_RESOLUTION_MAX_VALUE (DS18X20_RESOLUTION_MAX >> DS18X20_RESOLUTION_BITS_OFFSET)


//Размер частей значения температуры
#define DS18X20_TEMP_PART_SIZE 4
//Смещение целого значения температуры в старшем байте.
#define DS18X20_TEMP_INT_OFFSET_HI 0
//Маска целого значения температуры в старшем байте.
#define DS18X20_TEMP_INT_MASK_HI BIT_MAKE_MASK(DS18X20_TEMP_PART_SIZE, DS18X20_TEMP_INT_OFFSET_HI)
//Смещение целого значения температуры в младшем байте.
#define DS18X20_TEMP_INT_OFFSET_LO DS18X20_TEMP_PART_SIZE
//Маска целого значения температуры в младшем байте.
#define DS18X20_TEMP_INT_MASK_LO BIT_MAKE_MASK(DS18X20_TEMP_PART_SIZE, DS18X20_TEMP_INT_OFFSET_LO)
//Смещение дробного значения температуры в старшем байте.
#define DS18X20_TEMP_FRACT_OFFSET 0
//Маска дробного значения температуры в старшем байте.
#define DS18X20_TEMP_FRACT_MASK BIT_MAKE_MASK(DS18X20_TEMP_PART_SIZE, DS18X20_TEMP_FRACT_OFFSET)



//Структура памяти сенсора.
#pragma pack(push, 1)
typedef struct _Ds18x20_Scratchpad {
    uint8_t temp_lsb;
    uint8_t temp_msb;
    uint8_t th;
    uint8_t tl;
    uint8_t config_reg;
    uint8_t reserved_0xff;
    uint8_t reserved_0x0c;
    uint8_t reserved_0x10;
    uint8_t crc;
} ds18x20_scratchpad_t;
#pragma pack(pop)

#if DS18X20_READ_SCRATCHPAD_FULLY == 1
    #define SCRATCHPAD_READ_SIZE 9
#else
    #define SCRATCHPAD_READ_SIZE 5
#endif


static ds18x20_scratchpad_t scratchpad;


err_t ds18x20_init(ds18x20_t* sensor, one_wire_t* ow, one_wire_rom_id_t* rom)
{
    sensor->one_wire = ow;
    sensor->rom = rom;
    
    return E_NO_ERROR;
}

err_t ds18x20_select(ds18x20_t* sensor)
{
    if(!one_wire_reset(sensor->one_wire)) return E_ONE_WIRE_DEVICES_NOT_FOUND;
    
    if(sensor->rom){
        one_wire_match_rom(sensor->one_wire, sensor->rom);
    }else{
        one_wire_skip_rom(sensor->one_wire);
    }
    
    return E_NO_ERROR;
}

err_t ds18x20_configure(ds18x20_t* sensor, uint8_t resolution,
                       int8_t alarm_temp_lo, int8_t alarm_temp_hi)
{
    err_t err = E_NO_ERROR;
    
    err = ds18x20_select(sensor);
    if(err != E_NO_ERROR) return err;
    
    one_wire_send_cmd(sensor->one_wire, DS18X20_CMD_SCRATCHPAD_WRITE);
    one_wire_write_byte(sensor->one_wire, alarm_temp_hi);
    one_wire_write_byte(sensor->one_wire, alarm_temp_lo);
    one_wire_write_byte(sensor->one_wire, resolution);
    
    return E_NO_ERROR;
}

err_t ds18x20_start_conversion(ds18x20_t* sensor)
{
    err_t err = E_NO_ERROR;
    
    ds18x20_select(sensor);
    if(err != E_NO_ERROR) return err;
    
    one_wire_send_cmd(sensor->one_wire, DS18X20_CMD_TEMP_START_CONVERT);
    
    return E_NO_ERROR;
}

bool ds18x20_conversion_done(ds18x20_t* sensor)
{
    return one_wire_read_bit(sensor->one_wire);
}

err_t ds18x20_read_temp(ds18x20_t* sensor, fixed16_t* temp)
{
    uint8_t inv_res = 0;
    uint8_t res_mask = 0xff;
    int16_t res_temp = 0;
    err_t err = E_NO_ERROR;
    
    err = ds18x20_select(sensor);
    if(err != E_NO_ERROR) return err;
    
    one_wire_send_cmd(sensor->one_wire, DS18X20_CMD_SCRATCHPAD_READ);

#if DS18X20_READ_SCRATCHPAD_FULLY == 1

    one_wire_read(sensor->one_wire, &scratchpad, SCRATCHPAD_READ_SIZE);
    
    if(one_wire_calc_crc(&scratchpad, SCRATCHPAD_READ_SIZE - 1) != scratchpad.crc){
        return E_ONE_WIRE_INVALID_CRC;
    }
#else
    one_wire_read(sensor->one_wire, &scratchpad, SCRATCHPAD_READ_SIZE);
#endif
    
    inv_res = DS18X20_RESOLUTION_MAX_VALUE -
                ((scratchpad.config_reg & DS18X20_RESOLUTION_BITS_MASK) >>
                DS18X20_RESOLUTION_BITS_OFFSET);
    res_mask = 0xff << inv_res;
    
    res_temp |= (uint16_t)(scratchpad.temp_msb & DS18X20_TEMP_INT_MASK_HI) << (FIXED16_FRACT_BITS + DS18X20_TEMP_PART_SIZE);
    res_temp |= (uint16_t)(scratchpad.temp_lsb & DS18X20_TEMP_INT_MASK_LO) << DS18X20_TEMP_PART_SIZE;
    res_temp |= (uint16_t)((scratchpad.temp_lsb & DS18X20_TEMP_FRACT_MASK) & res_mask) << DS18X20_TEMP_PART_SIZE;
    
    *temp = res_temp;
    
    return E_NO_ERROR;
}
