#ifndef _BSP_BUTTON_H
#define _BSP_BUTTON_H

#include <stdint.h>
#include "io_handle.h"

/**
 * @brief Initialize a button component
 *
 * @param io_handle          The IO handle to the button
 * @param debounce_order    Times to filter during debouncing: (0, 8]\
 *                          0 for no debouncing
 *
 * @retval none
 */
void button_init(struct io_handle *io_handle, uint8_t debounce_order);

/**
 * @brief Check if button is pressed withoud debouncing
 *
 * @param io_handle The IO handle to the button
 *
 * @retval 1 if pressed
 * @retval 0 otherwise
 */
uint8_t button_pressed(const struct io_handle *io_handle);

/**
 * @brief Check if debounced button is pressed
 * @note If debouncing is disabled, a call to button_pressed() is returned
 *
 * @param io_handle The IO handle to the button
 *
 * @retval 1 if pressed
 * @retval 0 otherwise
 */
uint8_t button_filter(struct io_handle *io_handle);

/**
 * @brief Get the time the button needs to be debounced in milliseconds
 *
 * @param io_handle The IO handle to the button
 *
 * @return Time in milliseconds
 */
uint8_t button_debounce_time_ms(const struct io_handle *io_handle);

#endif /* _BSP_BUTTON_H */
