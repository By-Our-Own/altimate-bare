#ifndef _BSP_UTILS_H
#define _BSP_UTILS_H

#include <stdint.h>

/**
 * @brief Cause a delay in millisecond granularity
 *
 * @param ms Milliseconds to delay
 *
 * @retval none
 */
void delay_ms(uint32_t ms);

/**
 * @brief Return the milliseconds passed from the start of execution
 * @note Typically returns the milliseconds passed from the last timer rollover
 *
 * @return Rime in milliseconds
 */
uint32_t millis(void);

#endif /* _BSP_UTILS_H */
