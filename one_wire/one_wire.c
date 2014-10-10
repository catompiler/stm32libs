#include "one_wire.h"
#include "utils/utils.h"
#include "utils/delay.h"
#include "bits/bits.h"
#include "defs/defs.h"


#define ONE_WIRE_RESET_PULSE_MIN_US     480
#define ONE_WIRE_RESET_PULSE_MAX_US     640
#define ONE_WIRE_RESET_PULSE_US         500

#define ONE_WIRE_BUS_REACTION_MIN_US    15
#define ONE_WIRE_BUS_REACTION_MAX_US    60
#define ONE_WIRE_BUS_REACTION_US        30

#define ONE_WIRE_PRESENCE_PULSE_MIN_US  60
#define ONE_WIRE_PRESENCE_PULSE_MAX_US  240
#define ONE_WIRE_PRESENCE_PULSE_US      240

#define ONE_WIRE_FRAMES_SEPARATOR_MIN_US 1
#define ONE_WIRE_FRAMES_SEPARATOR_MAX_US 255
#define ONE_WIRE_FRAMES_SEPARATOR_US    2

#define ONE_WIRE_FRAME_HEAD_US          15

#define ONE_WIRE_DELAY_CORRECTION_US    (3)

#define ONE_WIRE_FRAME_BEGIN_MIN_US     1
#define ONE_WIRE_FRAME_BEGIN_MAX_US     ONE_WIRE_FRAME_HEAD_US
#define ONE_WIRE_FRAME_BEGIN_US         2

#define ONE_WIRE_FRAME_RW_US            60


/**
 * Начинает кадр передачи данных на шине 1-wire.
 * @param ow Шина 1-wire
 */
ALWAYS_INLINE static void one_wire_frame_begin(one_wire_t* ow)
{
    delay_us(ONE_WIRE_FRAMES_SEPARATOR_US);
    GPIO_ResetBits(ow->GPIO, ow->GPIO_Pin);
}

/**
 * Заканчивает кадр передачи данных на шине 1-wire.
 * @param ow Шина 1-wire
 */
ALWAYS_INLINE static void one_wire_frame_end(one_wire_t* ow)
{
    GPIO_SetBits(ow->GPIO, ow->GPIO_Pin);
}

/**
 * Устанавливает логическое значение на шине 1-wire.
 */
ALWAYS_INLINE static void one_wire_set_value(one_wire_t* ow, uint8_t value)
{
    GPIO_WriteBit(ow->GPIO, ow->GPIO_Pin, value);
}

/**
 * Возвращает текущее логическое значение на шине 1-wire.
 * @param ow Шина 1-wire
 */
ALWAYS_INLINE static uint8_t one_wire_get_value(one_wire_t* ow)
{
    return GPIO_ReadInputDataBit(ow->GPIO, ow->GPIO_Pin);
}

err_t one_wire_init(one_wire_t* ow, GPIO_TypeDef* GPIO, uint16_t GPIO_Pin)
{
    ow->GPIO = GPIO;
    ow->GPIO_Pin = GPIO_Pin;
    
    GPIO_InitTypeDef gpio_init = {
        .GPIO_Pin = ow->GPIO_Pin,
        .GPIO_Speed = GPIO_Speed_2MHz,
        .GPIO_Mode = GPIO_Mode_Out_OD
    };
    
    GPIO_Init(ow->GPIO, &gpio_init);
    
    one_wire_frame_end(ow);
    
    return E_NO_ERROR;
}

uint8_t one_wire_reset(one_wire_t* ow)
{
    //__interrupts_save_disable();
    one_wire_frame_begin(ow);
    
    delay_us(ONE_WIRE_RESET_PULSE_US);
    
    one_wire_frame_end(ow);
    
    delay_us(ONE_WIRE_BUS_REACTION_US);
    
    uint8_t presence = one_wire_get_value(ow);
    
    delay_us(ONE_WIRE_PRESENCE_PULSE_US);
    
    //__interrupts_restore();
    
    return BIT0_NOT(presence);
}

void one_wire_write_bit(one_wire_t* ow, uint8_t bit)
{
    //__interrupts_save_disable();
    one_wire_frame_begin(ow);
    
    delay_us(ONE_WIRE_FRAME_BEGIN_US);
    
    one_wire_set_value(ow, bit);
    
    delay_us(ONE_WIRE_FRAME_RW_US - ONE_WIRE_FRAME_BEGIN_US);
    
    one_wire_frame_end(ow);
    //__interrupts_restore();
}

uint8_t one_wire_read_bit(one_wire_t* ow)
{
    //__interrupts_save_disable();
    one_wire_frame_begin(ow);
    
    delay_us(ONE_WIRE_FRAME_BEGIN_US);
    
    one_wire_frame_end(ow);
    
    delay_us(ONE_WIRE_FRAME_HEAD_US - ONE_WIRE_FRAME_BEGIN_US - ONE_WIRE_DELAY_CORRECTION_US);
            
    uint8_t value = one_wire_get_value(ow);
    
    delay_us(ONE_WIRE_FRAME_RW_US - ONE_WIRE_FRAME_HEAD_US + ONE_WIRE_DELAY_CORRECTION_US);
    
    //__interrupts_restore();
    
    return value;
}

void one_wire_write_byte(one_wire_t* ow, uint8_t byte)
{
    uint8_t i = 0;
    for(; i < 8; i ++){
        one_wire_write_bit(ow, byte & 0x1);
        byte >>= 1;
    }
}

uint8_t one_wire_read_byte(one_wire_t* ow)
{
    uint8_t byte = 0;
    uint8_t i = 7;
    for(;;){
        if(one_wire_read_bit(ow)) byte |= 0x80;
        if(i == 0) break;
        i --;
        byte >>= 1;
    }
    return byte;
}

void one_wire_write(one_wire_t* ow, const void* data, size_t size)
{
    size_t i = 0;
    for(; i < size; i ++){
        one_wire_write_byte(ow, ((const uint8_t*)data)[i]);
    }
}

void one_wire_read(one_wire_t* ow, void* data, size_t size)
{
    size_t i = 0;
    for(; i < size; i ++){
        ((uint8_t*)data)[i] = one_wire_read_byte(ow);
    }
}

/**
 * Вычисляет очередную итерацию CRC.
 */
static uint8_t one_wire_crc_iter(uint8_t crc, uint8_t data)
{
    uint8_t i;

    crc ^= data;

    for(i = 0; i < 8; i ++){
        if(crc & 0x1){
            crc = (crc >> 1) ^ 0x8c;
        }else{
            crc >>= 1;
        }
    }
    
    return crc;
}


uint8_t one_wire_calc_crc(const void* data, size_t size)
{
    size_t i = 0;
    uint8_t crc = 0;
    for(i = 0; i < size; i ++){
        crc = one_wire_crc_iter(crc, ((const uint8_t*)data)[i]);
    }
    return crc;
}

err_t one_wire_read_rom(one_wire_t* ow, one_wire_rom_id_t* rom)
{
    uint8_t crc = 0;
    //uint8_t i = 0;
    
    //if(!one_wire_reset(ow)) return E_ONE_WIRE_DEVICES_NOT_FOUND;
    
    one_wire_send_cmd(ow, ONE_WIRE_CMD_READ_ROM);
    
    /*rom->family_code = one_wire_read_byte(ow);
    for(i = 0; i < ONE_WIRE_SERIAL_LEN; i ++){
        one_wire_write_byte(ow, rom->serial[i]);
    }
    rom->crc = one_wire_read_byte(ow);*/
    one_wire_read(ow, rom, sizeof(one_wire_rom_id_t));
    
    crc = one_wire_calc_crc((const void*)rom, 0x7);
    if(crc != rom->crc) return E_ONE_WIRE_INVALID_CRC;
    
    return E_NO_ERROR;
}

err_t one_wire_match_rom(one_wire_t* ow, one_wire_rom_id_t* rom)
{
    //uint8_t i = 0;
    
    //if(!one_wire_reset(ow)) return E_ONE_WIRE_DEVICES_NOT_FOUND;
    
    one_wire_send_cmd(ow, ONE_WIRE_CMD_MATCH_ROM);
    
    /*one_wire_write_byte(ow, rom->family_code);
    for(i = 0; i < ONE_WIRE_SERIAL_LEN; i ++){
        one_wire_write_byte(ow, rom->serial[i]);
    }
    one_wire_write_byte(ow, rom->crc);*/
    one_wire_write(ow, rom, sizeof(one_wire_rom_id_t));
    
    return E_NO_ERROR;
}

err_t one_wire_skip_rom(one_wire_t* ow)
{
    //if(!one_wire_reset(ow)) return E_ONE_WIRE_DEVICES_NOT_FOUND;
    
    one_wire_send_cmd(ow, ONE_WIRE_CMD_SKIP_ROM);
    
    return E_NO_ERROR;
}

err_t one_wire_search_roms(one_wire_t* ow, one_wire_rom_id_t* roms,
                           uint8_t roms_count, uint8_t* roms_found,
                           uint8_t max_attempts)
{
    //Текущий ROM.
    uint8_t cur_rom = 0;
    //бит, временная переменная
    uint8_t tmp_bit = 0;
    //Первый бит ответа.
    uint8_t bit0 = 0;
    //Второй бит ответа.
    uint8_t bit1 = 0;
    //Результат "сравнения" бит.
    uint8_t cmp = 0;
    //Итератор бит.
    uint8_t i_bit = 0;
    //Линейное значение передаваемого бита.
    uint8_t bit_l = 0;
    //Начальное значение позоций коллизий.
    #define COLLIS_POS_INITIAL 255
    //Последняя коллизия.
    uint8_t last_collis = COLLIS_POS_INITIAL;
    //Последняя коллизия на предыдущем проходе.
    uint8_t prev_collis = COLLIS_POS_INITIAL;
    //Контрольная сумма.
    uint8_t crc = 0;
    //Переменная для отладки.
    //uint8_t debug = 0;
    //Текущее число оставшихся попыток.
    uint8_t cur_attempts = max_attempts;
    
    *roms_found = 0;
    
    for(; cur_rom < roms_count;){
        //Если нет устройств на шине - нет смысла их искать (с) КЭП.
        if(!one_wire_reset(ow)){
            return E_ONE_WIRE_DEVICES_NOT_FOUND;
        }
        //Пошлём команду поиска.
        one_wire_send_cmd(ow, ONE_WIRE_CMD_SEARCH_ROM);
        
        //Нужно считать 64 бита.
        for(i_bit = 0; i_bit < 64; i_bit ++){
            
            //Получим номер бита в структуре ROM.
            bit_l = i_bit;//get_linear_bit_n(i_bit);
            
            //Считаем два бита, определяющих текущий бит в ромах устройств.
            bit1 = one_wire_read_bit(ow);
            bit0 = one_wire_read_bit(ow);
            
            cmp = (bit1 << 1) | bit0;
            
            //Оба бита равны нулю.
            //Часть устройств имеют 0, а часть 1 в этой позиции.
            if(cmp == 0x0){
                //Если текущая позиция меньше позиции предыдущей коллизии.
                if(i_bit < prev_collis){
                    //Получим бит предыдущего ROM, или 0.
                    if(cur_rom == 0) tmp_bit = 0;
                    else tmp_bit = bits_value((uint8_t*)&roms[cur_rom - 1], bit_l);
                    
                    //Установим его же значение.
                    bits_set_value((uint8_t*)&roms[cur_rom], bit_l, tmp_bit);
                    one_wire_write_bit(ow, tmp_bit);
                    
                    //Если бит не переключался, тогда зафиксируем коллизию.
                    if(tmp_bit == 0) last_collis = i_bit;
                //Если мы на позиции предыдущей коллизии.
                }else if(i_bit == prev_collis){
                    //Устанавливаем бит в 1.
                    bits_on((uint8_t*)&roms[cur_rom], bit_l);
                    one_wire_write_bit(ow, 1);
                //Если текущая позиция после позиции предыдущей коллизии.
                }else{ //if(i_bit > prev_collis)
                    //Устанавливаем бит в 0.
                    bits_off((uint8_t*)&roms[cur_rom], bit_l);
                    one_wire_write_bit(ow, 0);
                    //Зафиксируем коллизию.
                    last_collis = i_bit;
                }
            }
            //Второй бит равен единице.
            //Все устройства имеют 0 в этой позиции.
            else if(cmp == 0x1){
                bits_off((uint8_t*)&roms[cur_rom], bit_l);
                one_wire_write_bit(ow, 0);
            }
            //Первый бит равен единице.
            //Все устройства имеют 1 в этой позиции.
            else if(cmp == 0x2){
                bits_on((uint8_t*)&roms[cur_rom], bit_l);
                one_wire_write_bit(ow, 1);
            }
            //Оба бита равны единице.
            //Нет устройств.
            else{//0x3:
                //Если исчерпали попытки.
                if(cur_attempts == 0){
                    //Выход.
                    return E_ONE_WIRE_SEARCH_LOGIC_ERROR;
                }
                //Иначе попробуем ещё раз.
                cur_attempts --;
                break;
            }
        }
        //Если поиск устройства окончен.
        if(i_bit == 64){
            //Проверим контрольную сумму.
            crc = one_wire_calc_crc((const void*)&roms[cur_rom], 0x7);//1 байт family + 6 байт serial
            //Если совпадают, то поиск успешен.
            if(crc == roms[cur_rom].crc){
                //Увеличим число найденных ROM.
                (*roms_found) ++;
                //Перейдём к следующему ROM.
                cur_rom ++;
                //Обновим значение позиции предыдущей коллизии.
                prev_collis = last_collis;
                //сбросим число попыток.
                cur_attempts = max_attempts;
                //Если коллизий небыло, т.е. найдено последнее устройство - выход.
                if(last_collis == COLLIS_POS_INITIAL){
                    break;
                }
            }else{
                //Если исчерпали попытки.
                if(cur_attempts == 0){
                    //Выход.
                    return E_ONE_WIRE_INVALID_CRC;
                }
                //Иначе попробуем ещё раз.
                cur_attempts --;
            }
        }
        //Сбросим значение последней текущей коллизии.
        last_collis = COLLIS_POS_INITIAL;
    }
    
    return E_NO_ERROR;
}

