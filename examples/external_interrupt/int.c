#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "utils/uart_utils.h"

/*
    Arduino UNO:
    Su PD2 viene abilitato l'interrupt INT0 su rising edge
    PB2 viene togglato ogni 500ms
    Collegare PB0 con PD2: ogni volta che PB0 va HIGH si triggera l'interrupt

    [si può anche pizzicare PD2 con il GND per triggerare l'int. essendo PD2 col pullup]
    [ NOTA: pizzicando a mano il counter incrementa di molti valori alla volta perché il segnale
            ci mette un pò a stabilizzarsi per via del contatto non preciso e quindi fa su e giù ]
*/

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
            interrupt_occurred = 0; // reset the flag;
            printf("int %u\n", int_count);
        }

        // trigger interrupt (PB0 goes LOW but PD0 is HIGH because of its pullup, so PD0 state changes to LOW)
        PORTB &= ~trigger_mask;
        _delay_ms(500);

        // just go HIGH, nothing happens (cause INT0 was set on rising edge)
        PORTB |= trigger_mask;
        _delay_ms(500);
    }
}

// NOTA: Se un interrupt interno o esterno capita durante la CPU è ferma
//       (che sia per _delay_ms() o per sleep_cpu()) viene triggerato lo stesso
//       --> viene eseguita la ISR e poi la CPU torna a dormire