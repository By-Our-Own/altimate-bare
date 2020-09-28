#ifndef _BSP_IOHANDLE_H
#define _BSP_IOHANDLE_H

#include "gpio.h"
#include <stdint.h>

enum gpio_direction { GPIO_INPUT, GPIO_OUTPUT };

struct iohandle {
    GPIO_TypeDef *gpio_port;
    uint32_t gpio_pin;
    enum gpio_direction gpio_dir;
    uint8_t debouncing_order;
    uint8_t debouncing_accumulator;
    uint8_t debouncing_val;

    void (*config)(const struct iohandle *);

    uint32_t (*get)(const struct iohandle *);
    void (*set)(const struct iohandle *);
    void (*reset)(const struct iohandle *);
    void (*toggle)(const struct iohandle *);
};

/**
 * @brief Initialize an IO pin
 *
 * @param[out]  iohandle        The handler of the IO pin
 * @param[in]   gpio_port       The GPIO Port of the IO pin
 * @param       gpio_pin        The GPIO Pin number of the IO pin
 * @param       gpio_direction  The direction of the IO pin
 *
 * @return none
 */
void iohandle_init(struct iohandle *iohandle,
                   GPIO_TypeDef *gpio_port,
                   uint32_t gpio_pin,
                   enum gpio_direction direction);

#endif /* _BSP_IOHANDLE_H */
