#ifndef _BSP_LED_H
#define _BSP_LED_H

#include "iohandle.h"

void led_init(const struct iohandle *iohandle);
void led_blink(const struct iohandle *iohandle);
void led_on(const struct iohandle *iohandle);
void led_off(const struct iohandle *iohandle);

#endif /* _BSP_LED_H */
