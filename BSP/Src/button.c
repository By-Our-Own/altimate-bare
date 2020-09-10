#include "button.h"

void button_init(const struct iohandle *iohandle)
{
    iohandle->config(iohandle);
}

uint32_t button_pressed(const struct iohandle *iohandle)
{
    return !iohandle->get(iohandle);
}
