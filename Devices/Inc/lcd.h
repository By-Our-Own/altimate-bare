#ifndef _DEVICES_LCD_H
#define _DEVICES_LCD_H

#include "iohandle.h"

enum lcd_pins {
    LCD_E = 0,
    LCD_BL,
    LCD_RS,
    LCD_RW,
    LCD_DB7,
    LCD_DB6,
    LCD_DB5,
    LCD_DB4,
    LCD_DB3,
    LCD_DB2,
    LCD_DB1,
    LCD_DB0,
    LCD_MAX_PINS,
};

void lcd_init(const struct iohandle (*conn)[]);
void lcd_backlight(uint8_t on);
// void lcd_wait(const struct iohandle *iohandle);
void lcd_putc(uint8_t c);
void lcd_clear(void);
void lcd_display(int x, int y, const char *msg);

#endif /* _DEVICES_LCD_H */
