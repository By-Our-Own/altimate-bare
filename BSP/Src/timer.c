#include "timer.h"
#include "main.h"

static callback_fn h_isr = 0;

int timer_configure(uint32_t timer_freq,
		    uint32_t auto_reload,
		    callback_fn h_timer_expired)
{
	uint32_t prescaler;

	/* Set timer event interrupt */
	if (auto_reload > 65535) {
		return -1;
	}

	prescaler = __LL_TIM_CALC_PSC(SystemCoreClock, timer_freq);
	if (prescaler == 0U) {
		return -1;
	}
	LL_TIM_SetPrescaler(TIM6, prescaler);
	LL_TIM_SetAutoReload(TIM6, auto_reload - 1U);

	h_isr = h_timer_expired;
	if (h_isr) {
		/* Enable the update interrupt */
		LL_TIM_EnableIT_UPDATE(TIM6);

		/* Configure the NVIC to handle TIM6 update interrupt */
		NVIC_SetPriority(TIM6_IRQn, 0);
		NVIC_EnableIRQ(TIM6_IRQn);
	}

	return 0;
}

void timer_start(void)
{
	/* Start the timer counter */
	LL_TIM_EnableCounter(TIM6);

	/* Enable the auto-reload preload */
	LL_TIM_EnableARRPreload(TIM6);

	/* Force an Update Event */
	LL_TIM_GenerateEvent_UPDATE(TIM6);
}

void timer_stop(void)
{
	/* Start the timer counter */
	LL_TIM_DisableCounter(TIM6);
}

void TIM6_IRQHandler(void)
{
	if (LL_TIM_IsActiveFlag_UPDATE(TIM6) == 1) {
		LL_TIM_ClearFlag_UPDATE(TIM6);

		if (h_isr) {
			h_isr();
		}
	}
}
