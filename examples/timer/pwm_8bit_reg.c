#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "utils/uart_utils.h"

// Timer 1 has 8bit counter
// We use 8bit register so we count only up to 255

// Cofigurazione PWM
// COM0A1 (non-inverting mode for OC0A), WGM01 and WGM00 (Fast PWM mode)
#define TCCRA_MASK (1 << COM0A1) | (1 << WGM01) | (1 << WGM00)
#define TCCRB_MASK (1 << CS01) | (1 << CS00) // Set prescaler to 64

int main() {
    UART_init(19600);
    printf_init();

    // Usiamo timer 0
    TCCR0A = TCCRA_MASK;
    TCCR0B = TCCRB_MASK;

    // Setting dell'OCR ---------------------------------

    // initial duty cycle
    OCR0A = 1;

    // Setting del pin in output ------------------------
    const uint8_t mask = (1 << 6);
    DDRD |= mask;

    uint8_t intensity = 0;
    while (1) {
        // a ogni iterazione varia il duty cycle della PWM
        //OCR1AL = intensity;
        OCR0A = intensity;

        printf("duty cycle: %u\n", (int) OCR0A);

        _delay_ms(100);
        intensity += 8;
    }
}