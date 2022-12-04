/***********************************************************************
 * 
 * Stopwatch by Timer/Counter2 on the Liquid Crystal Display (LCD)
 *
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2017 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
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
#include <uart.h>

/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Update stopwatch value on LCD screen when 8-bit 
 *           Timer/Counter2 overflows.
 * Returns:  none
 **********************************************************************/
uint8_t i = 0;
uint16_t ValueX = 0;
uint16_t ValueY = 0;
uint8_t start = 0;
int8_t s = 0;
int8_t m = 0;
int8_t h = 0;
uint8_t timepos = 0;
uint8_t menu = 0;
int main(void)
{


  // Initialize UART to asynchronous, 8N1, 9600
  uart_init(UART_BAUD_SELECT(9600, F_CPU));
    
    lcd_init(LCD_DISP_ON);

    
    lcd_home();
    lcd_puts(">Stopwatch");
    lcd_gotoxy(0,1);
    lcd_puts(" Timer");

    
//
// ports

    GPIO_mode_input_pullup(&DDRB, PB2);
    GPIO_mode_input_pullup(&DDRB, PB3);
    GPIO_mode_input_pullup(&DDRB, PB4);
    GPIO_mode_input_pullup(&DDRB, PB5);
    GPIO_mode_output(&DDRD, PD2);


    
//
    // Configuration of 8-bit Timer/Counter2 for Stopwatch update
    // Set the overflow prescaler to 16 ms and enable interrupt
    TIM2_overflow_16us();
    TIM2_overflow_interrupt_enable();

    TIM1_overflow_262ms();
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
 * Function: Timer/Counter2 overflow interrupt
 * Purpose:  Update the stopwatch on LCD screen every sixth overflow,
 *           ie approximately every 100 ms (6 x 16 ms = 100 ms).
 **********************************************************************/
ISR(TIMER2_OVF_vect)
{
    static uint8_t alaststate = 1;
    static uint8_t menu0pos = 0;
    static uint8_t astate = 0;
    static uint8_t no_of_overflows = 0;
    char string[8];
  

    no_of_overflows++;
    if (no_of_overflows >= 2)
    {
      no_of_overflows = 0;
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

        uart_puts("stopw\r\n");
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

        uart_puts("timer\r\n");
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

        uart_puts("return\r\n");

        s = 0;
        m = 0;
        h = 0;
      }
      else if (menu == 1 & GPIO_read(&PINB, PB5) == 0)
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
      else if (menu == 2 & GPIO_read(&PINB, PB5) == 0)
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
          GPIO_write_high(&PORTD, PD2);
        }
        
      }
    }
    while (GPIO_read(&PINB, PB4) == 0 | GPIO_read(&PINB, PB5) == 0){
    
    }
}

ISR(TIMER1_OVF_vect)
{
char string[8];
static uint8_t no_of_overflows = 0;  

no_of_overflows++;
if (no_of_overflows >= 4){
  no_of_overflows = 0;
  if (start > 0){
    if (start == 1){
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
    } else {
      if (s > 0 | m > 0 | h > 0){
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
      } else if ((s == 0) & (m == 0) & (h == 0)){
        start = 0;
        lcd_gotoxy(11,1);
        lcd_puts(" stop");
        GPIO_write_high(&PORTD, PD2);
      }
      
    }
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
  if (menu == 2 & ValueY > 900 & ValueX > 400 & ValueX < 600 & start == 0){
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
  }
  if (menu == 2 & ValueY < 200 & ValueX > 400 & ValueX < 600 & start == 0){
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
  }
  if (menu == 2 & ValueX > 900 & ValueY > 400 & ValueY < 600 & start == 0){
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
  }
  if (menu == 2 & ValueX < 200 & ValueY > 400 & ValueY < 600 & start == 0){
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
ADMUX &= ~((1<<MUX0) | (1<<MUX1) | (1<<MUX2) | (1<<MUX3));
i = 1;
} else {
ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3));
ADMUX |= (1<<MUX0);
i = 0;
}
ADCSRA |= (1<<ADSC);

}
ISR(ADC_vect){
  uint16_t value;
  char string[4]; 
  value = ADC;
  if (i==1) // X
  {
    ValueX = value;

    itoa(value, string, 10);
    uart_puts(string); 
    uart_puts("   ");
  }
  else // Y
  {
    ValueY = value;
    itoa(value, string, 10);
    uart_puts(string); 
    uart_puts("\r\n");
  }
  
  

  //itoa(i, string, 10);
  
  //uart_puts(string);

  
}