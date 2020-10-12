#include "i2c_handle.h"

/* Standard mode @100kHz with I2CCLK = 2MHz,
 * rise time = 100ns, fall time = 10ns
 */
#define I2C_TIMING ((uint32_t)0x00000708)

static void __enable(const struct i2c_handle *this);
static int __transfer(const struct i2c_handle *this,
		      uint16_t slave_addr,
		      struct i2c_msg *msg);

static int check_errors(const struct i2c_handle *this);

void i2c_handle_init(struct i2c_handle *i2c_handle,
		     I2C_TypeDef *i2c_instance,
		     const struct io_handle *scl,
		     const struct io_handle *sda)
{
	/* Configure SCL and SDA GPIO pins */
	scl->config(scl);
	sda->config(sda);

	/* Populate I2C handle */
	i2c_handle->instance = i2c_instance;

	i2c_handle->enable = __enable;
	i2c_handle->transfer = __transfer;
}

static int check_errors(const struct i2c_handle *dev)
{
	I2C_TypeDef *i2c_dev = dev->instance;

	if (LL_I2C_IsActiveFlag_BERR(i2c_dev)) {
		return -1;
	}

	if (LL_I2C_IsActiveFlag_ARLO(i2c_dev)) {
		return -1;
	}

	if (LL_I2C_IsActiveFlag_OVR(i2c_dev)) {
		return -1;
	}

	return 0;
}

static void __enable(const struct i2c_handle *this)
{
	I2C_TypeDef *i2c_dev = this->instance;

	/* I2C Peripheral Clock Enable */
	if (i2c_dev == I2C1) {
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
		LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_SYSCLK);
	} else
	if (i2c_dev == I2C2) {
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
	} else
	if (i2c_dev == I2C3) {
		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);
		LL_RCC_SetI2CClockSource(LL_RCC_I2C3_CLKSOURCE_SYSCLK);
	}

	/* Disable I2C before any configuration */
	LL_I2C_Disable(i2c_dev);

	/* Configure I2C timings */
	LL_I2C_SetTiming(i2c_dev, I2C_TIMING);

	/* Configure peripheral as I2C */
	LL_I2C_SetMode(i2c_dev, LL_I2C_MODE_I2C);

	/* Enable I2C after the configurations */
	LL_I2C_Enable(i2c_dev);
}

static inline int i2c_write(const struct i2c_handle *this,
			    uint16_t slave_addr,
			    struct i2c_msg *msg)
{
	I2C_TypeDef *i2c_dev = this->instance;
	unsigned len = msg->len;
	uint8_t *buf = msg->buf;

	/* Write to the peripheral */
	while (len) {
		if (LL_I2C_IsActiveFlag_TXIS(i2c_dev)) {
			LL_I2C_TransmitData8(i2c_dev, *buf);
			buf++;
			len--;
		}

		if (check_errors(this)) {
			return -1;
		}
	}

	return 0;
}

static inline int i2c_read(const struct i2c_handle *this,
			   uint16_t slave_addr,
			   struct i2c_msg *msg)
{
	I2C_TypeDef *i2c_dev = this->instance;
	unsigned len = msg->len;
	uint8_t *buf = msg->buf;

	/* Read from the peripheral */
	while (len) {
		if (LL_I2C_IsActiveFlag_RXNE(i2c_dev)) {
			*buf = LL_I2C_ReceiveData8(i2c_dev);
			buf++;
			len--;
		}

		if (check_errors(this)) {
			return -1;
		}
	}

	return 0;
}

static int __transfer(const struct i2c_handle *this,
		      uint16_t slave_addr,
		      struct i2c_msg *msg)
{
	i2c_transfer_fn i2c_req_fun;

	/* Message intro */
	if (msg->flags & I2C_MSG_10BITS) {
		LL_I2C_SetMasterAddressingMode(this->instance,
					       LL_I2C_ADDRSLAVE_10BIT);
		LL_I2C_SetSlaveAddr(this->instance, (uint32_t) slave_addr);
	} else {
		LL_I2C_SetMasterAddressingMode(this->instance,
					       LL_I2C_ADDRSLAVE_7BIT);
		LL_I2C_SetSlaveAddr(this->instance,
				    (uint32_t) slave_addr << 1U);
	}

	if ((msg->flags & I2C_MSG_RW_MASK) == I2C_MSG_READ) {
		LL_I2C_SetTransferRequest(this->instance, LL_I2C_REQUEST_READ);
		i2c_req_fun = i2c_read;
	} else {
		LL_I2C_SetTransferRequest(this->instance, LL_I2C_REQUEST_WRITE);
		i2c_req_fun = i2c_write;
	}

	LL_I2C_SetTransferSize(this->instance, msg->len);
	LL_I2C_GenerateStartCondition(this->instance);

	/* Transfer message */
	if (i2c_req_fun(this, slave_addr, msg)) {
		return -1;
	}

	/* Message outro */
	while (!LL_I2C_IsActiveFlag_TC(this->instance) &&
	       !LL_I2C_IsActiveFlag_TCR(this->instance)) {
		if (check_errors(this)) {
			return -1;;
		}
	}

	/* Generate STOP condition if last message */
	if (msg->flags & I2C_MSG_STOP) {
		LL_I2C_GenerateStopCondition(this->instance);
		while (!LL_I2C_IsActiveFlag_STOP(this->instance)) {
		}

		LL_I2C_ClearFlag_STOP(this->instance);
	}

	return 0;
}
