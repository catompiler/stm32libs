/**
 * @file localization.h Библиотека локализации.
 */

#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <stddef.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include <stdbool.h>


//! Атрибуты декларации текста перевода.
#ifndef TEXT_TR_DECL_ATTRIBS
#define TEXT_TR_DECL_ATTRIBS
#endif


//! Атрибуты декларации перевода.
#ifndef TRANSLATION_DECL_ATTRIBS
#define TRANSLATION_DECL_ATTRIBS
#endif


//! Тип идентификатора перевода.
typedef uint32_t trid_t;

//! Тип переводимого текста.
typedef struct _Text_Tr {
    trid_t id; //!< Идентификатор переведённой строки текста.
    const char* text; //!< Переведённый текст.
} text_tr_t;

//! Начало декларации перевода.
#define TEXT_TRS_BEGIN(arg_trs_name)\
    TEXT_TR_DECL_ATTRIBS text_tr_t arg_trs_name[] = {

//! Определение переведённого текста.
#define TEXT_TR(arg_id, arg_text) { arg_id, arg_text },

//! Конец декларации перевода.
#define TEXT_TRS_END()\
    };

//! Тип идентификатора языка.
typedef uint32_t lang_id_t;

//! Тип перевода.
typedef struct _Translation {
    lang_id_t lang_id; //!< Идентификатор языка.
    const text_tr_t* text_trs; //!< Переведённые строки текста.
    size_t text_trs_count; //!< Число переведённых строк текста.
} translation_t;

//! Начало декларации структур перевода.
#define TRANSLATIONS_BEGIN(arg_trs_name)\
    TRANSLATION_DECL_ATTRIBS translation_t arg_trs_name[] = {

//! Заполнение структуры перевода по месту объявления.
#define TRANSLATION(arg_lang_id, arg_text_trs)\
    { arg_lang_id, arg_text_trs, (sizeof(arg_text_trs)/sizeof(text_tr_t)) },

//! Конец декларации структур перевода.
#define TRANSLATIONS_END()\
    };

//! Получение количества переводов в массиве.
#define TRANSLATIONS_COUNT(trs) (sizeof(trs)/sizeof(translation_t))


/**
 * Инициализирует локализацию.
 * @param trs Переводы.
 * @param trs_count Число переводов.
 * @return Код ошибки.
 */
EXTERN err_t localization_init(const translation_t* trs, size_t trs_count);

/**
 * Устанавливает язык перевода по-умолчанию.
 * По умолчанию выбирается язык с индексом 0 в списке переводов.
 * @param lang_id Идентификатор языка.
 * @return Флаг установки языка по-умолчанию.
 */
EXTERN bool localization_set_default_lang(lang_id_t lang_id);

/**
 * Устанавливает текст перевода по-умолчанию.
 * По умолчанию текст по-умолчанию отсутствует (NULL).
 * @param text Текст по-умолчанию.
 */
EXTERN void localization_set_default_text(const char* text);

/**
 * Устанавливает текущий язык перевода.
 * @param lang_id Идентификатор языка.
 * @return Флаг установки текущего языка.
 */
EXTERN bool localization_set_lang(lang_id_t lang_id);

/**
 * Получает текст перевода по его идентификатору.
 * @param id Идентификатор перевода.
 * @return Текст перевода по его идентификатору.
 * При отсутствии перевода в текущем языке пытается найти перевод
 * в языке по-умолчанию, в случае неудачи возвращает NULL.
 */
EXTERN const char* localization_translate(trid_t id);

//! Алиас для localization_translate.
#define TR(ID) localization_translate((trid_t)(ID))

//! Преобразование trid_t к const char*.
#define TEXT(ID) ((const char*)(ID))

//! Преобразование const char* к trid_t.
#define TRID(T) ((trid_t)(T))


/*

// Пример использования.

// Идентификаторы языков.
#define TR_LANG_ID_EN 0
#define TR_LANG_ID_RU 1

// Идентификаторы переводов текста.
#define TR_ID_HELLO 10
#define TR_ID_WORLD 20

// Английский перевод.
TEXT_TRS_BEGIN(text_trs_en)
TEXT_TR(TR_ID_HELLO, "Hello")
TEXT_TR(TR_ID_WORLD, "World")
TEXT_TRS_END()

// Русский перевод.
TEXT_TRS_BEGIN(text_trs_ru)
TEXT_TR(TR_ID_HELLO, "Привет")
TEXT_TR(TR_ID_WORLD, "Мир")
TEXT_TRS_END()

// Переводы для локализации.
TRANSLATIONS_BEGIN(trs_main)
TRANSLATION(TR_LANG_ID_EN, text_trs_en)
TRANSLATION(TR_LANG_ID_RU, text_trs_ru)
TRANSLATIONS_END()


// Инициализация локализации.
localization_init(trs_main, TRANSLATIONS_COUNT(trs_main));

// Установка английского языка.
localization_set_lang(TR_LANG_ID_EN);

printf("%s, %s!\n", TR(TR_ID_HELLO), TR(TR_ID_WORLD));

// Установка русского языка.
localization_set_lang(TR_LANG_ID_RU);

printf("%s, %s!\n", TR(TR_ID_HELLO), TR(TR_ID_WORLD));

*/

#endif /* LOCALIZATION_H */
