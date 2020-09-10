#include "led.h"

void led_init(const struct iohandle *iohandle)
{
    iohandle->config(iohandle);
}

void led_blink(const struct iohandle *iohandle)
{
    iohandle->toggle(iohandle);
}

void led_on(const struct iohandle *iohandle)
{
    iohandle->set(iohandle);
}

void led_off(const struct iohandle *iohandle)
{
    iohandle->reset(iohandle);
}
