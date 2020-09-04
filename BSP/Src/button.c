#include "button.h"
#include "gpio.h"

void button_init(void)
{
    /* Configure IO */
    LL_GPIO_SetPinPull(B1_GPIO_Port, B1_Pin, LL_GPIO_PULL_UP);
    LL_GPIO_SetPinMode(B1_GPIO_Port, B1_Pin, LL_GPIO_MODE_INPUT);
}

uint8_t button_pressed(void)
{
    return !LL_GPIO_IsInputPinSet(B1_GPIO_Port, B1_Pin);
}
