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
#include "gui.h"
#include "lcd.h"
#include "defs.h"
#include "board.h"
#include "led.h"
#include "button.h"
#include "iointerrupt.h"
#include "io_handle.h"
#include "i2c_handle.h"
#include "console.h"
#include "timer.h"
#include "deadline.h"
#include "delay.h"
#include "utils.h"
#include "bmp388.h"
#include <string.h>
#include <math.h>

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define ARRAY_SIZE(X) (sizeof(X) / sizeof(X[0]))

/* Private variables ---------------------------------------------------------*/

static char message[] = "System initilized at ";
static char num_buf[11];

static struct app_data {
    struct io_handle user_led_2;
    struct io_handle user_button_1;
    struct io_handle_bus lcd_bus;

    struct io_handle i2c_scl;
    struct io_handle i2c_sda;
    struct i2c_handle bmp388_i2c;
    struct bmp388_handle bmp388_dev;

    volatile uint8_t led_idx;

    uint16_t baro_dl;
    uint16_t button_dl;
    uint16_t lcd_dl;
    uint16_t lcd_flag;
} app_data;

/* Private function prototypes -----------------------------------------------*/
static void app_init(struct app_data *app_data);

static int64_t get_altitude(uint64_t pressure_zero, uint64_t pressure, int64_t temperature)
{
    double altitude;

    altitude = pow(pressure_zero / (double)pressure, 0.19) - 1;
    altitude *= temperature + 273.15;
    altitude /= 0.0065;

    if (altitude > 9999) {
        return -9999;
    }

    return altitude * 10;
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
    uint64_t pressure;
    uint64_t pressure_zero;
    int64_t temperature;
    int64_t height;

    app_init(&app_data);

    /* Print welcome message */
    console_write(message, strlen(message));
    itoa(num_buf, millis(), 0);
    console_write(num_buf, strlen(num_buf));
    console_write(" ms\n\r", 5);

    /* Initialize flags and states */
    debounce_period = button_debounce_time_ms(&app_data.user_button_1);

    button_down = 0U;
    last_button = 0U;
    button_flag = 0U;

    /* Initial readings */
    if (bmp388_get_data(&app_data.bmp388_dev, &pressure, &temperature)) {
        pressure = 0U;
        temperature = 0U;
    }
    pressure_zero = pressure;
    height = get_altitude(pressure_zero, pressure, temperature);

    /* Infinite loop */
    while (1) {
        /* Handle sensor readings */
        if (deadline_reached(app_data.baro_dl)) {
            if (bmp388_get_data(&app_data.bmp388_dev, &pressure, &temperature)) {
                pressure = 0U;
                temperature = 0U;
            }
            height = get_altitude(pressure_zero, pressure, temperature);
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
                pressure_zero = pressure;
                height = get_altitude(pressure_zero, pressure, temperature);
                app_data.lcd_flag = 1;

                button_flag = 0U;
            }

            app_data.button_dl = deadline_extend(app_data.button_dl, debounce_period);
        }

        /* Handle LCD refresh */
        if (app_data.lcd_flag || deadline_reached(app_data.lcd_dl)) {
            gui_update(pressure, temperature, height);

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
    struct ioconfig cfg = { 0 };

    /* Initialize system */
    board_init();

    /* Initialize LCD */
    io_handle_bus_init(&app_data->lcd_bus, GPIOB, 8, 15, GPIO_OUTPUT);
    lcd_init(&app_data->lcd_bus);

    /* Initialize UI */
    lcd_backlight(1);
    gui_welcome();

    /* Initialize on board LED */
    cfg.dir = GPIO_OUTPUT;
    cfg.otype = GPIO_PUSHPULL;
    cfg.pull = GPIO_PULL_NO;
    cfg.speed = GPIO_SPEED_LOW;
    io_handle_init(&app_data->user_led_2, LD2_GPIO_Port, LD2_Pin, &cfg);
    led_init(&app_data->user_led_2);
    led_on(&app_data->user_led_2);

    /* Initialize on board button */
    cfg.dir = GPIO_INPUT;
    cfg.otype = GPIO_PUSHPULL;
    cfg.pull = GPIO_PULL_UP;
    cfg.speed = GPIO_SPEED_LOW;
    io_handle_init(&app_data->user_button_1, B1_GPIO_Port, B1_Pin, &cfg);
    button_init(&app_data->user_button_1, 5U);

    /* Initialize BMP388 */
    cfg.dir = GPIO_ALTERNATE;
    cfg.otype = GPIO_PUSHPULL;
    cfg.pull = GPIO_PULL_UP;
    cfg.speed = GPIO_SPEED_HIGH;
    cfg.func = LL_GPIO_AF_6;
    io_handle_init(&app_data->i2c_scl, GPIOA, 9, &cfg);
    io_handle_init(&app_data->i2c_sda, GPIOA, 10, &cfg);
    i2c_handle_init(&app_data->bmp388_i2c, I2C1, &app_data->i2c_scl, &app_data->i2c_sda);
    bmp388_init(&app_data->bmp388_dev, &app_data->bmp388_i2c, BMP388_SLAVE_ADDR);

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
