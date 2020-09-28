#ifndef _MISC_UTILS_H
#define _MISC_UTILS_H

/**
 * @brief Convert a integer to a string
 * @note The user should provide a big enough string buffer
 *
 * @param dst Buffer to hold the converted number
 * @param num The number to convert
 *
 * @return A pointer to the converted string
 */
char *misc_itoa(char dst[], int num);

#endif /* _MISC_UTILS_H */
