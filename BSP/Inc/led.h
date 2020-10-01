#ifndef _BSP_LED_H
#define _BSP_LED_H

#include "io_handle.h"

/**
 * @brief Initialize a LED component
 *
 * @param io_handle The IO handle to the LED
 *
 * @retval none
 */
void led_init(const struct io_handle *io_handle);

/**
 * @brief Blink the LED
 *
 * @param io_handle The IO handle to the LED
 *
 * @retval none
 */
void led_blink(const struct io_handle *io_handle);

/**
 * @brief Turn the LED on
 *
 * @param io_handle The IO handle to the LED
 *
 * @retval none
 */
void led_on(const struct io_handle *io_handle);

/**
 * @brief Turn the LED off
 *
 * @param io_handle The IO handle to the LED
 *
 * @retval none
 */
void led_off(const struct io_handle *io_handle);

#endif /* _BSP_LED_H */
