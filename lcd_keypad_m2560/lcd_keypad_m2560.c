/* lcd_keypad_m2560.c
 * C driver for SainSmart "LCD Keypad" Shield on Mega2560 Baseboard
 *  V1.0, (c) 2015, Matthias Meier (cleanup lcd timing and refactoring in C)
 * Derived from Arduino C++ keypad_lcd library (LCD4Bit_mod.cpp)
 * Remark: neither author nor license condition are known for Arduino lcd_keypad library
*/


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "lcd_keypad_m2560.h"

//command bytes for LCD
#define CMD_CLR 0x01
#define CMD_RIGHT 0x1C
#define CMD_LEFT 0x18
#define CMD_HOME 0x02
#define CMD_DDADDR(addr) (0x80 | ((addr) & 0x7f))


// --------- PINS -------------------------------------
//is the RW pin assignment of the LCD under our control?  
// If we're only ever going to write to the LCD, we can use one less microcontroller pin, and just tie the LCD pin to the necessary signal, high or low.
//this stops us sending signals to the RW pin if it isn't being used.
#define USING_RW 0

//original pin assignments of Arduino C++ keypad_lcd library
//static int RS = 8;
//static int RW = 11;
//static int Enable = 9;
//#define DB[] = {4, 5, 6, 7};  //wire these to DB4~7 on LCD.

//pin assignments on Arduino Mega2560
#define	DB0_PORT	G	//DB0 = DB7 on display (because of 4bit Mode)
#define	DB0_BIT		5
#define	DB1_PORT	E
#define	DB1_BIT		3
#define DB2_PORT	H
#define	DB2_BIT		3
#define DB3_PORT	H
#define	DB3_BIT		4

#define	RS_PORT		H
#define	RS_BIT		5
#define	EN_PORT		H
#define EN_BIT 		6
#define	RW_PORT		B
#define	RW_BIT		5
//#define	BL_PORT		B	// (external pullup enables backlight when PB4 is defined as input)
//#define	BL_BIT		4


//nr of lines of the LCD
#define g_num_lines 2

//port helper macros
#define GLUE(a, b)     a##b
#define PORT(x)        GLUE(PORT, x)
#define PIN(x)         GLUE(PIN, x)
#define DDR(x)         GLUE(DDR, x)
#define OUTPUT			1
#define digitalWrite(port, bitnr, val) {if (val) PORT(port) |= 1<<bitnr; else PORT(port) &= ~(1<<bitnr);}
#define pinMode(port, bitnr, val) {if (val) DDR(port) |= 1<<bitnr; else DDR(port) &= ~(1<<bitnr);}

//--------------------------------------------------------


//pulse the Enable pin high (for a microsecond).
static void pulseEnablePin(){
  digitalWrite(EN_PORT, EN_BIT, 1);
  _delay_us(1);
  digitalWrite(EN_PORT, EN_BIT, 0);
}

//push a nibble of data through the the LCD's DB4~7 pins, clocking with the Enable pin.
static void pushNibble(int value){
  int val_nibble= value & 0x0F;  //clean the value.  (unnecessary)
  digitalWrite(DB0_PORT, DB0_BIT,val_nibble & 1);
  digitalWrite(DB1_PORT, DB1_BIT,val_nibble>>1 & 1);
  digitalWrite(DB2_PORT, DB2_BIT,val_nibble>>2 & 1);
  digitalWrite(DB3_PORT, DB3_BIT,val_nibble>>3 & 1);
  pulseEnablePin();
}

//push a byte of data through the LCD's DB4~7 pins, in two steps, clocking each with the enable pin.
static void pushByte(int value){
  int val_lower = value & 0x0F;
  int val_upper = value >> 4;
  pushNibble(val_upper);
  pushNibble(val_lower);
  _delay_us(27);
}

static void commandWriteNibble(int nibble) {
  digitalWrite(RS_PORT, RS_BIT, 0);
  if (USING_RW) { digitalWrite(RW_PORT, RW_BIT, 0); }
  pushNibble(nibble);
  _delay_us(27);
}

static void commandWrite(int value) {
  digitalWrite(RS_PORT, RS_BIT, 0);
  if (USING_RW) { digitalWrite(RW_PORT, RW_BIT, 0); }
  pushByte(value);
  //Remark: some commands needs additional delay!
}

static void dataWrite(int value) {
  digitalWrite(RS_PORT, RS_BIT, 1);
  if (USING_RW) { digitalWrite(RW_PORT, RW_BIT, 0); }
  pushByte(value);
}

//send the clear screen command to the LCD
static void clear(){
  commandWrite(CMD_CLR);
  _delay_ms(2);
}

/*
 * function to redirect stdout to lcd by FDEV_SETUP_STREAM() macro
 * Sends a character to the LCD display.
 * '\n' clears the display after the *next* character
 * '\r' sets the cursor address to begin of 2nd line
 */
int lcd_putchar(char c, FILE *unused)
{
  static char nl_seen;
  if (nl_seen && c != '\n') {
      // first character after newline, clear display and home cursor.
      clear();
      nl_seen = 0;
  }
  if (c == '\n') {
      nl_seen = 1;
  }
  else if (c != '\r') {
      dataWrite(c);
  } else {	
      commandWrite(CMD_DDADDR(64));
  }
  return 0;
}


// initiatize lcd - cursor and blink settings could be overriden after initializing
void init_lcd() {
  pinMode(EN_PORT, EN_BIT, OUTPUT);
  pinMode(RS_PORT, RS_BIT, OUTPUT);
  if (USING_RW) { pinMode(RW_PORT, RW_BIT, OUTPUT); }
  pinMode(DB0_PORT, DB0_BIT,OUTPUT);
  pinMode(DB1_PORT, DB1_BIT,OUTPUT);
  pinMode(DB2_PORT, DB2_BIT,OUTPUT);
  pinMode(DB3_PORT, DB3_BIT,OUTPUT);
  digitalWrite(EN_PORT, EN_BIT, 0);

  _delay_ms(50);

  //The first 4 nibbles and timings are not in my DEM16217 SYH datasheet, but apparently are HD44780 standard...
  commandWriteNibble(0x03);
  _delay_ms(5);
  commandWriteNibble(0x03);
  _delay_us(100);
  commandWriteNibble(0x03);
  _delay_ms(5);

  // needed by the LCDs controller
  //this being 2 sets up 4-bit mode.
  commandWriteNibble(0x02);
  commandWriteNibble(0x02);
  //todo: make configurable by the user of this library.
  //NFXX where
  //N = num lines (0=1 line or 1=2 lines).
  //F= format (number of dots (0=5x7 or 1=5x10)).
  //X=don't care

  int num_lines_ptn = (g_num_lines - 1) << 3;
  int dot_format_ptn = 0x00;      //5x7 dots.  0x04 is 5x10

  commandWriteNibble(num_lines_ptn | dot_format_ptn);
  _delay_us(60);

  //The rest of the init is not specific to 4-bit mode.
  //NOTE: we're writing full bytes now, not nibbles.

  // display control:
  // turn display on, cursor off, no blinking
  commandWrite(0x0C);
  _delay_us(60);

  //clear display
  commandWrite(0x01);
  _delay_ms(3);

  // entry mode set: 06
  // increment automatically, display shift, entire shift off
  commandWrite(0x06);

  _delay_ms(1);//TODO: remove unnecessary delays
}


//non-core stuff --------------------------------------
//move the cursor to the given absolute position.  line numbers start at 1.
//if this is not a 2-line LCD4Bit_mod instance, will always position on first line.
void cursorTo(int line_num, int x){
  //first, put cursor home
  commandWrite(CMD_HOME);

  //if we are on a 1-line display, set line_num to 1st line, regardless of given
  if (g_num_lines==1){
    line_num = 1;
  }
  //offset 40 chars in if second line requested
  if (line_num == 2){
    x += 40;
  }
  //advance the cursor to the right according to position. (second line starts at position 40).
  while (x-- > 0) {
    commandWrite(0x14);
  }
}

//scroll whole display to left
void leftScroll(int num_chars, int delay_time){
  while (num_chars-- > 0) {
    commandWrite(CMD_LEFT);
    int i=delay_time;
    while (i-- > 0)
		_delay_ms(1);
    //_delay_ms(delay_time);
  }
}
