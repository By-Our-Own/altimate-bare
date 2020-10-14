#ifndef _APP_GUI_H
#define _APP_GUI_H

#include <stdint.h>

/*
 * LCD screen:
 * Welcome:
 * +----------------+
 * |Fucking         |
 * |Booted          |
 * +----------------+
 *
 * Normal:
 * +----------------+
 * |100.743 kPa     |
 * |25.34 oC        |
 * +----------------+
 */

/**
 * @brief Show welcoming message
 *
 * @retval none
 */
void gui_welcome(void);

/**
 * @brief Update values in the GUI
 *
 * @param pressure      The pressure in 100*Pa
 * @param temperature   The temperature in 100*Celsius
 * @param altitude      The height in 10*m
 *
 * @retval none
 */
void gui_update(uint64_t pressure, int64_t temperature, int64_t altitude);

#endif /* _APP_GUI_H */
