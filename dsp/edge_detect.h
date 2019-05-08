/**
 * @file edge_detect.h Детектор фронта сигнала.
 */

#ifndef EDGE_DETECT_H_
#define EDGE_DETECT_H_

#include "errors/errors.h"
#include <stdint.h>
#include <stdbool.h>

//!< Перечисление состояния детектора.
typedef enum _Edge_Detect_State {
    EDGE_DETECT_LOW = 0, //!< Низкий уровень.
    EDGE_DETECT_RISING = 1, //!< Уровень изменился с низкого на высокий - передний фронт.
    EDGE_DETECT_FALLING = 2, //!< Уровень изменился с высокого на низкий - задний фронт.
    EDGE_DETECT_HIGH = 3 //!< Высокий уровень.
} edge_detect_state_t;

//! Структура детектора фронта.
typedef struct _Edge_Detect {
    uint8_t data; //!< Данные детектора (два последних семпла сигнала.
} edge_detect_t;


/**
 * Инициализирует детектор фронта.
 * @param edge_detect Детектор фронта.
 */
extern void edge_detect_init(edge_detect_t* edge_detect);

/**
 * Сбрасывает детектор фронта.
 * @param edge_detect Детектор фронта.
 */
extern void edge_detect_reset(edge_detect_t* edge_detect);

/**
 * Сбрасывает детектор фронта на заданное значение.
 * @param edge_detect Детектор фронта.
 * @param value Значение.
 */
extern void edge_detect_reset_value(edge_detect_t* edge_detect, bool value);

/**
 * Обрабатывает очередное значение.
 * @param edge_detect Детектор фронта.
 * @param value Значение.
 * @return Состояние детектора.
 */
extern edge_detect_state_t edge_detect_put(edge_detect_t* edge_detect, bool value);

/**
 * Получает состояние детектора.
 * @param edge_detect Детектор фронта.
 * @return Состояние детектора.
 */
extern edge_detect_state_t edge_detect_state(edge_detect_t* edge_detect);


#endif /* EDGE_DETECT_H_ */
