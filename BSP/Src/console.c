#include "console.h"

#include "usart.h"

/* Implemented according to:
 * STM32L073RZTx datasheet, 29.5.2 "USART Trasmitter"
 */
uint16_t console_write(const void *buf, uint16_t size)
{
    uint16_t bytes_sent = 0U;

    if (!buf || size == 0U) {
        return -1;
    }

    while (bytes_sent < size) {
        /* Wait until TXE flag is set */
        while (!LL_USART_IsActiveFlag_TXE(USART2)) {
            ;
        }

        /* Clear TC flag */
        LL_USART_ClearFlag_TC(USART2);

        /* Transmit the byte */
        LL_USART_TransmitData8(USART2, ((uint8_t *)buf)[bytes_sent++]);
    }

    /* Wait until TC flag is set */
    while (!LL_USART_IsActiveFlag_TC(USART2)) {
        ;
    }

    return bytes_sent;
}
