#ifndef _BSP_IO_HANDLE_H
#define _BSP_IO_HANDLE_H

#include "gpio.h"
#include <stdint.h>

enum gpio_direction { GPIO_INPUT=0, GPIO_OUTPUT, GPIO_ALTERNATE, GPIO_ANALOG };
enum gpio_pull { GPIO_PULL_UP=4, GPIO_PULL_DOWN, GPIO_PULL_NO };
enum gpio_output_type { GPIO_OPENDRAIN=7, GPIO_PUSHPULL };
enum gpio_speed { GPIO_SPEED_LOW=9, GPIO_SPEED_MEDIUM, GPIO_SPEED_HIGH, GPIO_SPEED_VERY_HIGH };

struct ioconfig {
    enum gpio_direction dir;
    enum gpio_output_type otype;
    enum gpio_pull pull;
    enum gpio_speed speed;
    uint32_t func;
};

struct io_handle {
    GPIO_TypeDef *gpio_port;
    uint32_t gpio_pin;
    enum gpio_direction dir;
    enum gpio_output_type otype;
    enum gpio_pull pull;
    enum gpio_speed speed;
    uint32_t func;
    uint8_t debouncing_order;
    uint8_t debouncing_accumulator;
    uint8_t debouncing_val;

    void (*config)(const struct io_handle *);

    uint32_t (*get)(const struct io_handle *);
    void (*set)(const struct io_handle *);
    void (*reset)(const struct io_handle *);
    void (*toggle)(const struct io_handle *);
};

struct io_handle_bus {
    GPIO_TypeDef *gpio_port;
    uint32_t gpio_pin_first;
    uint32_t gpio_pin_last;
    uint32_t gpio_pinmask;
    uint8_t gpio_pin_offset;
    enum gpio_direction gpio_dir;
    uint8_t debouncing_order;
    uint8_t debouncing_accumulator;
    uint8_t debouncing_val;

    void (*config)(const struct io_handle_bus *);

    void (*write)(const struct io_handle_bus *, uint16_t);
    uint16_t (*read)(const struct io_handle_bus *);
    void (*set)(const struct io_handle_bus *, uint16_t);
    void (*reset)(const struct io_handle_bus *, uint16_t);
};

/**
 * @brief Initialize an IO pin
 *
 * @param[out]  io_handle       The handler of the IO pin
 * @param[in]   gpio_port       The GPIO Port of the IO pin
 * @param       gpio_pin        The GPIO Pin number of the IO pin
 * @param[in]   gpio_config     The GPIO Pin configuration
 *
 * @return none
 */
void io_handle_init(struct io_handle *io_handle,
                   GPIO_TypeDef *gpio_port,
                   uint8_t gpio_pin,
                   const struct ioconfig *gpio_config);

/**
 * @brief Initialize an IO bus
 *
 * The bus is assosiated with one GPIO port and a set of continuous pins:
 * GPIOx[FIRST:LAST].
 *
 * @note The direction is the same for all the pins and yet it cannot change.
 *
 * @param[out]  io_handle_bus    The handler of the IO bus
 * @param[in]   gpio_port       The GPIO Port of the IO bus
 * @param       gpio_pin_first  The GPIO Pin number of the first IO pin
 * @param       gpio_pin_last   The GPIO Pin number of the last IO pin
 * @param       gpio_direction  The direction of the IO bus
 *
 * @return none
 */
void io_handle_bus_init(struct io_handle_bus *io_handle,
                       GPIO_TypeDef *gpio_port,
                       uint8_t gpio_pin_first,
                       uint8_t gpio_pin_last,
                       enum gpio_direction direction);

#endif /* _BSP_IO_HANDLE_H */
