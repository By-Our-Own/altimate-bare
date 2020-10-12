#ifndef _DEVICES_BMP388_H
#define _DEVICES_BMP388_H

#include "bmp3_defs.h"
#include "i2c_handle.h"
#include <stdint.h>

struct bmp388_handle {
	struct bmp3_dev instance;
	struct i2c_handle *bus;
	uint16_t address;
};

int bmp388_init(struct bmp388_handle *dev,
		struct i2c_handle *i2c,
		uint16_t i2c_address);
int8_t bmp388_get_data(struct bmp388_handle *dev,
		       uint64_t *pressure,
		       int64_t *temperature);

#endif /* _DEVICES_BMP388_H */
