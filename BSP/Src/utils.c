#include "utils.h"

#include "main.h"
#include "stm32l0xx_it.h"

void delay_ms(uint32_t ms)
{
    LL_mDelay(ms);
}

uint32_t millis(void)
{
    return systick_count;
}
