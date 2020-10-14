#include "bmp388.h"
#include "bmp3.h"
#include "bmp3_defs.h"
#include "delay.h"

static BMP3_INTF_RET_TYPE baro_read(uint8_t reg_addr, uint8_t *reg_data,
				    uint32_t len, void *intf_ptr);
static BMP3_INTF_RET_TYPE baro_write(uint8_t reg_addr, const uint8_t *reg_data,
				     uint32_t len, void *intf_ptr);
static void baro_delay(uint32_t period, void *intf_ptr);

int bmp388_init(struct bmp388_handle *dev,
		struct i2c_handle *i2c,
		uint16_t i2c_address)
{
	int8_t result;
	uint16_t settings_sel;

	i2c->enable(i2c);

	dev->bus = i2c;
	dev->address = i2c_address;

	dev->instance.intf_ptr = dev;
	dev->instance.intf = BMP3_I2C_INTF;
	dev->instance.read = baro_read;
	dev->instance.write = baro_write;
	dev->instance.delay_us = baro_delay;

	result = bmp3_init(&dev->instance);
	if (result != BMP3_OK) {
		return -1;
	}

	/* Configure BMP388 in forced mode with oversampling */
	dev->instance.settings.press_en = BMP3_ENABLE;
	dev->instance.settings.temp_en = BMP3_ENABLE;
	dev->instance.settings.odr_filter.press_os = BMP3_OVERSAMPLING_2X;
	dev->instance.settings.odr_filter.temp_os = BMP3_OVERSAMPLING_2X;
	settings_sel = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN |
		       BMP3_SEL_PRESS_OS | BMP3_SEL_TEMP_OS;
	result = bmp3_set_sensor_settings(settings_sel, &dev->instance);
	if (result != BMP3_OK) {
		return -1;
	}
	dev->instance.settings.op_mode = BMP3_MODE_FORCED;
	result = bmp3_set_op_mode(&dev->instance);
	if (result != BMP3_OK) {
		return -1;
	}

	return 0;
}

int8_t bmp388_get_data(struct bmp388_handle *dev,
		       uint64_t *pressure,
		       int64_t *temperature)
{
	int8_t result;
	uint8_t sensor_comp;
	struct bmp3_data data;

	dev->instance.settings.op_mode = BMP3_MODE_FORCED;
	result = bmp3_set_op_mode(&dev->instance);
	if (result != BMP3_OK) {
		return -1;
	}

	/* Sensor component selection */
	sensor_comp = BMP3_PRESS | BMP3_TEMP;
	result = bmp3_get_sensor_data(sensor_comp, &data, &dev->instance);
	if (result != BMP3_OK) {
		return -1;
	}

	*pressure = data.pressure;
	*temperature = data.temperature;

	return 0;
}

static BMP3_INTF_RET_TYPE baro_read(uint8_t reg_addr, uint8_t *reg_data,
				    uint32_t len, void *intf_ptr)
{
	struct bmp388_handle *dev = (struct bmp388_handle *)intf_ptr;
	struct i2c_msg msg0 = { &reg_addr, sizeof(reg_addr), I2C_MSG_WRITE };
	struct i2c_msg msg1 = { reg_data, len, I2C_MSG_READ | I2C_MSG_STOP };

	if (dev->bus->transfer(dev->bus, dev->address, &msg0)) {
		return -1;
	}

	if (dev->bus->transfer(dev->bus, dev->address, &msg1)) {
		return -1;
	}

	return 0;;
}

static BMP3_INTF_RET_TYPE baro_write(uint8_t reg_addr, const uint8_t *reg_data,
				     uint32_t len, void *intf_ptr)
{
	struct bmp388_handle *dev = (struct bmp388_handle *)intf_ptr;
	uint8_t buf[2] = { reg_addr, *reg_data };
	struct i2c_msg msg = { buf, 2, I2C_MSG_WRITE | I2C_MSG_STOP };

	if (dev->bus->transfer(dev->bus, dev->address, &msg)) {
		return -1;
	}

	return 0;
}

static void baro_delay(uint32_t period, void *intf_ptr)
{
	(void)intf_ptr;

	delay_ms(period / 1000U);
}
