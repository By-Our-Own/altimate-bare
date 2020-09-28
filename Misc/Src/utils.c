#include "utils.h"

static unsigned count_digits(int num)
{
    unsigned num_digits = 0;

    do {
        num_digits++;
    } while (num /= 10);

    return num_digits;
}

char *misc_itoa(char dst[], int num)
{
    int i;
    int sign;
    unsigned num_digits = 0;

    if ((sign = num) < 0) {
        num = -num;
        num_digits = 1; /* for the sign */
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
