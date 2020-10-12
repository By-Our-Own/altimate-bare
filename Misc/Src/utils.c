#include "utils.h"
#include <stdint.h>
#include <string.h>

static int pow10[8] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000 };

/**
 * @brief Count the digits in an integer
 *
 * @param num The number to convert
 *
 * @return The number of digits in the number
 */
static unsigned count_digits(int num)
{
	unsigned num_digits = 0;

	do {
		num_digits++;
	} while (num /= 10);

	return num_digits;
}

char *misc_itoa(char *dst, int num)
{
	int i;
	int sign;
	unsigned num_digits = 0;

	if ((sign = num) < 0) {
		num = -num;
		num_digits = 1;	/* for the sign */
	}
	num_digits += count_digits(num);

	i = num_digits;
	do {
		dst[--i] = '0' + num % 10;
	} while(num /= 10);

	if (sign < 0) {
		dst[0] = '-';
	}
	dst[num_digits] = '\0';

	return dst;
}

void fixed_point_to_str(char *dst,
			uint64_t num,
			uint8_t treat_signed,
			uint8_t fraction_digits,
			uint8_t fraction_to_show)
{
	uint8_t pos = 0U;
	uint8_t fraction_len;
	uint8_t pad;

	/* Append sign if has one */
	if (treat_signed) {
		if (((int64_t)num) < 0) {
			dst[pos] = '-';
			num = -((int64_t)num);
		} else
		if (((int64_t)num) > 0) {
			dst[pos] = '+';
		} else {
			dst[pos] = ' ';
		}
		pos++;
	}

	/* Append integer part */
	misc_itoa(&dst[pos], num / pow10[fraction_digits]);
	pos += strlen(&dst[pos]);

	/* Append point */
	dst[pos] = '.';
	pos++;

	/* Append fraction */
	uint64_t fraction = num % pow10[fraction_digits];
	fraction_len = count_digits(fraction);
	pad = fraction_digits - fraction_len;
	fraction /= pow10[fraction_len - fraction_to_show];
	memset(&dst[pos], '0', fraction_digits);
	misc_itoa(&dst[pos + pad], fraction);
	pos += fraction_to_show;

	/* Conclude */
	dst[pos] = '\0';
}
