/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "defs.h"
#include "board.h"
#include "led.h"
#include "button.h"
#include "iointerrupt.h"
#include "utils.h"
#include "console.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define ARRAY_SIZE(X) (sizeof(X) / sizeof(X[0]))

/* Private variables ---------------------------------------------------------*/
static volatile uint8_t button_down = 0U;
static uint16_t delays[] = { 2000U, 1000U, 500U, 100U };
static char message[] = "System initilized at ";
static char num[11];

/* Private function prototypes -----------------------------------------------*/
static void h_button_event(void);
static unsigned count_digits(int num);
static char *itoa(char dst[], int num);

/* Private user code ---------------------------------------------------------*/

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    uint8_t curr_delay = 0U;
    uint32_t initial;

    /* Initialize system */
    board_init();
    led_init();
    button_init();
    ioint_configure(B1_PORT, B1_PIN, IOINT_TRIG_FALLING, 0, h_button_event);
    ioint_enable(B1_PORT, B1_PIN);

    /* Print welcome message */
    console_write(message, strlen(message));
    console_write(itoa(num, millis()), strlen(num));
    console_write(" ms\n\r", 5);

    initial = millis();

    /* Infinite loop */
    while (1) {
        if (button_down) {
            /* Get next delay */
            curr_delay = (curr_delay + 1) % ARRAY_SIZE(delays);
            button_down = 0U;
        }

        /* If enough time has passed, blink */
        if (millis() - initial >= delays[curr_delay]) {
            led_blink();
            initial = millis();
        }
    }
}

void h_button_event(void)
{
    button_down = 1U;
}

unsigned count_digits(int num)
{
    unsigned num_digits = 0;

    do {
        num_digits++;
    } while (num /= 10);

    return num_digits;
}

char *itoa(char dst[], int num)
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

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}

#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
