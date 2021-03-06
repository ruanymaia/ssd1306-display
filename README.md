# OLED display AVR interface
![project](https://github.com/ruanymaia/ssd1306-display/blob/main/20201230_194731.jpg)

## How I built it
I based my project on [this repository](https://github.com/efthymios-ks/AVR-SSD1306) available on GitHub. 
As my objective is to print only text onto the screen, I simplified the code as much as I could to do just this specific task. I had to learn some concepts of electronics and embedded systems programming and made a lot of improvements on the code based on it and on CS50 classes. 

## Hardware
- Atmel ATMega328P microcontroller;
- SSD1306-based OLED display with I2C communication ports;
- AVR Programmer.

### Circuit diagram 
![display-circuit](https://github.com/ruanymaia/weather-station/blob/main/display-circuit.png)

## Software

### Requirements
- Compiler: GCC-AVR package;
- Programming Software: AVRdude.

### How to build
- `avr-gcc -mmcu=atmega328p -o main.o -Os main.c SSD1306.c TWI.c`
- `avr-objcopy -j .text -j .data -O ihex  main.o  main.hex`

### How to burn it into the AVR
- `sudo avrdude -c usbtiny -p m328p -U flash:w:main.hex`

### How it works
This library prints text on 128x64 pixels SSD1306-based OLED display connected with AVR Atmel ATmega328P via I2C (TWI) communication protocol.