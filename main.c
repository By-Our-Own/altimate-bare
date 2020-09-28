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
#include "timer.h"
#include "lcd.h"
#include "deadline.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define ARRAY_SIZE(X) (sizeof(X) / sizeof(X[0]))

/* Private variables ---------------------------------------------------------*/
const static uint8_t button_response_time = 40U; /* Should be in (10, 50] msec */
const static uint8_t button_filter_times = 8U;

/* For the LED frequencies */
static struct tim_settings {
    uint32_t frequency;
    uint32_t autorelod;
} tim_led_settings[3] = {
    {13000U, 2000U},    /* 6.5Hz */
    {17000U, 2000U},    /* 8.5Hz */
    {20000U, 2000U},    /* 10Hz */
};

static char message[] = "System initilized at ";
static char num[11];

static struct app_data {
    struct iohandle user_led_2;
    struct iohandle user_button_1;
    struct iohandle lcd_conn[8];

    volatile uint8_t led_idx;

    uint16_t c100ms_dl;
    uint16_t c1000ms_dl;
    uint16_t button_dl;
    uint16_t lcd_dl;
    uint16_t lcd_flag;
} app_data;

/* Private function prototypes -----------------------------------------------*/
static void app_init(struct app_data *app_data);
static unsigned count_digits(int num);
static char *itoa(char dst[], int num);
static void h_timer_expired(void);

/* Private user code ---------------------------------------------------------*/

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    uint32_t now;
    uint8_t button_down;
    uint8_t last_button;
    uint8_t button_flag;
    uint8_t debounce_period;
    uint16_t c1000ms;
    uint16_t c100ms;

    app_init(&app_data);
    now = millis();

    /* Print welcome message */
    console_write(message, strlen(message));
    console_write(itoa(num, now), strlen(num));
    console_write(" ms\n\r", 5);

    c1000ms = 0U;
    c100ms = 0U;

    /* Initialize flags and states */
    debounce_period = button_response_time / button_filter_times;

    button_down = 0U;
    last_button = 0U;
    button_flag = 0U;

    /* Infinite loop */
    while (1) {
        /* Handle 1000ms counter */
        if (deadline_reached(app_data.c1000ms_dl)) {
            c1000ms++;
            app_data.c1000ms_dl = deadline_extend(app_data.c1000ms_dl, 1000);
            if (!(c1000ms % 1000)) {
                app_data.lcd_flag = 1;  /* Display smooth 1 second changes */
            }
        }

        /* Handle 100ms counter */
        if (deadline_reached(app_data.c100ms_dl)) {
            c100ms++;
            app_data.c100ms_dl = deadline_extend(app_data.c100ms_dl, 100);
        }

        /* Handle button presses */
        if (deadline_reached(app_data.button_dl)) {
            /* Get filtered button output and update states and flags */
            button_down = button_filter(&app_data.user_button_1);
            button_flag = button_down && !last_button;
            last_button = button_down;

            /* If button is pressed, get next delay */
            if (button_flag) {
                /* Get next led blinking frequency*/
                app_data.led_idx = (app_data.led_idx + 1) % ARRAY_SIZE(tim_led_settings);
                timer_configure(tim_led_settings[app_data.led_idx].frequency,
                                tim_led_settings[app_data.led_idx].autorelod,
                                h_timer_expired);

                button_flag = 0U;
                // console_write("Button pressed\n\r", 16);
            }

            app_data.button_dl = deadline_extend(app_data.button_dl, debounce_period);
        }

        /* Handle LCD refresh */
        if (app_data.lcd_flag || deadline_reached(app_data.lcd_dl)) {
            lcd_display(9, 0, itoa(num, c1000ms));
            lcd_display(9, 1, itoa(num, c100ms));

            if (app_data.lcd_flag) {
                app_data.lcd_flag = 0;
                app_data.lcd_dl = deadline_make(400);
            } else {
                app_data.lcd_dl = deadline_extend(app_data.lcd_dl, 400);
            }
        }
    }
}

static void app_init(struct app_data *app_data)
{
    /* Initialize system */
    board_init();

    /* Initialize LCD */
    iohandle_init(&app_data->lcd_conn[LCD_E], GPIOC, LL_GPIO_PIN_7, GPIO_OUTPUT, 0, 0);
    iohandle_init(&app_data->lcd_conn[LCD_RS], GPIOA, LL_GPIO_PIN_9, GPIO_OUTPUT, 0, 0);
    iohandle_init(&app_data->lcd_conn[LCD_RW], GPIOA, LL_GPIO_PIN_8, GPIO_OUTPUT, 0, 0);
    iohandle_init(&app_data->lcd_conn[LCD_DB4], GPIOB, LL_GPIO_PIN_10, GPIO_OUTPUT, 0, 0);
    iohandle_init(&app_data->lcd_conn[LCD_DB5], GPIOB, LL_GPIO_PIN_4, GPIO_OUTPUT, 0, 0);
    iohandle_init(&app_data->lcd_conn[LCD_DB6], GPIOB, LL_GPIO_PIN_5, GPIO_OUTPUT, 0, 0);
    iohandle_init(&app_data->lcd_conn[LCD_DB7], GPIOB, LL_GPIO_PIN_3, GPIO_OUTPUT, 0, 0);
    iohandle_init(&app_data->lcd_conn[LCD_BL], GPIOB, LL_GPIO_PIN_6, GPIO_OUTPUT, 0, 0);

    lcd_init(&app_data->lcd_conn);

    app_data->led_idx = 0U;

    /* Initialize on board LED */
    iohandle_init(&app_data->user_led_2, LD2_GPIO_Port, LD2_Pin, GPIO_OUTPUT,
                  0U, 0U);
    // iohandle_init(&app_data->user_led_2, GPIOA, LL_GPIO_PIN_15, GPIO_OUTPUT,
    //               0U, 0U);
    led_init(&app_data->user_led_2);
    led_off(&app_data->user_led_2);

    /* Initialize on board button */
    iohandle_init(&app_data->user_button_1, B1_GPIO_Port, B1_Pin, GPIO_INPUT,
                  1U, button_filter_times);
    button_init(&app_data->user_button_1);

    /* Initialize timer for generation of the LED blinking frequencies */
    timer_configure(tim_led_settings[app_data->led_idx].frequency,
                    tim_led_settings[app_data->led_idx].autorelod,
                    h_timer_expired);
    timer_start();

    /* Initialize UI */
    lcd_backlight(1);
    lcd_display(0, 0, "C1000ms:");
    lcd_display(0, 1, "C 100ms:");

    /* Initialize deadlines */
    app_data->c100ms_dl = deadline_make(0);
    app_data->c1000ms_dl = deadline_make(0);
    app_data->button_dl = deadline_make(0);
    app_data->lcd_dl = deadline_make(0);
    app_data->lcd_flag = 0;
}

static void h_timer_expired(void)
{
    led_blink(&app_data.user_led_2);
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
