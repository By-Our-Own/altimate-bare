#include "lcd.h"
#include "delay.h"

#define LCD_WIDTH 16
#define LCD_LINES 2

#define FSET 0x40
#define FSET_8BIT 0x10
#define FSET_2_LINES 0x08
#define FSET_BIG_FONT 0x04

#define CTRL 0x08
#define CTRL_ON 0x04
#define CTRL_CUR 0x02
#define CTRL_BLINK 0x01

#define EMS 0x04
#define EMS_INC 0x02
#define EMS_SHIFT 0x01

#define DISP_INIT 0x03
#define DISP_4BITS 0x02
#define DISP_CLEAR 0x01
#define DISP_2LINES (FSET | FSET_2_LINES)
#define DISP_ON (CTRL | CTRL_ON)
#define DISP_OFF (CTRL)

#define CURS_RH 0x02

#define BL 0x80
#define EN 0x40
#define RS 0x20
#define RW 0x10
#define DB7 0x08
#define DB6 0x04
#define DB5 0x02
#define DB4 0x01

#define DATA_MASK (DB7 | DB6 | DB5 | DB4)

static const struct io_handle_bus *lcd_conn;

static void _lcd_strobe(void)
{
	lcd_conn->set(lcd_conn, EN);

	/* Super tiny delay >= 230ns */
	volatile int8_t i = 1;
	while (i--) {
	}

	lcd_conn->reset(lcd_conn, EN);
}

static inline void _lcd_data_write_4bits(uint8_t data)
{
	uint16_t val = lcd_conn->read(lcd_conn) & (~DATA_MASK);
	lcd_conn->write(lcd_conn, val | (data & DATA_MASK));

	_lcd_strobe();
}

static inline void _lcd_send_cmd(uint8_t data)
{
	_lcd_data_write_4bits(data >> 4);	/* Send hi 4bits */
	_lcd_data_write_4bits(data);	/* Send lo 4bits */
}

/**
 * @param pos Place of the cursor in the screen
 *
 * RS R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
 * 0   0   1  ADD ADD ADD ADD ADD ADD ADD
 */
static void _lcd_cmd_place_cursor(uint8_t pos)
{
	lcd_conn->reset(lcd_conn, RS | RW);

	_lcd_send_cmd(0x80 | (pos & 0x7f));
}

void lcd_init(const struct io_handle_bus *conn)
{
	lcd_conn = conn;

	/* Configure every pin */
	lcd_conn->config(lcd_conn);

	delay_ms(16);	/* Wait for more than 15 ms */

	lcd_conn->reset(lcd_conn, RS | RW);

	_lcd_data_write_4bits(DISP_INIT);

	delay_ms(5);	/* Wait for more than 4.1 ms */

	_lcd_data_write_4bits(DISP_INIT);

	delay_ms(1);	/* Wait for more than 100 us */

	_lcd_data_write_4bits(DISP_INIT);

	delay_ms(1);

	_lcd_data_write_4bits(DISP_4BITS);

	delay_ms(1);

	_lcd_send_cmd(DISP_2LINES);

	/* Turn OFF */
	_lcd_send_cmd(DISP_OFF);


	lcd_clear();		/* Clear display */

	_lcd_send_cmd(EMS | EMS_INC);

	/* Turn ON*/
	_lcd_send_cmd(DISP_ON);
}

/**
 * RS R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
 * 1   0   B7  B6  B5  B4  B3  B2  B1  B0
 */
void lcd_putc(uint8_t c)
{
	lcd_conn->set(lcd_conn, RS);
	lcd_conn->reset(lcd_conn, RW);

	_lcd_data_write_4bits(c >> 4);
	_lcd_data_write_4bits(c);
}

void lcd_clear(void)
{
	lcd_conn->reset(lcd_conn, RS | RW);
	_lcd_send_cmd(DISP_CLEAR);
	delay_ms(2);
}

void lcd_display(int x, int y, const char *msg)
{
	int n = LCD_WIDTH - x;
	uint8_t addr;

	if ((x < 0) || (x >= LCD_WIDTH)) {
		return;
	}

	if ((y < 0) || (y >= LCD_LINES)) {
		return;
	}

	addr = (y == 0) ? 0x00 : 0x40;

	_lcd_cmd_place_cursor(addr + x);

	while (*msg && n--) {
		lcd_putc(*msg++);
	}
}

void lcd_backlight(uint8_t on)
{
	if (on) {
		lcd_conn->set(lcd_conn, BL);
	} else {
		lcd_conn->reset(lcd_conn, BL);
	}
}
