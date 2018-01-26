#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

#include "HD44780_I2C.h"

// Define some device constants
#define LCD_CHR 1 // Mode - Sending data
#define LCD_CMD 0 // Mode - Sending command

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYMODESET 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHTON 0x08
#define LCD_BACKLIGHTOFF 0x00

#define ENABLE 0b00000100 // Enable bit

// Timing constants
#define E_PULSE 500 // us
#define E_DELAY 500 // us

static int i2c_open(struct display_info* disp, int bus, int addr)
{
    char path[PATH_MAX];
    if (snprintf(path, PATH_MAX, "/dev/i2c-%d", bus) >= PATH_MAX) {
        return -3;
    }

    disp->file = open(path, O_RDWR | O_NONBLOCK);
    if (disp->file < 0)
        return -1;

    if (ioctl(disp->file, I2C_SLAVE, addr) < 0)
        return -2;

    return 0;
}

static int i2c_write_byte(const struct display_info* disp, unsigned char bits)
{
    if (write(disp->file, &bits, 1) != 1)
        return -1;
    return 0;
}

static int i2c_close(struct display_info* disp)
{
    if (close(disp->file) < 0)
        return -1;

    return 0;
}

// Send byte in 4-bit mode.
static int send_byte(const struct display_info* disp, unsigned char bits, char mode)
{
    char bits_high = mode | (bits & 0xF0) | disp->backlight;
    char bits_low = mode | ((bits<<4) & 0xF0) | disp->backlight;

    // Write high bits and toggle ENABLE pin in us 
    i2c_write_byte(disp, (bits_high | ENABLE));
    usleep(E_PULSE);
    i2c_write_byte(disp,(bits_high & ~ENABLE));
    usleep(E_DELAY);

    // Write low bits and toggle ENABLE pin in us 
    i2c_write_byte(disp, (bits_low | ENABLE));
    usleep(E_PULSE);
    i2c_write_byte(disp,(bits_low & ~ENABLE));
    usleep(E_DELAY);

    return 0;
}

int hd44780_init(struct display_info* disp, int bus, int addr)
{
    int ret = i2c_open(disp, bus, addr);

    if (ret == 0) {
        disp->functionmode = LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
        send_byte(disp,0x33,LCD_CMD);// 110011 Initialise
        send_byte(disp,0x32,LCD_CMD);// 110010 Initialise
        send_byte(disp, disp->functionmode, LCD_CMD);
        hd44780_reset(disp);
  }
    return ret;
}

void hd44780_reset(struct display_info* disp)
{
    disp->displaymode = LCD_DISPLAYMODESET | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    disp->entrymode = LCD_ENTRYMODESET | LCD_CURSORSHIFT | LCD_ENTRYSHIFTINCREMENT;
    disp->backlight = LCD_BACKLIGHTON;
    send_byte(disp, disp->displaymode, LCD_CMD);
    send_byte(disp, disp->entrymode, LCD_CMD);
    hd44780_clear(disp);
}

void hd44780_clear(const struct display_info* disp)
{
    send_byte(disp, LCD_CLEARDISPLAY, LCD_CMD);
    usleep(2000);
}

void hd44780_home(const struct display_info* disp)
{
    send_byte(disp, LCD_RETURNHOME, LCD_CMD);
    usleep(2000);
}

void hd44780_set_cursor_position(const struct display_info* disp, unsigned char x, unsigned char y)
{
    const int row_address[] = { 0x00, 0x40, 0x14, 0x54 };
    send_byte(disp, LCD_SETDDRAMADDR | (x + row_address[y]), LCD_CMD);
}

int hd44780_finish(struct display_info* disp)
{
    return i2c_close(disp);
}

void hd44780_text(const struct display_info* disp, unsigned char x, unsigned char y, const char *message)
{
    hd44780_set_cursor_position(disp, x, y);
    while (*message)
    {
        send_byte(disp, *message++, LCD_CHR);
    }
}

void hd44780_display_on(struct display_info* disp)
{
    disp->displaymode |= LCD_DISPLAYON;
    send_byte(disp, disp->displaymode, LCD_CMD);
}

void hd44780_display_off(struct display_info* disp)
{
    disp->displaymode &= ~LCD_DISPLAYON;
    send_byte(disp, disp->displaymode, LCD_CMD);
}

void hd44780_cursor_on(struct display_info* disp)
{
    disp->displaymode |= LCD_CURSORON;
    send_byte(disp, disp->displaymode, LCD_CMD);
}

void hd44780_cursor_off(struct display_info* disp)
{
    disp->displaymode &= ~LCD_CURSORON;
    send_byte(disp, disp->displaymode, LCD_CMD);
}

void hd44780_blink_on(struct display_info* disp)
{
    disp->displaymode |= LCD_BLINKON;
    send_byte(disp, disp->displaymode, LCD_CMD);
}

void hd44780_blink_off(struct display_info* disp)
{
    disp->displaymode &= ~LCD_BLINKON;
    send_byte(disp, disp->displaymode, LCD_CMD);
}

void hd44780_backlight_on(struct display_info* disp)
{
    disp->backlight = LCD_BACKLIGHTON;
    i2c_write_byte(disp, disp->backlight);
}

void hd44780_backlight_off(struct display_info* disp)
{
    disp->backlight = LCD_BACKLIGHTOFF;
    i2c_write_byte(disp, disp->backlight);
}

void hd44780_create_char(struct display_info* disp, unsigned char location, const unsigned char charmap[])
{
    int i;
    location &= 0x7; // we only have 8 locations 0-7
    send_byte(disp, LCD_SETCGRAMADDR | (location << 3), LCD_CMD);    
    for (i=0; i<8; i++) {
        send_byte(disp, charmap[i], LCD_CHR);
    }
}

void hd44780_write(const struct display_info* disp, unsigned char x, unsigned char y, unsigned char value)
{
    hd44780_set_cursor_position(disp, x, y);
    send_byte(disp, value, LCD_CHR);
}
