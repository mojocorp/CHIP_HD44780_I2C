# HD44780_I2C for Next Thing C.H.I.P

HD44780 compatible LCDs library, interfaced via a PCF8574 I2C serial extender for the [Next Thing C.H.I.P](https://getchip.com).

<img src="https://raw.github.com/mojocorp/CHIP_HD44780_I2C/master/1602-i2c.jpg" >

## Tested LCD sizes

* 1602: 16 characters, 2 lines
* 2004: 20 characters, 4 lines

## Wiring

| CHIP              | DISPLAY |
|:-----------------:|:-------:|
| GND (U13/1)       | GND     |
| VCC-5V (U13/3)    | VCC     |
| TWI2-SDA (U14/25) | SDA     |
| TWI2-SCK (U14/26) | SCL     |

## Detect lcd

  sudo i2cdetect -y 2
  
 
