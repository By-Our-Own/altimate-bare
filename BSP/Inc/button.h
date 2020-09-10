#ifndef _BSP_BUTTON_H
#define _BSP_BUTTON_H

#include <stdint.h>
#include "iohandle.h"

void button_init(const struct iohandle *iohandle);
uint32_t button_pressed(const struct iohandle *iohandle);

#endif /* _BSP_BUTTON_H */
