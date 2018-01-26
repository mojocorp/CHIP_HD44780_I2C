#include <stdio.h>
#include <unistd.h>
#include <time.h>

extern "C" {
#include "HD44780_I2C.h"
}

unsigned char bell[8]  = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
unsigned char note[8]  = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
//unsigned char clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
unsigned char heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
unsigned char duck[8]  = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
unsigned char check[8] = {0x0, 0x1 ,0x3, 0x16, 0x1c, 0x8, 0x0};
unsigned char cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
unsigned char retarrow[8] = {	0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};

int main(int argc, char**argv)
{
    const char i2c_address = 0x3f;
    const int  i2c_bus = 2;

    display_info disp;

    if(hd44780_init(&disp, i2c_bus, i2c_address))
        printf("init failed\n");

    hd44780_create_char(&disp, 0, bell);
    hd44780_create_char(&disp, 1, heart);

    int i=0;
    while(true) {
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        time (&rawtime);
        timeinfo = localtime (&rawtime);

        strftime (buffer,80,"%e %b     %R",timeinfo);

        if (i++ % 2 == 0)
            buffer[13] = ' ';
        
        hd44780_text(&disp, 0, 0, buffer);
        hd44780_write(&disp, 0, 1, 0);
        hd44780_text(&disp, 1, 1, "\1Tilda&Lisbeth\1");
        sleep(1);
    }

    hd44780_finish(&disp);
    
    return 0;
}
