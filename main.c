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
#include "iohandle.h"
#include "utils.h"
#include "console.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define ARRAY_SIZE(X) (sizeof(X) / sizeof(X[0]))

/* Private variables ---------------------------------------------------------*/
const static uint8_t button_response_time = 40U; /* Should be in (10, 50] msec */
const static uint8_t button_filter_times = 8U;
static uint16_t blink_periods[] = { 2000U, 1000U, 500U, 100U };
static char message[] = "System initilized at ";
static char num[11];

static struct app_data {
    struct iohandle user_led_2;
    struct iohandle user_button_1;
} app_data;

/* Private function prototypes -----------------------------------------------*/
static void app_init(struct app_data *app_data);
static unsigned count_digits(int num);
static char *itoa(char dst[], int num);

/* Private user code ---------------------------------------------------------*/

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    uint8_t i;
    uint16_t blink_period;
    uint32_t now;
    uint32_t start_blink;
    uint32_t start_debouncing;
    uint8_t button_down;
    uint8_t last_button;
    uint8_t button_flag;
    uint8_t debounce_period;

    app_init(&app_data);
    now = millis();

    /* Print welcome message */
    console_write(message, strlen(message));
    console_write(itoa(num, now), strlen(num));
    console_write(" ms\n\r", 5);

    /* Initialize flags and states */
    debounce_period = button_response_time / button_filter_times;

    button_down = 0U;
    last_button = 0U;
    button_flag = 0U;

    i = 0U;
    blink_period = blink_periods[i];

    start_debouncing = millis();
    start_blink = millis();

    /* Infinite loop */
    while (1) {
        /* If enough time has passed, blink */
        now = millis();
        if (now - start_debouncing >= debounce_period) {
            start_debouncing = now;

            /* Get filtered button output and update states and flags */
            button_down = button_filter(&app_data.user_button_1);
            button_flag = button_down && !last_button;
            last_button = button_down;

            /* If button is pressed, get next delay */
            if (button_flag) {
                button_flag = 0U;
                blink_period = blink_periods[++i % ARRAY_SIZE(blink_periods)];
                console_write("Button pressed\n\r", 16);
            }
        }

        /* If enough time has passed, blink */
        now = millis();
        if (now - start_blink >= blink_period) {
            start_blink = now;

            led_blink(&app_data.user_led_2);
            console_write("LED blink\n\r", 11);
        }
    }
}

static void app_init(struct app_data *app_data)
{
    /* Initialize system */
    board_init();

    /* Initialize on board LED */
    iohandle_init(&app_data->user_led_2, LD2_GPIO_Port, LD2_Pin, GPIO_OUTPUT,
                  0U, 0U);
    led_init(&app_data->user_led_2);

    /* Initialize on board button */
    iohandle_init(&app_data->user_button_1, B1_GPIO_Port, B1_Pin, GPIO_INPUT,
                  1U, button_filter_times);
    button_init(&app_data->user_button_1);
}

static unsigned count_digits(int num)
{
    unsigned num_digits = 0;

    do {
        num_digits++;
    } while (num /= 10);

    return num_digits;
}

static char *itoa(char dst[], int num)
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
