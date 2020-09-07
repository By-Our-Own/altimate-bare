#ifndef _BSP_CONSOLE_H
#define _BSP_CONSOLE_H

#include <stdint.h>

/**
 * @brief Write an amount of data in blocking mode
 *
 * @param [in]buf   Data to send
 * @param size      Size of the data
 *
 * @return The amount of data sent, otherwise -1 on failure
 */
uint16_t console_write(const void *buf, uint16_t size);

#endif /* _BSP_CONSOLE_H */
