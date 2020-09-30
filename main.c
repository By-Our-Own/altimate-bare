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
#include "console.h"
#include "timer.h"
#include "lcd.h"
#include "deadline.h"
#include "delay.h"
#include "utils.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define ARRAY_SIZE(X) (sizeof(X) / sizeof(X[0]))

/* Private variables ---------------------------------------------------------*/

static char message[] = "System initilized at ";
static char num_buf[11];

static struct app_data {
    struct iohandle user_led_2;
    struct iohandle user_button_1;
    struct iohandle lcd_conn[8];
    struct iohandle_bus lcd_bus;

    volatile uint8_t led_idx;

    uint16_t baro_dl;
    uint16_t button_dl;
    uint16_t lcd_dl;
    uint16_t lcd_flag;
} app_data;

/* Private function prototypes -----------------------------------------------*/
static void app_init(struct app_data *app_data);

static uint16_t get_pressure(void)
{
    static uint16_t i = 0;
    uint16_t press[5] = { 4, 67, 130, 50, 3};
    return press[(i++) % 5];
}

static int16_t get_temperature(void)
{
    static uint8_t i = 0;
    int8_t temp[5] = { 24, 25, 23, 22, 21};
    return temp[(i++) % 5];
}

static int16_t get_height(int16_t reset)
{
    static uint16_t i = 0;
    if (reset) {
        i = 0;
    }
    return i++;
}

/* Private user code ---------------------------------------------------------*/

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    uint8_t button_down;
    uint8_t last_button;
    uint8_t button_flag;
    uint8_t debounce_period;
    uint16_t pressure;
    int16_t temperature;
    int16_t height;

    app_init(&app_data);

    /* Print welcome message */
    console_write(message, strlen(message));
    misc_itoa(num_buf, millis());
    console_write(num_buf, strlen(num_buf));
    console_write(" ms\n\r", 5);

    /* Initialize flags and states */
    debounce_period = button_debounce_time_ms(&app_data.user_button_1);

    button_down = 0U;
    last_button = 0U;
    button_flag = 0U;

    pressure = get_pressure();
    temperature = get_temperature();
    height = get_height(1);

    /* Infinite loop */
    while (1) {
        /* Handle sensor readings */
        if (deadline_reached(app_data.baro_dl)) {
            pressure = get_pressure();
            temperature = get_temperature();
            height = get_height(0);
            app_data.baro_dl = deadline_extend(app_data.baro_dl, 100);
        }

        /* Handle button presses */
        if (deadline_reached(app_data.button_dl)) {
            /* Get filtered button output and update states and flags */
            button_down = button_filter(&app_data.user_button_1);
            button_flag = button_down && !last_button;
            last_button = button_down;

            /* If button is pressed, get next delay */
            if (button_flag) {
                height = get_height(1);
                app_data.lcd_flag = 1;

                button_flag = 0U;
            }

            app_data.button_dl = deadline_extend(app_data.button_dl, debounce_period);
        }

        /* Handle LCD refresh */
        if (app_data.lcd_flag || deadline_reached(app_data.lcd_dl)) {
            /* Blanken the value fields */
            lcd_display(5, 0, "         ");
            lcd_display(5, 1, "     ");

            /* Fill the value fields */
            lcd_display(5, 0, misc_itoa(num_buf, pressure));
            lcd_display(10, 0, misc_itoa(num_buf, height));
            lcd_display(5, 1, misc_itoa(num_buf, temperature));

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
    iohandle_bus_init(&app_data->lcd_bus, GPIOB, LL_GPIO_PIN_8, LL_GPIO_PIN_15, GPIO_OUTPUT);
    lcd_init(&app_data->lcd_bus);

    /* Initialize UI */
    lcd_backlight(1);
    lcd_display(0, 0, "kPa:          :m");
    lcd_display(0, 1, "oC:");


    /* Initialize on board LED */
    iohandle_init(&app_data->user_led_2, LD2_GPIO_Port, LD2_Pin, GPIO_OUTPUT);
    // iohandle_init(&app_data->user_led_2, GPIOA, LL_GPIO_PIN_15, GPIO_OUTPUT);
    led_init(&app_data->user_led_2);
    led_off(&app_data->user_led_2);

    /* Initialize on board button */
    iohandle_init(&app_data->user_button_1, B1_GPIO_Port, B1_Pin, GPIO_INPUT);
    button_init(&app_data->user_button_1, 5U);

    /* Initialize deadlines */
    app_data->baro_dl = deadline_make(0);
    app_data->button_dl = deadline_make(0);
    app_data->lcd_dl = deadline_make(0);
    app_data->lcd_flag = 0;
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
