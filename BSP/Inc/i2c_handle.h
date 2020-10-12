#ifndef _BSP_I2C_HANDLE_H
#define _BSP_I2C_HANDLE_H

#include "main.h"
#include <stdint.h>
#include "io_handle.h"

/*
 * Flags for the message
 */
#define I2C_MSG_RW_MASK (0x01U)
#define I2C_MSG_READ (0x00U)
#define I2C_MSG_WRITE (0x01U)
#define I2C_MSG_STOP (0x02U)
#define I2C_MSG_10BITS (0x04U)

struct i2c_msg {
	uint8_t *buf;
	uint32_t len;
	uint8_t flags;
};

/* Forward declaration for i2c_handle */
struct i2c_handle;

/**
 * @brief Enable the I2C peripheral
 *
 * @param[out]  i2c_handle      The handler of the I2C peripheral
 *
 * @return none
 */
typedef void (*i2c_enable_fn)(const struct i2c_handle *);

/**
 * @brief Transfer a message to the I2C peripheral
 *
 * @note This implementation only transfers master to slave directed messages.
 *
 * @param[out]  i2c_handle  The handler of the I2C peripheral
 * @param       i2c_address The address of the slave
 * @param[in]   i2c_msg     The message to sent
 *
 * @retval 0 on success
 * @retval -1 otherwise
 */
typedef int (*i2c_transfer_fn)(const struct i2c_handle *, uint16_t,
			       struct i2c_msg *);

struct i2c_handle {
	I2C_TypeDef *instance;

	i2c_enable_fn enable;
	i2c_transfer_fn transfer;
};

/**
 * @brief Initialize an I2C peripheral
 *
 * @param[out]  i2c_handle      The handler of the I2C peripheral
 * @param[in]   i2c_instance    The I2C peripheral to use
 * @param[in]   scl             The I2C SCL pin handler
 * @param[in]   sda             The I2C SDA pin handler
 *
 * @return none
 */
void i2c_handle_init(struct i2c_handle *i2c_handle,
		     I2C_TypeDef *i2c_instance,
		     const struct io_handle *scl,
		     const struct io_handle *sda);

#endif /* _BSP_I2C_HANDLE_H*/
