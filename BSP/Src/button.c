#include "button.h"

#define BUTTON_RESPONSE_TIME_MS 40U /* Should be in (10, 50] msec */

void button_init(struct iohandle *iohandle, uint8_t debounce_order)
{
    if (iohandle->gpio_dir == GPIO_INPUT) {
        if (debounce_order > 0 && debounce_order <= 8) {
            iohandle->debouncing_order = debounce_order;
            iohandle->debouncing_accumulator = 0U;
            iohandle->debouncing_val = (1U << (8U - debounce_order + 1U)) - 1U;
        } else {
            iohandle->debouncing_order = 0U;
        }
    }

    iohandle->config(iohandle);
}

uint8_t button_pressed(const struct iohandle *iohandle)
{
    return !iohandle->get(iohandle);
}

uint8_t button_filter(struct iohandle *iohandle)
{
    if (iohandle->debouncing_order) {
        iohandle->debouncing_accumulator <<= 1U;
        if (!iohandle->get(iohandle)) {
            iohandle->debouncing_accumulator |= iohandle->debouncing_val;
        } else {
            iohandle->debouncing_accumulator &= ~iohandle->debouncing_val;
        }
        return (iohandle->debouncing_accumulator == (uint8_t)~0U) ? 1U : 0U;
    }

    return button_pressed(iohandle);
}

uint8_t button_debounce_time_ms(const struct iohandle *iohandle)
{
    if (iohandle->debouncing_order == 0U) {
        return 0U;
    }
    return BUTTON_RESPONSE_TIME_MS / iohandle->debouncing_order;
}

