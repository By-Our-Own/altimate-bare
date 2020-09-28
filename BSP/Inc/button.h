#ifndef _BSP_BUTTON_H
#define _BSP_BUTTON_H

#include <stdint.h>
#include "iohandle.h"

/**
 * @brief Initialize a button component
 *
 * @param iohandle The IO handle to the button
 *
 * @retval none
 */
void button_init(const struct iohandle *iohandle);

/**
 * @brief Check if button is pressed
 *
 * @param iohandle The IO handle to the button
 *
 * @retval 1 if pressed
 * @retval 0 otherwise
 */
uint8_t button_pressed(const struct iohandle *iohandle);

/**
 * @brief Check if debounced button is pressed
 *
 * @param iohandle The IO handle to the button
 *
 * @retval 1 if pressed
 * @retval 0 otherwise
 */
uint8_t button_filter(struct iohandle *iohandle);

#endif /* _BSP_BUTTON_H */
