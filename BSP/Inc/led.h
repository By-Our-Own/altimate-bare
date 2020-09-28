#ifndef _BSP_LED_H
#define _BSP_LED_H

#include "iohandle.h"

/**
 * @brief Initialize a LED component
 *
 * @param iohandle The IO handle to the LED
 *
 * @retval none
 */
void led_init(const struct iohandle *iohandle);

/**
 * @brief Blink the LED
 *
 * @param iohandle The IO handle to the LED
 *
 * @retval none
 */
void led_blink(const struct iohandle *iohandle);

/**
 * @brief Turn the LED on
 *
 * @param iohandle The IO handle to the LED
 *
 * @retval none
 */
void led_on(const struct iohandle *iohandle);

/**
 * @brief Turn the LED off
 *
 * @param iohandle The IO handle to the LED
 *
 * @retval none
 */
void led_off(const struct iohandle *iohandle);

#endif /* _BSP_LED_H */
