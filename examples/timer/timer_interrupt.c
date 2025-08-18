#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "utils/uart_utils.h"

// FONDAMENTALE definirla volatile se no il compiler può ottimizzare
// volatile: var che può cambiare in qualsiasi momento anche quando il compiler non lo sa
//              la ISR non è contemplata dal compiler quindi lui vede che non cambia mai nel programma
//              e la ottimizza con un valore costante in un registro
volatile uint8_t interrupt_occured = 0;
uint16_t interrupt_count = 0;

// Routine eseguita quando avviene l'interrupt
ISR(TIMER1_COMPA_vect) {
    interrupt_occured = 1;
    interrupt_count++;
}

int main() {
    UART_init(19200);
    printf_init();

    // Usiamo timer 1 che è a 16bit

    // setta prescaler a 1024
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12); 

    // clock = 16MHz , prescaler = 1024 --> freq = 16M / 1024 = 15625
    // Ogni secondo il timer fa 15625 tick
    // Se voglio un evento ogni k secondi deve essere OCR = 15625 * k
    const int event_interval_ms = 500;
    uint16_t ocrval = (uint16_t) (15.625 * event_interval_ms);

    OCR1A = ocrval; // 15.625 * 500 = 7812
    
    cli(); // disabilita interrupt
    // abilita possibilità del timer di provocare interrupt
    TIMSK1 |= (1 << OCIE1A);
    sei(); // riabilita interrupt

    while (1) {
        if (interrupt_occured) {
            interrupt_occured = 0; // reset flag
            printf("interrupt no.%u\n", interrupt_count);
        }

        // blocca l'esecuzione a tempo indeterminato, si sblocca con un nuovo interrupt
        sleep_cpu();
    }
}
