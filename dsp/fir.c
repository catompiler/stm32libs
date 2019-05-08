#include "fir.h"
#include <string.h>


err_t fir_init(fir_t* fir, q15_t* coefs, q15_t* data, size_t size)
{
    if(coefs == NULL) return E_NULL_POINTER;
    if(data == NULL) return E_NULL_POINTER;
    if(size == 0) return E_INVALID_VALUE;

    fir->coefs = coefs;
    fir->data = data;
    fir->size = size;
    fir->index = 0;

    return E_NO_ERROR;
}

void fir_reset(fir_t* fir)
{
    memset(fir->data, 0x0, sizeof(q15_t) * fir->size);
    fir->index = 0;
}

ALWAYS_INLINE static size_t fir_next_index(fir_t* fir, size_t index)
{
    size_t next_index = index + 1;

    if(next_index >= fir->size) next_index = 0;

    return next_index;
}

void fir_put(fir_t* fir, q15_t data)
{
    fir->data[fir->index] = data;
    fir->index = fir_next_index(fir, fir->index);
}

// Прямая версия.
q15_t fir_calc(fir_t* fir)
{
    int32_t res;

    register int32_t c;
    register int32_t d;

    size_t index = fir->index;

    register int64_t acc = 0;

    size_t i;
    for(i = 0; i < fir->size; i ++){
        c = fir->coefs[i];
        d = fir->data[index];

        //acc += c * d;
        acc = iq15_mlal(c, d, acc);

        index = fir_next_index(fir, index);
    }

    res = acc >> 15;
    res = q15_sat(res);

    return res;
}

// Версия с загрузкой по два элемента.
/*q15_t fir_calc(fir_t* fir)
{
    int32_t res;

    union _U_DATA32 {
        struct _S_DATA32{
            int16_t _0;
            int16_t _1;
        } _16;
        int32_t _32;
    };

    register union _U_DATA32 c;
    register union _U_DATA32 d;

    register int64_t acc = 0;

    size_t last_i = fir->size - 1;

    int16_t* coefs = fir->coefs;
    int16_t* data = &fir->data[fir->index];

    int16_t* data_beg = fir->data;
    int16_t* data32_end = (int16_t*)(&((int32_t*)fir->data)[fir->size >> 1]);
    int16_t* data_end = &fir->data[fir->size];

    size_t i;
    for(i = 0; i < fir->size;){

        if(data < data32_end && i < last_i){
            c._32 = *(int32_t*)coefs;
            d._32 = *(int32_t*)data;

            //acc += c * d;
            acc = iq15_mlal(c._16._0, d._16._0, acc);
            acc = iq15_mlal(c._16._1, d._16._1, acc);

            coefs += 2;
            data += 2;

            i += 2;
        }else{
            c._16._0 = *(int16_t*)coefs;
            d._16._0 = *(int16_t*)data;

            //acc += c * d;
            acc = iq15_mlal(c._16._0, d._16._0, acc);

            coefs += 1;
            data += 1;

            i += 1;
        }

        if(data == data_end) data = data_beg;
    }

    res = acc >> 15;
    res = q15_sat(res);

    return res;
}*/

q15_t fir_filter(fir_t* fir, q15_t data)
{
    int32_t res;

    register int32_t c;
    register int32_t d = data;
    register int32_t tmp;

    register int64_t acc = 0;

    size_t i;
    for(i = 0; i < fir->size; i ++){

        c = fir->coefs[i];

        //acc += c * d;
        acc = iq15_mlal(c, d, acc);

        tmp = fir->data[i];
        fir->data[i] = d;
        d = tmp;
    }

    res = acc >> 15;
    res = q15_sat(res);

    return res;
}

