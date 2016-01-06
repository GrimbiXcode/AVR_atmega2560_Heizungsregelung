/********************************************************************
* Project:          SimpleClock
* Description:      Einfache interrupt-gesteuerte 24h Uhr.
* Developer:        David Grimbichler
* Organization:     FHNW Brugg-Windisch
* File:             SimpleClock.c
* Compiler:         avr-gcc
* Date:             04.12.2015
********************************************************************/

/********************************************************************
* Includes and defines
*
********************************************************************/
#include <avr/io.h> 
#include <util/delay.h> 
#include <stdio.h>
#include <avr/interrupt.h>

#include "lcd_keypad_m2560.h"

  
#define LEDPORT PORTA
#define LEDDDR DDRA
#define HEARTBEAT_LED 0b10
 
#define BTNMASK ((1<<5)|(1<<4)) // buttons on Port A Bit 4..5
 
/********************************************************************
* Variables & Prototypes
* volatile -> optimierung ausgeschaltet
********************************************************************/
//Variablen für die Zeit
volatile unsigned long millisekunden;
volatile unsigned long WritableMS;
volatile char bDoUpdate;
volatile unsigned int g_adValue = 0;

/********************************************************************
* Init-code
*
********************************************************************/
/*===================================================================
* Inititalisation of Ports
* Input: -
* Return: -
===================================================================*/
void initIO(void)
{
  LEDDDR = HEARTBEAT_LED; //identisch mit: DDRD = 0b00000010
  DDRC = 0xff;    // set LED-Port to output
  DDRA = 0x00;    // set Porta as input
  PORTA = BTNMASK;  // enable pullups on buttons
}
/*===================================================================
* end of function
===================================================================*/


/*===================================================================
* Inititalisation of ADConverter
* Input: -
* Return: -
===================================================================*/
void init_adc(void)
{
  ADCSRA = 0b10001110;    // Enable the ADC and its interrupt feature
                          // and set the ACD clock pre-scalar to clk/64
  ADMUX  = 0b01000000;    // Select internal 2.56V as Vref, left justify 
                    // data registers and select ADC0 as input channel 
  // Global Interrupts aktivieren
  sei();
  //ADMUX = 0b01000000; // VCC ref , Left adj ,Single ended ADC0
  //ADCSRA = 0b10000111; // enable Presc:125kHz
}
/*===================================================================
* end of function
===================================================================*/


/*===================================================================
* Inititalisation of ADConverter
* Input: -
* Return: -
===================================================================*/
/*void init_timerInterrupt(void)
{
  // Timer 0 konfigurieren
  TCCR0A = (1<<WGM01); // CTC Modus
  TCCR0B |= (1<<CS01)|(1<<CS00); // Prescaler 8
  // ((1000000/8)/1000) = 125
  OCR0A = 255;
 
  // Compare Interrupt erlauben
  TIMSK0 |= (1<<OCIE0A);
 
  // Global Interrupts aktivieren
  sei();
}*/
/*===================================================================
* end of function
===================================================================*/

/********************************************************************
* Routines
*
********************************************************************/

/*===================================================================
* Reading of ADConverter
* Input: -
* Return: -
===================================================================*/
unsigned int read_adc(void)
{
  unsigned int adValue = 0;
  ADCSRA |= 0b01000000; // start convertion
  while(ADCSRA & 0b01000000)
  {
  }
  adValue = ADC;
  return adValue;
}
/*===================================================================
* end of function
===================================================================*/

/*===================================================================
* Decoding ADC to Tastencode 0 to 5
* Input: -
* Return: -
===================================================================*/
unsigned char decode(unsigned int adValue)
{
  if(adValue >= 1000)  return 0;
  if(adValue >= 700)  return 1;
  if(adValue >= 450)  return 2;
  if(adValue >= 300)   return 3;
  if(adValue >= 100)   return 4;
  else                  return 5;
}
/*===================================================================
* end of function
===================================================================*/


/*===================================================================
* Update Display
* Input: -
* Return: -
===================================================================*/
void updateDisplay(void)
{
    printf("Ist  Temp: %d\rSoll Temp:\n",g_adValue); // print both lines of lcd display
}
/*===================================================================
* end of function
===================================================================*/

/********************************************************************
* Main-code
*
********************************************************************/
int main(void)
{
  //unsigned int adValue = 0;

  static FILE lcd_fd = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &lcd_fd; // set stdout to lcd stream initalized above
  
  millisekunden = 0;
  bDoUpdate = 0;
  g_adValue = 0;
  
  initIO();
  init_lcd();
  init_adc();
  updateDisplay();
  ADCSRA |= 0b01000000;
  while (1)
  { 
    updateDisplay();
    //adValue = read_adc();
    //printf("Code:%d RAW=%u\rCalc. = %umV\n",decode(adValue),adValue,(unsigned int)(adValue*2560L/1023L)); // print both lines of lcd display
    //_delay_ms(100); // waiting for display
    /*if(bDoUpdate == 1)
    {
      PORTC = (~PORTC & HEARTBEAT_LED);
      bDoUpdate = 0;
      updateDisplay();
    }*/
  }
}

/********************************************************************
* End of code
********************************************************************/


/********************************************************************
* Interrupt-Routines
*
********************************************************************/

/*===================================================================
* ADC Conversion Complete Interrupt Service Routine (ISR)
===================================================================*/
ISR(ADC_vect)
{
  g_adValue = ADC;
  ADCSRA |= 0b01000000; // start convertion
}

/*===================================================================
* Der Compare Interrupt Handler 
* wird aufgerufen, wenn 
* TCNT0 = OCR0A = 125-1 
* ist (125 Schritte), d.h. genau alle 1 ms
===================================================================*/
/*ISR (TIMER0_COMPA_vect)
{
  millisekunden++;

  if(!(millisekunden % 10))
    bDoUpdate = 1;
    WritableMS = millisekunden;*/
  /*if(millisekunden == 1000)
  {
    sekunde++;
    millisekunden = 0;
    if(sekunde == 60)
    {
      minute++;
      sekunde = 0;
    }
    if(minute == 60)
    {
      stunde++;
      minute = 0;
    }
    if(stunde == 24)
    {
      stunde = 0;
    }
  }
}*/
/*===================================================================
* end of function
===================================================================*/




//  char buttons;

//    PORTC=buttons;
//    buttons = (~PINA & BTNMASK) >> 4;   // read buttons ans shift to LSB






/*===================================================================
* Function-Description
* Input: -
* Return: -
===================================================================*/










