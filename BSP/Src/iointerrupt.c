#include "iointerrupt.h"
#include "main.h"

static uint32_t syscfg_exti_ports[6] = {
	LL_SYSCFG_EXTI_PORTA,
	LL_SYSCFG_EXTI_PORTB,
	LL_SYSCFG_EXTI_PORTC,
	LL_SYSCFG_EXTI_PORTD,
	LL_SYSCFG_EXTI_PORTE,
	LL_SYSCFG_EXTI_PORTH,
};

static uint32_t syscfg_exti_lines[16] = {
	LL_SYSCFG_EXTI_LINE0,
	LL_SYSCFG_EXTI_LINE1,
	LL_SYSCFG_EXTI_LINE2,
	LL_SYSCFG_EXTI_LINE3,
	LL_SYSCFG_EXTI_LINE4,
	LL_SYSCFG_EXTI_LINE5,
	LL_SYSCFG_EXTI_LINE6,
	LL_SYSCFG_EXTI_LINE7,
	LL_SYSCFG_EXTI_LINE8,
	LL_SYSCFG_EXTI_LINE9,
	LL_SYSCFG_EXTI_LINE10,
	LL_SYSCFG_EXTI_LINE11,
	LL_SYSCFG_EXTI_LINE12,
	LL_SYSCFG_EXTI_LINE13,
	LL_SYSCFG_EXTI_LINE14,
	LL_SYSCFG_EXTI_LINE15,
};

static uint32_t exti_lines[16] = {
	LL_EXTI_LINE_0,
	LL_EXTI_LINE_1,
	LL_EXTI_LINE_2,
	LL_EXTI_LINE_3,
	LL_EXTI_LINE_4,
	LL_EXTI_LINE_5,
	LL_EXTI_LINE_6,
	LL_EXTI_LINE_7,
	LL_EXTI_LINE_8,
	LL_EXTI_LINE_9,
	LL_EXTI_LINE_10,
	LL_EXTI_LINE_11,
	LL_EXTI_LINE_12,
	LL_EXTI_LINE_13,
	LL_EXTI_LINE_14,
	LL_EXTI_LINE_15,
};

static uint8_t trigger_types[4] = {
	LL_EXTI_TRIGGER_NONE,
	LL_EXTI_TRIGGER_FALLING,
	LL_EXTI_TRIGGER_RISING,
	LL_EXTI_TRIGGER_RISING_FALLING,
};

static callback_fn h_isr[16] = { 0 };

void ioint_configure(uint8_t port,
		     uint8_t pin,
		     enum trigger_type trigger,
		     uint8_t trigger_state,
		     callback_fn callback)
{
	(void) port;

	LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

	/* Connect External Line to the GPIO */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);

	/* Enable a EXTI line interupt according to port and trigger type */
	LL_SYSCFG_SetEXTISource(syscfg_exti_ports[port],
				syscfg_exti_lines[pin]);

	EXTI_InitStruct.Line_0_31 = exti_lines[pin];
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
	EXTI_InitStruct.Trigger = trigger_types[trigger];
	LL_EXTI_Init(&EXTI_InitStruct);

	if (callback) {
		h_isr[pin] = callback;
	}
}

void ioint_enable(uint8_t port, uint8_t pin)
{
	if (pin <= 15 && pin >= 4) {
		/* Configure NVIC for EXTI4_15_IRQn */
		NVIC_EnableIRQ(EXTI4_15_IRQn);
		NVIC_SetPriority(EXTI4_15_IRQn, 0);
	} else
	if (pin >= 2) {
		/* Configure NVIC for EXTI2_3_IRQn */
		NVIC_EnableIRQ(EXTI2_3_IRQn);
		NVIC_SetPriority(EXTI2_3_IRQn, 0);
	} else
	if (pin >= 0) {
		/* Configure NVIC for EXTI0_1_IRQn */
		NVIC_EnableIRQ(EXTI0_1_IRQn);
		NVIC_SetPriority(EXTI0_1_IRQn, 0);
	}
}

void ioint_disable(uint8_t port, uint8_t pin)
{
	if (pin <= 15 && pin >= 4) {
		/* Configure NVIC for EXTI4_15_IRQn */
		NVIC_DisableIRQ(EXTI4_15_IRQn);
	} else
	if (pin >= 2) {
		/* Configure NVIC for EXTI2_3_IRQn */
		NVIC_DisableIRQ(EXTI2_3_IRQn);
	} else
	if (pin >= 0) {
		/* Configure NVIC for EXTI0_1_IRQn */
		NVIC_DisableIRQ(EXTI0_1_IRQn);
	}
}

void EXTI4_15_IRQHandler(void)
{
	uint8_t i;
	for (i = 4; i <= 15; i++) {
		if (LL_EXTI_IsActiveFlag_0_31(exti_lines[i]) != RESET) {
			LL_EXTI_ClearFlag_0_31(exti_lines[i]);
			if (h_isr[i]) {
				h_isr[i]();
			}
		}
	}
}
