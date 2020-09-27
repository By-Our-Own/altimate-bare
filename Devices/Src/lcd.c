#include "lcd.h"
#include "utils.h"

#define LCD_WIDTH   16
#define LCD_LINES   2

#define FSET            0x40
#define FSET_8BIT       0x10
#define FSET_2_LINES    0x08
#define FSET_BIG_FONT   0x04

#define CTRL       0x08
#define CTRL_ON    0x04
#define CTRL_CUR   0x02
#define CTRL_BLINK 0x01

#define EMS         0x04
#define EMS_INC     0x02
#define EMS_SHIFT   0x01

#define DISP_INIT   0x03
#define DISP_4BITS  0x02
#define DISP_CLEAR  0x01
#define DISP_2LINES (FSET|FSET_2_LINES)
#define DISP_ON     (CTRL|CTRL_ON)
#define DISP_OFF    (CTRL)

#define CURS_RH      0x02

#define rs  ((*lcd_conn)[LCD_RS])
#define rw  ((*lcd_conn)[LCD_RW])
#define en  ((*lcd_conn)[LCD_E])
#define db4  ((*lcd_conn)[LCD_DB4])
#define db5  ((*lcd_conn)[LCD_DB5])
#define db6  ((*lcd_conn)[LCD_DB6])
#define db7  ((*lcd_conn)[LCD_DB7])
#define bl  ((*lcd_conn)[LCD_BL])

static const struct iohandle (*lcd_conn)[8];

static void _lcd_strobe(void)
{
    en.set(&en);

    volatile int8_t i = 10;
    while(i--) {
    }

    en.reset(&en);
}

static void _lcd_data_write_4bits(uint8_t data)
{
    rs.reset(&rs);
    rw.reset(&rw);
    (data & (1 << 3)) ? db7.set(&db7) : db7.reset(&db7);
    (data & (1 << 2)) ? db6.set(&db6) : db6.reset(&db6);
    (data & (1 << 1)) ? db5.set(&db5) : db5.reset(&db5);
    (data & (1 << 0)) ? db4.set(&db4) : db4.reset(&db4);

    _lcd_strobe();
}

static void _lcd_send_cmd(uint8_t data)
{
    _lcd_data_write_4bits(data >> 4);   /* Send hi 4bits */
    _lcd_data_write_4bits(data);        /* Send lo 4bits */
}

/**
 * @param pos Place of the cursor in the screen
 *
 * RS R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
 * 0   0   1  ADD ADD ADD ADD ADD ADD ADD
 */
static void _lcd_cmd_place_cursor(uint8_t pos)
{
    rs.reset(&rs);
    rw.reset(&rw);

    /* First strobe */
    db7.set(&db7);
    (pos & (1 << 6)) ? db6.set(&db6) : db6.reset(&db6);
    (pos & (1 << 5)) ? db5.set(&db5) : db5.reset(&db5);
    (pos & (1 << 4)) ? db4.set(&db4) : db4.reset(&db4);

    _lcd_strobe();

    (pos & (1 << 3)) ? db7.set(&db7) : db7.reset(&db7);
    (pos & (1 << 2)) ? db6.set(&db6) : db6.reset(&db6);
    (pos & (1 << 1)) ? db5.set(&db5) : db5.reset(&db5);
    (pos & (1 << 0)) ? db4.set(&db4) : db4.reset(&db4);

    _lcd_strobe();
}

void lcd_init(const struct iohandle (*conn)[8])
{
    uint8_t i;
    lcd_conn = conn;

    /* Configure every pin */
    for (i = 0; i < 8; i++) {
        ((*lcd_conn)[i]).config(&((*lcd_conn)[i]));
    }

    delay_ms(16);   /* Wait for more than 15 ms */

    _lcd_data_write_4bits(DISP_INIT);

    delay_ms(5);    /* Wait for more than 4.1 ms */

    _lcd_data_write_4bits(DISP_INIT);

    delay_ms(1);    /* Wait for more than 100 us */

    _lcd_data_write_4bits(DISP_INIT);

    delay_ms(1);

    _lcd_data_write_4bits(DISP_4BITS);

    delay_ms(1);

    _lcd_send_cmd(DISP_2LINES);

    /* Turn OFF */
    _lcd_send_cmd(DISP_OFF);


    lcd_clear();          /* Clear display */

    _lcd_send_cmd(EMS|EMS_INC);

    /* Turn ON*/
    _lcd_send_cmd(DISP_ON);
}

/**
 * RS R/W DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
 * 1   0   B7  B6  B5  B4  B3  B2  B1  B0
 */
void lcd_putc(uint8_t c)
{
    rs.set(&rs);
    rw.reset(&rw);

    /* First strobe */
    (c & (1 << 7)) ? db7.set(&db7) : db7.reset(&db7);
    (c & (1 << 6)) ? db6.set(&db6) : db6.reset(&db6);
    (c & (1 << 5)) ? db5.set(&db5) : db5.reset(&db5);
    (c & (1 << 4)) ? db4.set(&db4) : db4.reset(&db4);

    _lcd_strobe();

    (c & (1 << 3)) ? db7.set(&db7) : db7.reset(&db7);
    (c & (1 << 2)) ? db6.set(&db6) : db6.reset(&db6);
    (c & (1 << 1)) ? db5.set(&db5) : db5.reset(&db5);
    (c & (1 << 0)) ? db4.set(&db4) : db4.reset(&db4);

    _lcd_strobe();
}

void lcd_clear(void)
{
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
    on ? bl.set(&bl) : bl.reset(&bl);
}
