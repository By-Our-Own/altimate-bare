#include "io_handle.h"
#include "gpio.h"

static void __config(const struct io_handle *this);
static uint32_t __get(const struct io_handle *this);
static void __set(const struct io_handle *this);
static void __reset(const struct io_handle *this);
static void __toggle(const struct io_handle *this);

static void __config_bus(const struct io_handle_bus *this);
static void __write_bus(const struct io_handle_bus *this, uint16_t value);
static uint16_t __read_bus(const struct io_handle_bus *this);
static void __set_bus(const struct io_handle_bus *this, uint16_t pinmask);
static void __reset_bus(const struct io_handle_bus *this, uint16_t pinmask);

void io_handle_init(struct io_handle *io_handle,
		    GPIO_TypeDef *gpio_port,
		    uint8_t gpio_pin,
		    const struct ioconfig *gpio_config)
{
	io_handle->gpio_port = gpio_port;
	io_handle->gpio_pin = (1U << gpio_pin);	/* convert to STM32 pinval */

	io_handle->dir = gpio_config->dir;
	io_handle->otype = gpio_config->otype;
	io_handle->pull = gpio_config->pull;
	io_handle->speed = gpio_config->speed;
	io_handle->func = gpio_config->func;

	io_handle->config = __config;

	io_handle->get = __get;
	io_handle->set = __set;
	io_handle->reset = __reset;
	io_handle->toggle = __toggle;
}

static void __config(const struct io_handle *this)
{
	/* GPIO Ports Clock Enable */
	if (this->gpio_port == GPIOA) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOA);
	else
	if (this->gpio_port == GPIOB) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOB);
	else
	if (this->gpio_port == GPIOC) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOC);
	else
	if (this->gpio_port == GPIOD) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOD);
	else
	if (this->gpio_port == GPIOE) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOE);
	else
	if (this->gpio_port == GPIOH) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOH);

	/* Set Direction */
	switch (this->dir) {
	case GPIO_OUTPUT:
		LL_GPIO_SetPinMode(this->gpio_port,
				   this->gpio_pin,
				   LL_GPIO_MODE_OUTPUT);
		break;
	case GPIO_INPUT:
		LL_GPIO_SetPinMode(this->gpio_port,
				   this->gpio_pin,
				   LL_GPIO_MODE_INPUT);
		break;
	case GPIO_ALTERNATE:
		LL_GPIO_SetPinMode(this->gpio_port,
				   this->gpio_pin,
				   LL_GPIO_MODE_ALTERNATE);
		if (this->gpio_pin < LL_GPIO_PIN_8) {
			LL_GPIO_SetAFPin_0_7(this->gpio_port,
					     this->gpio_pin,
					     this->func);
		} else {
			LL_GPIO_SetAFPin_8_15(this->gpio_port,
					      this->gpio_pin,
					      this->func);
		}
		break;
	case GPIO_ANALOG:
		LL_GPIO_SetPinMode(this->gpio_port,
				   this->gpio_pin,
				   LL_GPIO_MODE_ANALOG);
		break;
	default:
		return;
	}

	/* Set Output Type */
	switch (this->otype) {
	case GPIO_PUSHPULL:
		LL_GPIO_SetPinOutputType(this->gpio_port,
					 this->gpio_pin,
					 LL_GPIO_OUTPUT_PUSHPULL);
		break;
	case GPIO_OPENDRAIN:
		LL_GPIO_SetPinOutputType(this->gpio_port,
					 this->gpio_pin,
					 LL_GPIO_OUTPUT_OPENDRAIN);
		break;
	default:
		return;
	}

	/* Set Pull */
	switch (this->pull) {
	case GPIO_PULL_NO:
		LL_GPIO_SetPinPull(this->gpio_port,
				   this->gpio_pin,
				   LL_GPIO_PULL_NO);
		break;
	case GPIO_PULL_UP:
		LL_GPIO_SetPinPull(this->gpio_port,
				   this->gpio_pin,
				   LL_GPIO_PULL_UP);
		break;
	case GPIO_PULL_DOWN:
		LL_GPIO_SetPinPull(this->gpio_port,
				   this->gpio_pin,
				   LL_GPIO_PULL_DOWN);
		break;
	default:
		return;
	}

	/* Set Speed */
	switch (this->speed) {
	case GPIO_SPEED_LOW:
		LL_GPIO_SetPinSpeed(this->gpio_port,
				    this->gpio_pin,
				    LL_GPIO_SPEED_FREQ_LOW);
		break;
	case GPIO_SPEED_MEDIUM:
		LL_GPIO_SetPinSpeed(this->gpio_port,
				    this->gpio_pin,
				    LL_GPIO_SPEED_FREQ_MEDIUM);
		break;
	case GPIO_SPEED_HIGH:
		LL_GPIO_SetPinSpeed(this->gpio_port,
				    this->gpio_pin,
				    LL_GPIO_SPEED_FREQ_HIGH);
		break;
	case GPIO_SPEED_VERY_HIGH:
		LL_GPIO_SetPinSpeed(this->gpio_port,
				    this->gpio_pin,
				    LL_GPIO_SPEED_FREQ_VERY_HIGH);
		break;
	default:
		return;
	}
}

static uint32_t __get(const struct io_handle *this)
{
	return LL_GPIO_IsInputPinSet(this->gpio_port, this->gpio_pin);
}

static void __set(const struct io_handle *this)
{
	LL_GPIO_SetOutputPin(this->gpio_port, this->gpio_pin);
}

static void __reset(const struct io_handle *this)
{
	LL_GPIO_ResetOutputPin(this->gpio_port, this->gpio_pin);
}

static void __toggle(const struct io_handle *this)
{
	LL_GPIO_TogglePin(this->gpio_port, this->gpio_pin);
}

void io_handle_bus_init(struct io_handle_bus *io_handle,
			GPIO_TypeDef *gpio_port,
			uint8_t gpio_pin_first,
			uint8_t gpio_pin_last,
			enum gpio_direction direction)
{
	io_handle->gpio_port = gpio_port;
	io_handle->gpio_pin_first = (1U << gpio_pin_first);	/* convert to STM32 pinval */
	io_handle->gpio_pin_last = (1U << gpio_pin_last);	/* convert to STM32 pinval */
	io_handle->gpio_dir = direction;

	/* Find offset of the first pin
	   For example the offset of the first pin of this pinmask 00011100 is 2
	                                                                ^
	 */
	io_handle->gpio_pin_offset = gpio_pin_first;;

	/* Find pinmask */
	uint32_t pinmask = 0U;
	for (uint32_t pin = io_handle->gpio_pin_first;
	     pin <= io_handle->gpio_pin_last;
	     pin <<= 1U) {
		pinmask |= pin;
	}
	io_handle->gpio_pinmask = pinmask;

	io_handle->config = __config_bus;

	io_handle->write = __write_bus;
	io_handle->read = __read_bus;
	io_handle->set = __set_bus;
	io_handle->reset = __reset_bus;
}

static void __config_bus(const struct io_handle_bus *this)
{
	uint32_t mode, pull;

	/* GPIO Ports Clock Enable */
	if (this->gpio_port == GPIOA) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOA);
	else
	if (this->gpio_port == GPIOB) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOB);
	else
	if (this->gpio_port == GPIOC) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOC);
	else
	if (this->gpio_port == GPIOD) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOD);
	else
	if (this->gpio_port == GPIOE) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOE);
	else
	if (this->gpio_port == GPIOH) LL_IOP_GRP1_EnableClock(
			LL_IOP_GRP1_PERIPH_GPIOH);

	/* Configure GPIO bus */
	if (this->gpio_dir == GPIO_OUTPUT) {
		mode = LL_GPIO_MODE_OUTPUT;
		pull = LL_GPIO_PULL_NO;
	} else {
		mode = LL_GPIO_MODE_INPUT;
		pull = LL_GPIO_PULL_UP;
	}

	uint32_t pin = this->gpio_pin_first;
	do {
		LL_GPIO_SetPinMode(this->gpio_port, pin, mode);
		LL_GPIO_SetPinSpeed(this->gpio_port, pin,
				    LL_GPIO_SPEED_FREQ_LOW);
		LL_GPIO_SetPinOutputType(this->gpio_port,
					 pin,
					 LL_GPIO_OUTPUT_PUSHPULL);
		LL_GPIO_SetPinPull(this->gpio_port, pin, pull);
		pin <<= 1U;
	} while (pin <= this->gpio_pin_last);
}

static void __write_bus(const struct io_handle_bus *this, uint16_t value)
{
	/* Prepare the output register */
	uint32_t reg = LL_GPIO_ReadOutputPort(this->gpio_port);
	reg &= ~this->gpio_pinmask;	/* Clear the bus related bits from the output */
	reg |= ((uint32_t)value) << this->gpio_pin_offset;	/* Update register */

	LL_GPIO_WriteOutputPort(this->gpio_port, reg);
}

static uint16_t __read_bus(const struct io_handle_bus *this)
{
	uint32_t reg = LL_GPIO_ReadOutputPort(this->gpio_port);
	reg &= this->gpio_pinmask;	/* Get the bus related bits from the output */

	return reg >> this->gpio_pin_offset;	/* Return the bus bits */
}

static void __set_bus(const struct io_handle_bus *this, uint16_t pinmask)
{
	LL_GPIO_SetOutputPin(this->gpio_port,
			     ((uint32_t)pinmask) << this->gpio_pin_offset);
}

static void __reset_bus(const struct io_handle_bus *this, uint16_t pinmask)
{
	LL_GPIO_ResetOutputPin(this->gpio_port,
			       ((uint32_t)pinmask) << this->gpio_pin_offset);
}
