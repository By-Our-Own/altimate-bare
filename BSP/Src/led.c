#include "led.h"

void led_init(const struct io_handle *io_handle)
{
	io_handle->config(io_handle);
}

void led_blink(const struct io_handle *io_handle)
{
	io_handle->toggle(io_handle);
}

void led_on(const struct io_handle *io_handle)
{
	io_handle->set(io_handle);
}

void led_off(const struct io_handle *io_handle)
{
	io_handle->reset(io_handle);
}
