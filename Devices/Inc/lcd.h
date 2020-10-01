#ifndef _DEVICES_LCD_H
#define _DEVICES_LCD_H

#include "io_handle.h"

/**
 * @brief Initialize a LCD device
 * @note The driver works in 4bit data length mode
 * @note Screen configuration implies 2 lines, no cursor, no blinking
 * @note The driver commands work with time delays and no waiting
 *
 * The driver needs the bus to have this map:
 *
 *     Last                        First
 * BUS   7   6   5   4   3   2   1   0
 *      BL  EN  RS  R/W DB7 DB6 DB5 DB4
 *
 * @param[in] conn The IO bus that LCD uses
 *
 * @retval none
 */
void lcd_init(const struct io_handle_bus *conn);

/**
 * @brief Turn on/off the backlight
 * @note The backlight pin should be connected
 *
 * @param on 1 to turn the backlight on, 0 otherwise
 *
 * @retval none
 */
void lcd_backlight(uint8_t on);

/**
 * @brief Put a character on the display
 *
 * @param c The character to display
 *
 * @retval none
 */
void lcd_putc(uint8_t c);

/**
 * @brief Clear the display
 *
 * @retval none
 */
void lcd_clear(void);

/**
 * @brief Display a message
 *
 * @param x Place in a line to display the message
 * @param y Number of line to display the message
 * @param msg The message to display
 *
 * @retval none
 */
void lcd_display(int x, int y, const char *msg);

#endif /* _DEVICES_LCD_H */
