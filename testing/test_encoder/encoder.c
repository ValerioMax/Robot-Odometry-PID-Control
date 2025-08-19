#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "../utils/uart_utils.h"

// PROBLEMA (?): QUALCHE VOLTA direction assume valori diversi da -1, 0, 1
// PROBLEMA (?): forse la printf/UART_getstring oppure usare altri pin della porta 
//                  da problemi se si usano gli interrupt di gruppo (PCINT e non INT)

//const uint8_t mask_a = (1 << 0);
//const uint8_t mask_b = (1 << 1);
//const uint8_t mask = (1 << 0) | (1 << 1);
#define MASK_A (1 << 0)
#define MASK_B (1 << 1)
#define MASK (MASK_A | MASK_B)

// NOTA: la concatenazione prev-curr è uguale all'indice in decimale
//  0: stato invariato o transizione impossibile
//  1: shaft gira in senso CW
// -1: shaft gira in senso CCW
// [vedi disegno delle onde quadre sfasate]
const int8_t transition_table[] = {
            // idx  prev    curr
      0,    // 0    00      00
      1,    // 1    00      01
     -1,    // 2    00      10
      0,    // 3    00      11    
     -1,    // 4    01      00
      0,    // 5    01      01
      0,    // 6    01      10
      1,    // 7    01      11
      1,    // 8    10      00
      0,    // 9    10      01
      0,    // 10   10      10
     -1,    // 11   10      11
      0,    // 12   11      00
     -1,    // 13   11      01
      1,    // 14   11      10
      0     // 15   11      11
};

// External Interrupt settings
volatile uint8_t interrupt_occured = 0;

uint8_t prev_state = MASK;
uint8_t curr_state = MASK;

int counter = 0;
int direction = 0;

uint8_t idx = 0;

ISR(PCINT0_vect) {
    interrupt_occured = 1;
    
    curr_state = PINB; // read entire port values

    // compute index in the lookup table
    // "& mask" serve solo per sicurezza nel caso si abbiano altri pin attivi nella porta che fanno altro 
    idx = (prev_state << 2) | (curr_state & MASK);
    /*
        Es:
        Si passa da 11 a 11:
        prev = 0000011
        curr = 0000011

        prev<<2 = 0001100
        curr    = 0000011
        &       = 0001111
        --> output è 15
        
        Es:
        Si passa da 01 a 00:
        prev = 0000001
        curr = 0000000

        prev<<2 = 0000100
        curr    = 0000000
        &       = 0000100
        --> output è 4
    */

    int value = transition_table[idx];
    direction = value;
    counter += abs(value); // non sicuro di abs()

    // next iteration
    prev_state = curr_state;
}

int main() {
    UART_init(19200);
    printf_init();

    DDRB &= ~MASK; // ENA, ENB come input
    PORTB |= MASK; // pullup su ENA e ENB

    // abilita interrupt per il gruppo 0
    PCICR |= (1 << PCIE0);

    // specifica quali pin del gruppo possono triggerare interrupt on change
    PCMSK0 |= MASK;

    // abilita intettupt globali
    sei();

    while (1) {
        if (interrupt_occured) {
            interrupt_occured = 0;
            printf("dir: %d, count: %d, idx: %d, mask: %x\n", direction, counter, idx, MASK);
        }
        sleep_cpu();
    }
}