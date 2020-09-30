#include "iohandle.h"
#include "gpio.h"

static void __config(const struct iohandle *this);
static uint32_t __get(const struct iohandle *this);
static void __set(const struct iohandle *this);
static void __reset(const struct iohandle *this);
static void __toggle(const struct iohandle *this);

static void __config_bus(const struct iohandle_bus *this);
static void __write_bus(const struct iohandle_bus *this, uint16_t value);
static uint16_t __read_bus(const struct iohandle_bus *this);
static void __set_bus(const struct iohandle_bus *this, uint16_t pinmask);
static void __reset_bus(const struct iohandle_bus *this, uint16_t pinmask);

void iohandle_init(struct iohandle *iohandle,
                    GPIO_TypeDef *gpio_port,
                    uint32_t gpio_pin,
                    enum gpio_direction direction)
{
    iohandle->gpio_port = gpio_port;
    iohandle->gpio_pin = gpio_pin;
    iohandle->gpio_dir = direction;

    iohandle->config = __config;

    iohandle->get = __get;
    iohandle->set = __set;
    iohandle->reset = __reset;
    iohandle->toggle = __toggle;
}

static void __config(const struct iohandle *this)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    if (this->gpio_port == GPIOA) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    else if (this->gpio_port == GPIOB) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    else if (this->gpio_port == GPIOC) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    else if (this->gpio_port == GPIOD) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD);
    else if (this->gpio_port == GPIOE) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOE);
    else if (this->gpio_port == GPIOH) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);

    /**/
    if (this->gpio_dir == GPIO_OUTPUT) {
        LL_GPIO_ResetOutputPin(this->gpio_port, this->gpio_pin);

        /* Initialize GPIO Pin */
        GPIO_InitStruct.Pin = this->gpio_pin;
        GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(this->gpio_port, &GPIO_InitStruct);
    } else {
        LL_GPIO_SetPinPull(this->gpio_port, this->gpio_pin, LL_GPIO_PULL_UP);
        LL_GPIO_SetPinMode(this->gpio_port, this->gpio_pin, LL_GPIO_MODE_INPUT);
    }
}

static uint32_t __get(const struct iohandle *this)
{
    return LL_GPIO_IsInputPinSet(this->gpio_port, this->gpio_pin);
}

static void __set(const struct iohandle *this)
{
    LL_GPIO_SetOutputPin(this->gpio_port, this->gpio_pin);
}

static void __reset(const struct iohandle *this)
{
    LL_GPIO_ResetOutputPin(this->gpio_port, this->gpio_pin);
}

static void __toggle(const struct iohandle *this)
{
    LL_GPIO_TogglePin(this->gpio_port, this->gpio_pin);
}

void iohandle_bus_init(struct iohandle_bus *iohandle,
                       GPIO_TypeDef *gpio_port,
                       uint32_t gpio_pin_first,
                       uint32_t gpio_pin_last,
                       enum gpio_direction direction)
{
    iohandle->gpio_port = gpio_port;
    iohandle->gpio_pin_first = gpio_pin_first;
    iohandle->gpio_pin_last = gpio_pin_last;
    iohandle->gpio_dir = direction;

    /* Find offset of the first pin
       For example the offset of the first pin of this pinmask 00011100 is 2
                                                                    ^
    */
    uint8_t offset = 0U;
    for (uint16_t count = 1U; count != gpio_pin_first; count <<= 1U) {
        offset++;
    }
    iohandle->gpio_pin_offset = offset;

    /* Find pinmask */
    uint32_t pinmask = 0U;
    for (uint32_t pin = gpio_pin_first; pin <= gpio_pin_last; pin <<= 1U) {
        pinmask |= pin;
    }
    iohandle->gpio_pinmask = pinmask;

    iohandle->config = __config_bus;

    iohandle->write = __write_bus;
    iohandle->read = __read_bus;
    iohandle->set = __set_bus;
    iohandle->reset = __reset_bus;
}

static void __config_bus(const struct iohandle_bus *this)
{
    uint32_t mode, pull;

    /* GPIO Ports Clock Enable */
    if (this->gpio_port == GPIOA) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    else if (this->gpio_port == GPIOB) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    else if (this->gpio_port == GPIOC) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    else if (this->gpio_port == GPIOD) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD);
    else if (this->gpio_port == GPIOE) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOE);
    else if (this->gpio_port == GPIOH) LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);

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
        LL_GPIO_SetPinSpeed(this->gpio_port, pin, LL_GPIO_SPEED_FREQ_LOW);
        LL_GPIO_SetPinOutputType(this->gpio_port, pin, LL_GPIO_OUTPUT_PUSHPULL);
        LL_GPIO_SetPinPull(this->gpio_port, pin, pull);
        pin <<= 1U;
    } while (pin <= this->gpio_pin_last);
}

static void __write_bus(const struct iohandle_bus *this, uint16_t value)
{
    /* Prepare the output register */
    uint32_t reg = LL_GPIO_ReadOutputPort(this->gpio_port);
    reg &= ~this->gpio_pinmask; /* Clear the bus related bits from the output */
    reg |= ((uint32_t)value) << this->gpio_pin_offset;  /* Update register */

    LL_GPIO_WriteOutputPort(this->gpio_port, reg);
}

static uint16_t __read_bus(const struct iohandle_bus *this)
{
    uint32_t reg = LL_GPIO_ReadOutputPort(this->gpio_port);
    reg &= this->gpio_pinmask;  /* Get the bus related bits from the output */

    return reg >> this->gpio_pin_offset;    /* Return the bus bits */
}

static void __set_bus(const struct iohandle_bus *this, uint16_t pinmask)
{
    LL_GPIO_SetOutputPin(this->gpio_port,
                         ((uint32_t)pinmask) << this->gpio_pin_offset);
}

static void __reset_bus(const struct iohandle_bus *this, uint16_t pinmask)
{
    LL_GPIO_ResetOutputPin(this->gpio_port,
                           ((uint32_t)pinmask) << this->gpio_pin_offset);
}
