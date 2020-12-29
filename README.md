# OLED display AVR interface

This library prints text on 128x64 pixels SSD1306-based OLED displays connected with AVR Atmel ATmega328P via I2C communication protocol.

## How I built it
I based my project on [this repository](https://github.com/efthymios-ks/AVR-SSD1306) available on GitHub. 
As my objective is to print only text onto the screen, I simplified the code as much as I could to do just this specific task. I had to learn some concepts of electronics and embedded systems programming and made a lot of improvements on the code based on it and on CS50 classes. 

## Hardware
* Atmel ATMega328P microcontroller;
* SSD1306-based OLED display with I2C communication ports.
* AVR Programmer;

### Circuit diagram 
![display-circuit](https://github.com/ruanymaia/weather-station/blob/main/display-circuit.png)

## Software

### Requirements
* Compiler: GCC-AVR package;
* Programming Software: AVRdude.

### How to build
- `avr-gcc -mmcu=atmega328p -Os main.c -o main.o`
- `avr-objcopy -j .text -j .data -O ihex  main.o  main.hex`

### How to burn it into the AVR
- `sudo avrdude -c usbtiny -p m328p -U flash:w:main.hex`

### How it works

TWI (AVR abstraction for I2C), master slave, matrix of pixels, font in the program memory 