/***********************************************************************
 * Components:
 *   16x2 character LCD with parallel interface
 *     VSS  - GND (Power supply ground)
 *     VDD  - +5V (Positive power supply)
 *     Vo   - (Contrast)
 *     RS   - PB0 (Register Select: High for Data transfer, Low for Instruction transfer)
 *     RW   - GND (Read/Write signal: High for Read mode, Low for Write mode)
 *     E    - PB1 (Read/Write Enable: High for Read, falling edge writes data to LCD)
 *     D3:0 - NC (Data bits 3..0, Not Connected)
 *     D4   - PD4 (Data bit 4)
 *     D5   - PD5 (Data bit 5)
 *     D6   - PD6 (Data bit 3)
 *     D7   - PD7 (Data bit 2)
 *     A+K  - Back-light enabled/disabled by PB2
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <lcd.h>            // Peter Fleury's LCD library
#include <stdlib.h>         // C library. Needed for number conversions


uint8_t i = 0;  // poiter to which joystick axis we read
uint16_t ValueX = 0; // value of the axi x
uint16_t ValueY = 0; // value of the axi y
uint8_t start = 0; // 0 timer/stopwatch turn off, 1 stopwatch turn on , 2 timer turn on  
int8_t s = 0; // seconds
int8_t m = 0; // minutes
int8_t h = 0; // hours
uint8_t timepos = 0; // 0 changing sec, 1 changing min, 2 changing hours,
uint8_t menu = 0; // indicates what menu we are in
int main(void)
{
    
  // init of disp and set default menu
  lcd_init(LCD_DISP_ON);
  lcd_home();
  lcd_puts(">Stopwatch");
  lcd_gotoxy(0,1);
  lcd_puts(" Timer");

// init of ports

    GPIO_mode_input_pullup(&DDRB, PB2);
    GPIO_mode_input_pullup(&DDRB, PB3);
    GPIO_mode_input_pullup(&DDRB, PB4);
    GPIO_mode_input_pullup(&DDRB, PB5);
    GPIO_mode_output(&DDRD, PD2);
   
// init of timers
TIM2_overflow_16us();
TIM2_overflow_interrupt_enable();

TIM1_overflow_33ms();
TIM1_overflow_interrupt_enable();


 // Configure Analog-to-Digital Convertion unit
    // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
    ADMUX |= (1<<REFS0);
    ADMUX &= ~(1<<REFS1);
    // Select input channel ADC0 (voltage divider pin)
    ADMUX &= ~((1<<MUX0) | (1<<MUX1) | (1<<MUX2) | (1<<MUX3));
    // Enable ADC module
    ADCSRA |= (1<<ADEN);
    // Enable conversion complete interrupt
     ADCSRA |= (1<<ADIE);
    // Set clock prescaler to 128
    ADCSRA |= ((1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2));
    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines, ISRs */
    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************

ISR(TIMER2_OVF_vect)
{
    static uint8_t alaststate = 1;
    static uint8_t menu0pos = 0;
    static uint8_t astate = 0;
    static uint8_t no_of_overflows = 0;
    char string[8];
  

    no_of_overflows++;
    if (no_of_overflows >= 1)
    {
      no_of_overflows = 0;
      // changing pos of ">"
      astate = GPIO_read(&PINB, PB3);
      if ((astate != alaststate) & (menu == 0))
      {
        if (menu0pos == 0) {
            lcd_gotoxy(0,0);
            lcd_puts(" ");
                        
            lcd_gotoxy(0,1);
            lcd_puts(">");
            menu0pos = 1;
        } else {
            lcd_gotoxy(0,1);
            lcd_puts(" ");
                        
            lcd_gotoxy(0,0);
            lcd_puts(">");
            menu0pos = 0;
        }
      }
      alaststate = astate;
      // go to stopwatch menu1
      if (GPIO_read(&PINB, PB4) == 0 & menu == 0 & menu0pos == 0){
        menu = 1;
        lcd_clrscr();
        lcd_home();
        lcd_puts("Stopwatch");
        lcd_gotoxy(0,1);
        lcd_puts("00:00:00");

      } 
      // go to timer menu2
      else if (GPIO_read(&PINB, PB4) == 0 & menu == 0 & menu0pos == 1){ 
        menu = 2; 
        lcd_clrscr();
        lcd_home();
        lcd_puts("Timer");
        lcd_gotoxy(0,1);
        lcd_puts("00:00:00");
        lcd_gotoxy(13,0);
        lcd_puts("sec");
      }
      // return to menu0
      else if (GPIO_read(&PINB, PB4) == 0 & menu >= 1){
        menu = 0;
        start = 0;
        menu0pos = 0;
        timepos = 0;
        lcd_clrscr();
        lcd_home();
        lcd_puts(">Stopwatch");
        lcd_gotoxy(0,1);
        lcd_puts(" Timer");

        GPIO_write_low(&PORTD, PD2);
        s = 0;
        m = 0;
        h = 0;
      }
      else if (menu == 1 & GPIO_read(&PINB, PB5) == 0) // starting/stoping stopwatch
      {
        if (start == 0)
        {
          start = 1;
          lcd_gotoxy(11,1);
          lcd_puts("start");
        } 
        else
        {
          start = 0;
          lcd_gotoxy(11,1);
          lcd_puts(" stop");
        }
        
      }
      else if (menu == 2 & GPIO_read(&PINB, PB5) == 0) // starting/stoping stopwatch
      {
        if (start == 0)
        {
          start = 2;
          lcd_gotoxy(11,1);
          lcd_puts("start");
          GPIO_write_low(&PORTD, PD2);
        } 
        else
        {
          start = 0;
          lcd_gotoxy(11,1);
          lcd_puts(" stop");
         // GPIO_write_high(&PORTD, PD2);
        }
        
      }
    }
    while (GPIO_read(&PINB, PB4) == 0 | GPIO_read(&PINB, PB5) == 0){ // for joystick or encoder button to be released
    
    }
}

ISR(TIMER1_OVF_vect){
  char string[8];
  static uint8_t no_of_overflows = 0;  
  static uint8_t no_of_overflows2 = 0;  

  no_of_overflows++;
  if (no_of_overflows >= 30){
    no_of_overflows = 0;
    if (start > 0){
      if (start == 1){ // stopwatch incrementing time
          s++;
          if (s >= 60){
            s = 0;
            m++;
            if (m >= 60)
            {
              m = 0;
              h++;
            }
          }
        } else if (s > 0 | m > 0 | h > 0){ // timer decrements time 
          s--;
          if (s == -1){
            s = 59;
            m--;
            if (m == -1)
            {
              m = 59;
              h--;
            }
          }
        } else { // if time is zero stop timer 
          start = 0;
          lcd_gotoxy(11,1);
          lcd_puts(" stop");
          GPIO_write_high(&PORTD, PD2);
        }
      // code to display current timer to lcd
      itoa(h, string, 10);
      lcd_gotoxy(0,1);
      if (h <= 9){
        lcd_puts("0");
        lcd_puts(string);
      } else {
        lcd_puts(string);
      }
      lcd_puts(":");
      itoa(m, string, 10);
      if (m <= 9){
        lcd_puts("0");
        lcd_puts(string);
      } else {
        lcd_puts(string);
      }
      lcd_puts(":");
      itoa(s, string, 10);
      if (s <= 9){
        lcd_puts("0");
        lcd_puts(string);
      } else {
        lcd_puts(string);
      }
    }
  }
  no_of_overflows2++;
  if (no_of_overflows2 >= 6){
    no_of_overflows2 = 0;
    if (menu == 2 & ValueY < 200 & start == 0){ // if joystick is on left cycle timepos to left
      switch (timepos)
      {
      case 0:
        timepos = 1;
        lcd_gotoxy(13,0);
        lcd_puts("min");
        break;
      case 1:
        timepos = 2;
        lcd_gotoxy(13,0);
        lcd_puts("hod");
        break;
      case 2:
        timepos = 0;
        lcd_gotoxy(13,0);
        lcd_puts("sec");
        break;
      }
    } else if (menu == 2 & ValueY > 900 & start == 0){ // if joystick is on right cycle timepos to right
      switch (timepos)
      {
      case 0:
        timepos = 2;
        lcd_gotoxy(13,0);
        lcd_puts("hod");
        break;
      case 1:
        timepos = 0;
        lcd_gotoxy(13,0);
        lcd_puts("sec");
        break;
      case 2:
        timepos = 1;
        lcd_gotoxy(13,0);
        lcd_puts("min");
        break;
      }
    } else if (menu == 2 & ValueX > 900 & start == 0){ // if joystick is on top increment time depending on timepos 
      switch (timepos)
      {
      case 0:
        if (s != 59){
          s++;
          itoa(s, string, 10);
          lcd_gotoxy(6,1);
          if (s <= 9){
            lcd_puts("0");
            lcd_puts(string);
          } else {
            lcd_puts(string);
          }
        }
        break;
      case 1:
        if (m != 59){
          m++;
          itoa(m, string, 10);
          lcd_gotoxy(3,1);
          if (m <= 9){
            lcd_puts("0");
            lcd_puts(string);
          } else {
            lcd_puts(string);
          }
        }
        break;
      case 2:
        if (h != 59){
          h++;
          itoa(h, string, 10);
          lcd_gotoxy(0,1);
          if (h <= 9){
            lcd_puts("0");
            lcd_puts(string);
          } else {
            lcd_puts(string);
          }
        }
        break;
      }
    } else if (menu == 2 & ValueX < 200  & start == 0){ // if joystick is on bottom decrement time depending on timepos 
      switch (timepos)
      {
      case 0:
        if (s != 0){
          s--;
          itoa(s, string, 10);
          lcd_gotoxy(6,1);
          if (s <= 9){
            lcd_puts("0");
            lcd_puts(string);
          } else {
            lcd_puts(string);
          }
        }
        break;
      case 1:
        if (m != 0){
          m--;
          itoa(m, string, 10);
          lcd_gotoxy(3,1);
          if (m <= 9){
            lcd_puts("0");
            lcd_puts(string);
          } else {
            lcd_puts(string);
          }
        }
        break;
      case 2:
        if (h != 0){
          h--;
          itoa(h, string, 10);
          lcd_gotoxy(0,1);
          if (h <= 9){
            lcd_puts("0");
            lcd_puts(string);
          } else {
            lcd_puts(string);
          }
        }
        break;
      }
    }
  }
    if (i == 0){ 
    ADMUX &= ~((1<<MUX0) | (1<<MUX1) | (1<<MUX2) | (1<<MUX3)); // select input channel A0
    i = 1;
    } else {
    ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3)); // select input channel A1
    ADMUX |= (1<<MUX0); // select input channel A1
    i = 0;
    }
    ADCSRA |= (1<<ADSC); // start
  
}

ISR(ADC_vect){
  uint16_t value;
  char string[4]; 
  value = ADC;
  if (i==1) //  i = 1 means we are reading from X axis
  {
    ValueX = value; 

    itoa(value, string, 10);

  }
  else //  i = 0 means we are reading from Y axis
  {
    ValueY = value;
    itoa(value, string, 10);

  }
}
