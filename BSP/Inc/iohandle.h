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

struct iohandle_bus {
    GPIO_TypeDef *gpio_port;
    uint32_t gpio_pin_first;
    uint32_t gpio_pin_last;
    uint32_t gpio_pinmask;
    uint8_t gpio_pin_offset;
    enum gpio_direction gpio_dir;
    uint8_t debouncing_order;
    uint8_t debouncing_accumulator;
    uint8_t debouncing_val;

    void (*config)(const struct iohandle_bus *);

    void (*write)(const struct iohandle_bus *, uint16_t);
    uint16_t (*read)(const struct iohandle_bus *);
    void (*set)(const struct iohandle_bus *, uint16_t);
    void (*reset)(const struct iohandle_bus *, uint16_t);
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

/**
 * @brief Initialize an IO bus
 *
 * The bus is assosiated with one GPIO port and a set of continuous pins:
 * GPIOx[FIRST:LAST].
 *
 * @note The direction is the same for all the pins and yet it cannot change.
 *
 * @param[out]  iohandle_bus    The handler of the IO bus
 * @param[in]   gpio_port       The GPIO Port of the IO bus
 * @param       gpio_pin_first  The GPIO Pin number of the first IO pin
 * @param       gpio_pin_last   The GPIO Pin number of the last IO pin
 * @param       gpio_direction  The direction of the IO bus
 *
 * @return none
 */
void iohandle_bus_init(struct iohandle_bus *iohandle,
                       GPIO_TypeDef *gpio_port,
                       uint32_t gpio_pin_first,
                       uint32_t gpio_pin_last,
                       enum gpio_direction direction);

#endif /* _BSP_IOHANDLE_H */
