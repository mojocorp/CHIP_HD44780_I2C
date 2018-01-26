#ifndef HD44780_I2C_H
#define HD44780_I2C_H

struct display_info {
    int file;
    char functionmode;
    char entrymode;
    char displaymode;
    char backlight;
};

// HD44870 initialization
extern int hd44780_init(struct display_info* disp, int bus, int addr);
extern int hd44780_finish(struct display_info* disp);

// Write zero-terminated string to LCD
extern void hd44780_text(const struct display_info* disp, unsigned char x, unsigned char y, const char *message);

// Resets the LCD and clears the screen
extern void hd44780_reset(struct display_info* disp);

// Clears the screen
extern void hd44780_clear(const struct display_info* disp);

// Returns cursor to the home position
extern void hd44780_home(const struct display_info* disp);

// Set cursor position
extern void hd44780_set_cursor_position(const struct display_info* disp, unsigned char x, unsigned char y);

// Turn cursor on/off
extern void hd44780_cursor_on(struct display_info* disp);
extern void hd44780_cursor_off(struct display_info* disp);

// Turn display on/off
extern void hd44780_display_on(struct display_info* disp);
extern void hd44780_display_off(struct display_info* disp);

// Turn blink indicator on/off
extern void hd44780_blink_on(struct display_info* disp);
extern void hd44780_blink_off(struct display_info* disp);

// Turn backlight on/off
extern void hd44780_backlight_on(struct display_info* disp);
extern void hd44780_backlight_off(struct display_info* disp);

extern void hd44780_create_char(struct display_info* disp, unsigned char location, const unsigned char charmap[]);
extern void hd44780_write(const struct display_info* disp, unsigned char x, unsigned char y, unsigned char value);

#endif

