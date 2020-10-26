#ifndef _SCHEDULER_DEADLINE_H
#define _SCHEDULER_DEADLINE_H

#include <stdint.h>
#include "delay.h"

/**
 * @brief Checks if a deadline has been reached
 *
 * @param dl The deadline to check
 *
 * @retval 1 if deadline has been reached
 * @retval 0 otherwise
 */
static inline uint8_t deadline_reached(uint16_t dl)
{
	return ((int16_t)(millis() - dl)) >= 0 ? 1 : 0;
}

/**
 * @brief Create a new deadline
 *
 * @param t The time to create the deadline
 *
 * @return The new deadline
 */
static inline uint16_t deadline_make(uint16_t t)
{
	return millis() + t;
}

/**
 * @brief Extend a current deadline
 *
 * @param dl The current deadline
 * @param t The time to extend the deadline
 *
 * @return The extended deadline
 */
static inline uint16_t deadline_extend(uint16_t dl, uint16_t t)
{
	return dl + t;
}

#endif /* _SCHEDULER_DEADLINE_H */
