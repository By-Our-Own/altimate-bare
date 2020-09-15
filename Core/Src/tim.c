/**
  ******************************************************************************
  * File Name          : tim.c
  * Description        : This file provides code for the configuration
  *                      of all used TIM perpheral.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"
/** Configure timers
*/
void MX_TIM_Init(void)
{
    /* Initialize timer */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);

}
