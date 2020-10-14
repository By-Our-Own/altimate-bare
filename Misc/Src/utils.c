#include "utils.h"
#include <stdint.h>
#include <string.h>

/* Internal buffer to hold the biggest 64bit number */
static char buf[21];

/**
 * @brief Count the digits in an integer
 *
 * @param num The number to convert
 *
 * @return The number of digits in the number
 */
static unsigned count_digits(uint64_t num)
{
	unsigned num_digits = 0;

	do {
		num_digits++;
	} while (num /= 10);

	return num_digits;
}

uint8_t itoa(char *dst, uint64_t num, uint8_t treat_signed)
{
	int i;
	unsigned num_digits = 0;

	if (treat_signed) {
		if (((int64_t)num) < 0) {
			dst[0] = '-';
			num = -((int64_t)num);
			num_digits = 1;	/* for the sign */
		}
	}
	num_digits += count_digits(num);

	i = num_digits;
	do {
		dst[--i] = '0' + num % 10;
	} while(num /= 10);

	dst[num_digits] = '\0';

	return num_digits;
}

uint8_t fptoa(char *dst,
	      uint64_t num,
	      uint8_t treat_signed,
	      uint8_t fraction_digits,
	      uint8_t fraction_to_show)
{
	uint8_t num_pos = 0U;
	uint8_t sign = 0U;
	uint8_t point_pos;
	uint8_t num_len = itoa(buf, num, treat_signed);
	uint8_t zeros_after_point;

	/* Handle number if equal to zero */
	if (num == 0U) {
		strncpy(dst, buf, num_len + 1);
		return num_len;
	}

	/* Take care of the sign */
	if (buf[0] == '-') {
		dst[num_pos] = '-';
		num_pos++;
		num_len--;
		sign = 1U;
	}

	/* Handle number if greater than zero */
	if (num_len > fraction_digits) {
		/* Find the position for the point */
		point_pos = num_pos + (num_len - fraction_digits);

		/* Copy the integer part */
		strncpy(&dst[num_pos], &buf[num_pos], point_pos);

		/* Add the point if applicable */
		if (fraction_to_show == 0U) {
			dst[point_pos] = '\0';
			return point_pos;
		}
		dst[point_pos] = '.';
		num_pos = point_pos + 1;

		/* Copy the fraction part */
		if (fraction_to_show > fraction_digits) {
			/* If the fraction to show is bigger than the actual fraction
			   the actual fraction digits are to be shown
			 */
			fraction_to_show = fraction_digits;
		}
		strncpy(&dst[num_pos], &buf[point_pos], fraction_to_show);
		num_pos += fraction_to_show;

		dst[num_pos] = '\0';
		return num_pos;
	}

	/* Handle number if smaller than zero */

	/* Add 0. in front */
	dst[num_pos] = '0';
	num_pos++;
	dst[num_pos] = '.';
	num_pos++;

	/* Copy the fraction part */
	if (fraction_to_show > fraction_digits) {
		/* If the fraction to show is bigger than the actual fraction
		   the actual fraction digits are to be shown
		 */
		fraction_to_show = fraction_digits;
	}

	/* Add zeros if need be */
	zeros_after_point = fraction_digits - num_len;
	if (zeros_after_point > fraction_to_show) {
		zeros_after_point = fraction_to_show;
	}
	fraction_to_show -= zeros_after_point;
	memset(&dst[num_pos], '0', zeros_after_point);
	num_pos += zeros_after_point;

	/* Copy the rest of the fraction part */
	strncpy(&dst[num_pos], &buf[sign], fraction_to_show);
	num_pos += fraction_to_show;

	dst[num_pos] = '\0';
	return num_pos;
}
