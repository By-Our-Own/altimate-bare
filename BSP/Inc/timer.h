#ifndef _BSP_TIMER_H
#define _BSP_TIMER_H

#include <stdint.h>

typedef void (*callback_fn)(void);

int timer_configure(uint32_t timer_freq, uint32_t auto_reload, callback_fn h_timer_expired);
void timer_start(void);
void timer_stop(void);


#endif /* _BSP_TIMER_H */
