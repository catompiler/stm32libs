#include "localization.h"
#include <stdlib.h>


//! Тип локализации.
typedef struct _Localization {
    const translation_t* trs; //!< Переводы.
    size_t trs_count; //!< Число переводов.
    size_t cur_tr; //!< Индекс текущего перевода.
    size_t def_tr; //!< Индекс перевода по-умолчанию.
    const char* default_text; //!< Возвращаемый текст по-умолчанию.
} localization_t;


static localization_t loc;


err_t localization_init(const translation_t* trs, size_t trs_count)
{
    if(trs == NULL) return E_NULL_POINTER;
    if(trs_count == 0) return E_INVALID_VALUE;
    
    loc.trs = trs;
    loc.trs_count = trs_count;
    loc.cur_tr = 0;
    loc.def_tr = 0;
    loc.default_text = NULL;
    
    return E_NO_ERROR;
}

/**
 * Проверяет равенство двух идентификаторов языков.
 * @param l_lang Идентификатор языка.
 * @param r_lang Идентификатор языка.
 * @return Флаг равенства двух идентификаторов изыков.
 */
ALWAYS_INLINE static bool localization_langs_equals(lang_id_t l_lang, lang_id_t r_lang)
{
    return l_lang == r_lang;
}

/**
 * Ищет перевод по идентификатору языка.
 * @param lang_id Идентификатор языка.
 * @param index Возвращаемый индекс перевода.
 * @return Наличие перевода для заданного языка.
 */
static bool localization_tr_index_by_lang_id(lang_id_t lang_id, size_t* index)
{
    size_t i = 0;
    
    for(; i < loc.trs_count; i ++){
        if(localization_langs_equals(loc.trs[i].lang_id, lang_id)){
            if(index) *index = i;
            return true;
        }
    }
    
    return false;
}

bool localization_set_default_lang(lang_id_t lang_id)
{
    size_t index = 0;
    if(localization_tr_index_by_lang_id(lang_id, &index)){
        loc.def_tr = index;
        return true;
    }
    return false;
}

void localization_set_default_text(const char* text)
{
    loc.default_text = text;
}

bool localization_set_lang(lang_id_t lang_id)
{
    size_t index = 0;
    if(localization_tr_index_by_lang_id(lang_id, &index)){
        loc.cur_tr = index;
        return true;
    }
    return false;
}

/**
 * Функция сравнения перевода текста и идентификатора.
 * @param id Идентификатор перевода текста.
 * @param text_tr Перевод.
 * @return Результат сравнения.
 */
static int localization_compare_text_trs (const void * id, const void * text_tr)
{
    trid_t id_tr = ((const text_tr_t*)text_tr)->id;
    
    if((trid_t)id > id_tr) return 1;
    if((trid_t)id < id_tr) return -1;
    
    return 0;
}

/**
 * Ищет текст перевода по идентификатору в заданном переводе.
 * @param id Идентификатор перевода текста.
 * @param search_tr Перевод.
 * @return Текст перевода.
 */
static const char* localization_translate_impl(trid_t id, const translation_t* search_tr)
{
    const text_tr_t* text_tr = bsearch((const void*)id, search_tr->text_trs, search_tr->text_trs_count,
                                 sizeof(text_tr_t), localization_compare_text_trs);
    
    if(text_tr) return text_tr->text;
    
    return NULL;
}

const char* localization_translate(trid_t id)
{
    const char* cur_text = localization_translate_impl(id, &loc.trs[loc.cur_tr]);
    
    if(!cur_text){
        cur_text = localization_translate_impl(id, &loc.trs[loc.def_tr]);
    }
    
    if(cur_text) return cur_text;
    
    return loc.default_text;
}
