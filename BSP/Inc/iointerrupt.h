#ifndef _BSP_IOINTERRUPT_H
#define _BSP_IOINTERRUPT_H

#include <stdint.h>

enum trigger_type {
	IOINT_TRIG_NONE,
	IOINT_TRIG_RISING,
	IOINT_TRIG_FALLING,
	IOINT_TRIG_RISING_FALLING,
} trigger_type;

typedef void (*callback_fn)(void);

void ioint_configure(uint8_t port,
		     uint8_t pin,
		     enum trigger_type trigger,
		     uint8_t trigger_state,
		     callback_fn callback);
void ioint_enable(uint8_t port, uint8_t pin);
void ioint_disable(uint8_t port, uint8_t pin);

#endif /* _BSP_IOINTERRUPT_H */
