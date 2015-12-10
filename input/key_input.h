/**
 * @file key_input.h Библиотека работы с клавиатурным вводом.
 */

#ifndef KEY_INPUT_H
#define	KEY_INPUT_H

#include "keys.h"
#include <stdint.h>
#include <stddef.h>
#include "errors/errors.h"
#include "defs/defs.h"



// Модификаторы ввода.
//! Отсутствие.
#define KEYMOD_NONE   0
//! Shift.
#define KEYMOD_SHIFT  1
//! Ctrl.
#define KEYMOD_CTRL   2
//! Alt.
#define KEYMOD_ALT    4
//! Caps.
#define KEYMOD_CAPS   8
//! Scroll.
#define KEYMOD_SCROLL 16
//! Num.
#define KEYMOD_NUM    32

//! Тип модификатора ввода.
typedef uint8_t keymod_t;
//! Тип модификаторов.
typedef uint8_t keymods_t;

//! Тип символа клавиши.
typedef uint16_t keysym_t;

//! Тип каллбэка нажатия клавиши.
typedef void (*key_input_on_pressed_callback_t)(key_t key);

//! Тип каллбэка отпускания клавиши.
typedef void (*key_input_on_released_callback_t)(key_t key);

//! Тип состояния клавиши.
typedef enum _KeyState {
    KEY_RELEASED = 0, //!< Клавиша отпущена.
    KEY_PRESSED = 1 //!< Клавиша нажата.
} key_state_t;

//! Размер раскладки клавиатуры.
#define KEY_INPUT_LAYOUT_SIZE (KEY_MAX + 1)

//! Тип идентификатора раскладки клавиатуры.
typedef int key_layout_id_t;

//! Тип раскладки клавиатуры.
typedef struct _KeyLayout {
    const char* name; //!< Имя раскладки.
    key_layout_id_t id; //!< Идентификатор раскладки.
    const wchar_t map_normal[KEY_INPUT_LAYOUT_SIZE]; //!< Карта раскладки без модификаторов.
    const wchar_t map_shifted[KEY_INPUT_LAYOUT_SIZE]; //!< Карта раскладки при нажатом shift.
} key_layout_t;

/**
 * Инициализирует клавиатурный ввод.
 * @return Код ошибки.
 */
extern err_t key_input_init(void);

/**
 * Обработчик нажатия клавиши.
 * @param key Код клавиши.
 * @return Код ошибки.
 */
extern err_t key_input_pressed(key_t key);

/**
 * Обработчик отпускания клавиши.
 * @param key Код клавиши.
 * @return Код ошибки.
 */
extern err_t key_input_released(key_t key);

/**
 * Получает состояние клавиши.
 * @param key Код клавиши.
 * @return Состояние клавиши.
 */
extern key_state_t key_input_key_state(key_t key);

/**
 * Получает состояние модификатора.
 * @param mod Код модификатора.
 * @return Состояние модификаторы.
 */
extern key_state_t key_input_mod_state(keymod_t mod);

/**
 * Получает состояние модификаторов.
 * @return Состояние модификаторов.
 */
extern keymods_t key_input_mods(void);

/**
 * Устанавливает раскладки клавиатуры.
 * @param layouts Раскладки клавиатуры.
 * @param count Число раскладок.
 * @return Код ошибки.
 */
extern err_t key_input_set_layouts(const key_layout_t** layouts, size_t count);

/**
 * Получает текущую раскладку клавиатуры.
 * @return Текущая раскладка клавиатуры.
 */
extern const key_layout_t* key_input_current_layout(void);

/**
 * Получает текущую раскладку клавиатуры по индексу.
 * @param index Индекс раскладки.
 * @return Текущая раскладка клавиатуры.
 */
extern const key_layout_t* key_input_layout_by_index(size_t index);

/**
 * Получает имя раскладки клавиатуры.
 * @param layout Раскладка клавиатуры.
 * @return Имя раскладки клавиатуры.
 */
ALWAYS_INLINE static const char* key_layout_name(const key_layout_t* layout)
{
    return layout->name;
}

/**
 * Получает идентификатор раскладки клавиатуры.
 * @param layout Раскладка клавиатуры.
 * @return Идентификатор раскладки клавиатуры.
 */
ALWAYS_INLINE static key_layout_id_t key_layout_id(const key_layout_t* layout)
{
    return layout->id;
}

/**
 * Получает карту раскладки клавиатуры.
 * @param layout Раскладка клавиатуры.
 * @return Карта раскладки клавиатуры.
 */
ALWAYS_INLINE static const wchar_t* key_layout_map_normal(const key_layout_t* layout)
{
    return layout->map_normal;
}

/**
 * Получает карту раскладки клавиатуры с нажатым shift.
 * @param layout Раскладка клавиатуры.
 * @return Карта раскладки клавиатуры.
 */
ALWAYS_INLINE static const wchar_t* key_layout_map_shifted(const key_layout_t* layout)
{
    return layout->map_shifted;
}

/**
 * Получает индекс текущей раскладки клавиатуры.
 * @return Индекс текущей раскладки клавиатуры.
 */
extern size_t key_input_current_layout_index(void);

/**
 * Устанавливает текущую раскладку по индексу.
 * @param index Индекс раскладки.
 * @return Код ошибки.
 */
extern err_t key_input_set_layout_by_index(size_t index);

/**
 * Устанавливает текущую раскладку по идентификатору.
 * @param id Идентификатор раскладки.
 * @return Код ошибки.
 */
extern err_t key_input_set_layout_by_id(key_layout_id_t id);

/**
 * Устанавливает текущую раскладку по имени.
 * @param name Имя раскладки.
 * @return Код ошибки.
 */
extern err_t key_input_set_layout_by_name(const char* name);

/**
 * Устанавливает следующую раскладку.
 * @return Код ошибки.
 */
extern err_t key_input_next_layout(void);

/**
 * Устанавливает предыдущую раскладку.
 * @return Код ошибки.
 */
extern err_t key_input_prev_layout(void);

/**
 * Получает символ по коду клавиши.
 * @param key Код клавиши.
 * @return Символ, 0 при ошибке.
 */
extern wchar_t key_input_key_to_char(key_t key);

/**
 * Получает каллбэк нажатия клавиши.
 * @return Каллбэк нажатия клавиши.
 */
extern key_input_on_pressed_callback_t key_input_on_pressed_callback(void);

/**
 * Устанавливает каллбэк нажатия клавиши.
 * @return Каллбэк нажатия клавиши.
 */
extern void key_input_set_on_pressed_callback(key_input_on_pressed_callback_t callback);

/**
 * Получает каллбэк отпускания клавиши.
 * @return Каллбэк отпускания клавиши.
 */
extern key_input_on_released_callback_t key_input_on_released_callback(void);

/**
 * Устанавливает каллбэк отпускания клавиши.
 * @return Каллбэк отпускания клавиши.
 */
extern void key_input_set_on_released_callback(key_input_on_released_callback_t callback);

#endif	/* KEY_INPUT_H */
