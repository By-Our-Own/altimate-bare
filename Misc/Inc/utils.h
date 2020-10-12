#ifndef _MISC_UTILS_H
#define _MISC_UTILS_H

#include <stdint.h>

/**
 * @brief Convert a integer to a string
 * @note The user should provide a big enough string buffer
 *
 * @param[out]  dst Buffer to hold the converted number
 * @param       num The number to convert
 *
 * @return A pointer to the converted string
 */
char *misc_itoa(char *dst, int num);

/**
 * @brief Convert fixed point to string
 *
 * num        = +xxxxxx
 *              ^--------- treat_signed
 *
 * actual num = +xxxx.xx
 *                    ^^-- fract_len_embed
 *
 * dst        = +xxxx.x
 *                    ^--- fract_len
 *
 * @param[out]  dst             The buffer that contains the result
 * @param       num             The number to convert
 * @param       treat_signed    Treat the number as signed
 * @param       fract_len_embed Number of fraction digits embedded in the number
 * @param       fract_len       Lenght of the fraction digits in the result
 */
void fixed_point_to_str(char *dst,
			uint64_t num,
			uint8_t treat_signed,
			uint8_t fraction_digits,
			uint8_t fraction_to_show);

#endif /* _MISC_UTILS_H */
