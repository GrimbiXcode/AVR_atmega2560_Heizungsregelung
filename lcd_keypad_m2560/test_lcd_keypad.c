/* lcdtest.c
 * V1.1, (c) 2015 Matthias Meier
 * example and test frame for Mega2560 LCD Keypad using from C
 * derived from WinAVR-Demo-Projekt WinAVR-20070525\examples\stdiodemo
 */


#include <avr/io.h> 
#include <util/delay.h> 
#include <stdio.h>

#include "lcd_keypad_m2560.h"

#define BTNMASK ((1<<5)|(1<<4)) // buttons on Port A Bit 4..5


int main()
{
  char buttons;

  static FILE lcd_fd = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &lcd_fd; // set stdout to lcd stream initalized above

  init_lcd();

  DDRC = 0xff;    // set LED-Port to output
  PORTA = BTNMASK;  // enable pullups on buttons
  
  while (1)
  { 
    buttons = (~PINA & BTNMASK) >> 4;   // read buttons ans shift to LSB
    printf("Hey Raphael \rEs funzt!! %d \n",buttons); // print both lines of lcd display
    PORTC=buttons;
  _delay_ms(100);
  }
}