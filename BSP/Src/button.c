#include "button.h"

void button_init(const struct iohandle *iohandle)
{
    iohandle->config(iohandle);
}

uint8_t button_pressed(const struct iohandle *iohandle)
{
    return !iohandle->get(iohandle);
}

uint8_t button_filter(struct iohandle *iohandle)
{
    iohandle->debouncing_accumulator <<= 1U;
    if (!iohandle->get(iohandle)) {
        iohandle->debouncing_accumulator |= iohandle->debouncing_val;
    } else {
        iohandle->debouncing_accumulator &= ~iohandle->debouncing_val;
    }
    return (iohandle->debouncing_accumulator == (uint8_t)~0U) ? 1U : 0U;
}

