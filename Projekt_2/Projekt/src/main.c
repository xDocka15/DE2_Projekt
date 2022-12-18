#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <gpio.h>           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <stdlib.h>         // C library. Needed for number conversions



static uint8_t servo = 0;
int main(void)
 { 

  TIM0_overflow_16ms(); // set over flow to 16ms
  TIM0_overflow_interrupt_enable(); // enable Timer0 overflow

  GPIO_mode_input_pullup(&DDRD, PD5); // set pin PD5(4) as input with pullup

  DDRB |= (1<<PB3);    // Fast PWM output at OCR2A pin
  DDRD |= (1<<PD3);    // Fast PWM output at OCR2B pin
	TCCR2A |= (1<<COM2A1) | (1<<COM2B1) | (1<<WGM21) | (1<<WGM20);	// clears OC2A and OC2B on compare match, Fast PWM
	TCCR2B |= (1<<CS22) | (1<<CS21);	// Prescaler set to 256

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

ISR(TIMER0_OVF_vect){
  static uint8_t no_of_overflows = 0;
  no_of_overflows++;
  if (no_of_overflows >= 4){
    no_of_overflows = 0;
    if (GPIO_read(&PIND, PD5) == 0){ // if button is pressed value of servo variable is changed between 0 and 1
      if (servo == 0){
        servo = 1;
      } else {
        servo = 0;
      } 
    }
    while (GPIO_read(&PIND, PB5) == 0){} // waits for  button to be released
    ADCSRA |= (1<<ADSC); // starts ADC conversion
  }
}

ISR(ADC_vect){
  uint16_t value; // variable where we save ADC value
  char string[4]; 
  value = ADC;  // saving ADV value to value
  if (servo == 1){  // changes the duty cycle on PB3 or PD3 depending on servo variable
    OCR2A = ((((value/5)*(105/5))/(1016/5)) + 40/5)*5; // converts value with range 0-1023 to new value with range 40-145
  } else {
    OCR2B = ((((value/5)*(105/5))/(1016/5)) + 40/5)*5; // converts value with range 0-1023 to new value with range 40-145
  }
}
