#include "button.h"

#define BUTTON_RESPONSE_TIME_MS 40U	/* Should be in (10, 50] msec */

void button_init(struct io_handle *io_handle, uint8_t debounce_order)
{
	if (io_handle->dir == GPIO_INPUT) {
		if (debounce_order > 0 && debounce_order <= 8) {
			io_handle->debouncing_order = debounce_order;
			io_handle->debouncing_accumulator = 0U;
			io_handle->debouncing_val =
				(1U << (8U - debounce_order + 1U)) - 1U;
		} else {
			io_handle->debouncing_order = 0U;
		}
	}

	io_handle->config(io_handle);
}

uint8_t button_pressed(const struct io_handle *io_handle)
{
	return !io_handle->get(io_handle);
}

uint8_t button_filter(struct io_handle *io_handle)
{
	if (io_handle->debouncing_order) {
		io_handle->debouncing_accumulator <<= 1U;
		if (!io_handle->get(io_handle)) {
			io_handle->debouncing_accumulator |=
				io_handle->debouncing_val;
		} else {
			io_handle->debouncing_accumulator &=
				~io_handle->debouncing_val;
		}
		return (io_handle->debouncing_accumulator ==
			(uint8_t) ~0U) ? 1U : 0U;
	}

	return button_pressed(io_handle);
}

uint8_t button_debounce_time_ms(const struct io_handle *io_handle)
{
	if (io_handle->debouncing_order == 0U) {
		return 0U;
	}
	return BUTTON_RESPONSE_TIME_MS / io_handle->debouncing_order;
}
