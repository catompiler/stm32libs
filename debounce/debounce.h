/**
 * @file debounce.h Библиотека устранения дребезга величин.
 */

#ifndef DEBOUNCE_H_
#define DEBOUNCE_H_

#include "defs/defs.h"
#include <stdint.h>
#include "fixed/fixed32.h"
#include <stdbool.h>

/**
 * Тип структуры устранения дребезга величин.
 */
typedef struct _Debounce {
    fixed32_t timeout; //!< Тайм-аут изменения величины.
    fixed32_t time; //!< Время с последнего изменения величины.
    bool changed; //!< Флаг изменения состояния величины.
    bool state; //!< Текущее состояние величины.
} debounce_t;

/**
 * Инициализирует устранение дребезга.
 * @param debounce Устранение дребезга.
 */
EXTERN void debounce_init(debounce_t* debounce);

/**
 * Сбрасывает устранение дребезга.
 * @param debounce Устранение дребезга.
 */
EXTERN void debounce_reset(debounce_t* debounce);

/**
 * Инициализирует устранение дребезга.
 * @param debounce Устранение дребезга.
 * @param deadtime Тайм-аут.
 */
EXTERN void debounce_init_timeout(debounce_t* debounce, fixed32_t timeout);

/**
 * Устанавливает тайм-аут.
 * @param debounce Устранение дребезга.
 * @param timeout Тайм-аут.
 */
EXTERN void debounce_set_timeout(debounce_t* debounce, fixed32_t timeout);

/**
 * Выполняет обработку состояния величины.
 * @param debounce Устранение дребезга.
 * @param state Состояние величины.
 * @param dt Интервал времени с последней обработки.
 * @return Флаг изменения состояния величины.
 */
EXTERN bool debounce_process(debounce_t* debounce, bool state, fixed32_t dt);

/**
 * Получает флаг изменения состояния величины.
 * @param debounce Устранение дребезга.
 * @return Флаг изменения состояния величины.
 */
EXTERN bool debounce_state_changed(debounce_t* debounce);

/**
 * Получает флаг стабильности состояния величины.
 * @param debounce Устранение дребезга.
 * @return Флаг стабильности состояния величины.
 */
EXTERN bool debounce_state_stable(debounce_t* debounce);

/**
 * Получает состояние величины.
 * @param debounce Устранение дребезга.
 * @return Состояние величины.
 */
EXTERN bool debounce_state(debounce_t* debounce);

#endif /* DEBOUNCE_H_ */
