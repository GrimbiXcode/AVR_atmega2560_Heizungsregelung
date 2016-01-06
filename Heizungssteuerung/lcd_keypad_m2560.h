#ifndef LCD_KEYPAD_M2560_H
#define LCD_KEYPAD_M2560_H
/* lcd_keypad_m2560.h
 * C driver for SainSmart "LCD Keypad" Shield on Mega2560 baseboard.
 * 
 *  V1.0, (c) 2015, Matthias Meier (cleanup lcd timing and refactoring in C)
 * Derived from Arduino C++ keypad_lcd library (LCD4Bit_mod.cpp)
 * Remark: neither author nor license condition are known for Arduino lcd_keypad library

 * lcd_keypad_m2560.c/h is a lcd driver for the SainSmart LCD Keypad Module
 *
 * To use this driver module in a C project:
 * 1.) copy lcd_keypad_m2560.h and lcd_keypad_m2560.c to the project folder
 * 2.) add lcd_keypad_m2560.c to your project source files
 * 4.) include this headerfile on top of your main module
 * 3.) define globally in the project settings F_CPU to the clock frequency
 * 5.) initialize the driver at start of main() by:
 *
 *   static FILE fd_lcd = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
 *   stdout = &fd_lcd; // set stdout to point to the lcd file descriptor above
 *
 *   init_lcd();
 *
 *   printf("that's it!");
 */

#include <stdio.h>  // used for FILE struct

/*
 * function used by FDEV_SETUP_STREAM() to write a character to the lcd display:
 * '\n' clears the display after the *next* character
 * '\r' sets the cursor address to begin of the second display line
 * other characters are sent to the display
 */
extern int lcd_putchar(char c, FILE *unused);

/* 
 * initiatize lcd 
 * cursor and blink settings could be modified after initializing
 */
extern void init_lcd(void);

#endif // LCD_KEYPAD_M2560_H
