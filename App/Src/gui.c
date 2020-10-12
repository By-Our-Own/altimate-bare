#include "gui.h"
#include "lcd.h"
#include "delay.h"
#include "utils.h"
#include <string.h>

#define PRESS_OFF 0U
#define PRESS_INTEGER_LEN 3U
#define PRESS_FRACTION_LEN 5U
#define PRESS_MAX_LEN (PRESS_INTEGER_LEN + 1U + PRESS_FRACTION_LEN + 4U)
#define TEMP_OFF 0U
#define TEMP_INTEGER_LEN 2U
#define TEMP_FRACTION_LEN 2U
#define TEMP_MAX_LEN (1U + TEMP_INTEGER_LEN + 1U + TEMP_FRACTION_LEN + 2U)
#define ALTD_OFF 8U
#define ALTD_INTEGER_LEN 4U
#define ALTD_FRACTION_LEN 1U
#define ALTD_MAX_LEN (1U + ALTD_INTEGER_LEN + 1U + ALTD_FRACTION_LEN + 2U)

static char num_buf[PRESS_MAX_LEN + 1];

void gui_welcome(void)
{
	lcd_display(0, 0, "Fucking");
	lcd_display(0, 1, "Booted!");
	delay_ms(1000);
	lcd_clear();
}

void gui_update(uint64_t pressure, int64_t temperature, int64_t altitude)
{
	/* Blanken the value fields */
	lcd_clear();

	/* Show pressure: xxx.xxx kPa */
	memset(num_buf, 0, PRESS_MAX_LEN + 1U);
	fixed_point_to_str(num_buf, pressure, 0, PRESS_FRACTION_LEN, 3);
	lcd_display(PRESS_OFF, 0, num_buf);
	lcd_display(PRESS_OFF + strlen(num_buf), 0, " kPa");

	/* Show temperature: [+- ]xx.x C */
	memset(num_buf, 0, TEMP_MAX_LEN + 1U);
	fixed_point_to_str(num_buf, temperature, 1, TEMP_FRACTION_LEN, 1);
	lcd_display(TEMP_OFF, 1, num_buf);
	lcd_display(TEMP_OFF + strlen(num_buf), 1, " C");

	/* Show altitude: [+- ]xxxx.x m */
	memset(num_buf, 0, ALTD_MAX_LEN + 1U);
	fixed_point_to_str(num_buf, altitude, 0, ALTD_FRACTION_LEN, 1);
	lcd_display(ALTD_OFF, 1, num_buf);
	lcd_display(ALTD_OFF + strlen(num_buf), 1, " m");
}
