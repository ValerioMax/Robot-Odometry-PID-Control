#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "utils/uart_utils.h"

int main() {
    UART_init(19600);
    printf_init();

    // Usiamo timer 1

    // disabilita PWM e interrupt connessi al timer 1
    TCCR1A = 0;

    // setta il prescaler per dividere il clock di 1024
    TCCR1B = (1 << CS12) | (1 << CS10);

    while (1) {
        // legge valore del counter del timer
        uint16_t timer_value = TCNT1;
        printf("timer value: %u\n", timer_value);
        _delay_ms(1000);
    }
}