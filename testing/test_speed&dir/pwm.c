#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/uart_utils.h"

// inverted logic (?) perchè pwm va al contrario
//#define TCCRA_MASK (1 << WGM10) | (1 << COM1A0) | (1 << COM1A1)
#define TCCRA_MASK (1 << WGM10) | (1 << COM1A1)
#define TCCRB_MASK ((1 << WGM12) | (1 << CS10))

#define MAX_BUF 256

int main() {
    UART_init(19200);
    printf_init();

    // Usiamo timer 1

    TCCR1A = TCCRA_MASK;
    TCCR1B = TCCRB_MASK;

    // Setting dell'OCR 
    OCR1AH = 0;
    OCR1BH = 0;
    //OCR1CH = 0; // ATMega only

    OCR1AL = 1;

    char buf[MAX_BUF];

    // pwm pin
    const uint8_t mask = (1 << 1);

    // directions pins
    const uint8_t mask1 = (1 << 2);
    const uint8_t mask2 = (1 << 3);

    DDRB |= (mask | mask1 | mask2);

    while (1) {
        uint8_t size = UART_getstring((uint8_t*) buf);
        //if (!strcmp(buf, EXIT_MSG))

        short number = (short) atoi(buf);

        if (!strcmp(buf, "0\n") || number) {
            // CW
            if (number < 0) {
                PORTB |= mask1; // in1 HIGH
                PORTB &= (~mask2); // in2 LOW
            }
            // CCW
            else if (number > 0) {
                PORTB &= (~mask1); // in1 LOW
                PORTB |= mask2; // in2 HIGH
            }

            uint16_t duty_cycle = (uint16_t) abs(number);
            OCR1AL = duty_cycle;
            printf("dt: %u\n", duty_cycle);
        }
        else
            printf("enter a valid dt\n");
    }
}