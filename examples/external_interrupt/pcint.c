#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "utils/uart_utils.h"

#define PIN_MASK 0x0F // last four bit of PORT B (50-53)

// DA VEDERE (vedi anche /testing/test_encoder.c per i PinChangeINT)

volatile uint8_t previous_pins;
volatile uint8_t current_pins;

volatile uint8_t int_occurred=0;
volatile uint16_t int_count=0;

// interrupt routine for position PCINT0
ISR(PCINT0_vect) {
  previous_pins=current_pins;
  current_pins=PINB&PIN_MASK;
  int_occurred=1;
  int_count=1;
}

int main(void){
  UART_init(19600);
  printf_init();

  //set pin_mask pins as input
  DDRB &= ~pin_mask;

  //enable pull up resistors
  PORTB |= pin_mask;

  // set interrupt on change, looking up PCMSK0
  PCICR |= (1 << PCIE0);

  // set PCINT0 group (pins PCINT0,1,2,3) to trigger an interrupt on state change
  PCMSK0 |= PIN_MASK;
  
  // enable global interrupts
  sei();

  while(1) {
    if (int_occurred) {
      // we reset the flag;
      int_occurred=0;
      printf("int %u, p:%x, c:%x!\n", int_count, previous_pins, current_pins);
    }
    sleep_cpu();
  }
}
