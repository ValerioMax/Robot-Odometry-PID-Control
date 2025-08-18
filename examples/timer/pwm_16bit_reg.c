#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "utils/uart_utils.h"

// Timer 1 has 16bit counter
// We use 16bit register so we count up to 65535
// (in this example we use only the first 8bit (OCR1AL) anyway
// but we could have a much more granular dutycycle using all 16bit)

// Cofigurazione PWM
// fast PWM, 8 bit, non inverted (?)
// output compare set low
#define TCCRA_MASK (1 << WGM10) | (1 << COM1A0) | (1 << COM1A1)
#define TCCRB_MASK ((1 << WGM12) | (1 << CS10))  

int main() {
    UART_init(19600);
    printf_init();

    // Usiamo timer 1
    TCCR1A = TCCRA_MASK;
    TCCR1B = TCCRB_MASK;

    // Setting dell'OCR ---------------------------------

    // clear di tutti gli High bits degli OCR
    // in modo da assicurarsi che la PWM sia a 8 bit
    // [serve strettamente solo per OCR1A ma è good practise farlo per tutti]
    OCR1AH = 0;
    OCR1BH = 0;
    //OCR1CH = 0; // ATMega only

    // setting dell'OCR (duty cycle) (dato solo dal registro inferiore)
    OCR1AL = 1;

    // Setting del pin in output ------------------------
    const uint8_t mask = (1 << 1);
    DDRB |= mask;

    uint8_t intensity = 0;
    while (1) {
        // a ogni iterazione varia il duty cycle della PWM
        OCR1AL = intensity;

        printf("duty cycle: %u\n", (int) OCR1AL);

        _delay_ms(100);
        intensity += 8;
    }
}