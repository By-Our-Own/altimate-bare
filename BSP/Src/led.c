#include "led.h"
#include "gpio.h"

void led_init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    /**/
    LL_GPIO_ResetOutputPin(LD2_GPIO_Port, LD2_Pin);

    /* Initialize GPIO Pin */
    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

void led_blink(void)
{
    LL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

void led_on(void)
{
    LL_GPIO_SetOutputPin(LD2_GPIO_Port, LD2_Pin);
}

void led_off(void)
{
    LL_GPIO_ResetOutputPin(LD2_GPIO_Port, LD2_Pin);
}
