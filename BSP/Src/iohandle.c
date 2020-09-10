#include "iohandle.h"
#include "gpio.h"

static void __config(const struct iohandle *this);
static uint32_t __get(const struct iohandle *this);
static void __set(const struct iohandle *this);
static void __reset(const struct iohandle *this);
static void __toggle(const struct iohandle *this);

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
