#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "utils/uart_utils.h"

volatile uint8_t interrupt_occurred=0;
uint16_t int_count=0;

//const uint8_t mask = (1 << 0); // for ATMega
const uint8_t mask = (1 << 2);  // for Uno (Uno only has PD2 and PD2 as External Interrupt pins
                                // (all the others are PCINT (Pin Change External Interrupt)))
const uint8_t trigger_mask = (1 << 0);

ISR(INT0_vect) {
  interrupt_occurred = 1;
  int_count++;
}

int main() {
    UART_init(19200);
    printf_init();

    DDRD &= ~mask; // PD0 as input
    PORTD |= mask; // pullup su PD0

    // abilita interrupt 0
    EIMSK |= 1 << INT0;

    // configura int0 per triggerarsi su rising edge
    EICRA = (1 << ISC01) | (1 << ISC00);

    // abilita gli interrupt globali
    sei();

    DDRB |= trigger_mask;

    while(1){
        if (interrupt_occurred) {
            interrupt_occurred = 0;
            printf("int %u\n", int_count);
        }

        PORTB &= ~trigger_mask;
        _delay_ms(500);
        PORTB |= trigger_mask;
        _delay_ms(500);

        // TOGLIENDO cli() e sei() IL PROGRAMMA SI BLOCCA QUANDO SCRIVO QUALCOSA DA TERMINALE
        // METTENDOLI LA UART_getstring RIMANE BLOCCANTE MA QUANDO INVIO QUALCOSA POI L'ESECUZIONE RIPRENDE
        // NON HO CAPITO BENE PERCHE'!!!!!!!!
        cli();
        uint8_t buf[256];
        uint8_t siz = UART_getstring(buf);
        sei();
    }
}