#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>

int main(void) {
    const uint_8t mask = (1 << 2); // mette 1 all'indice 2: 00000100
    DDRB |= mask; // setta PB2 in output

    while (1) {
        PORTB |= mask; // PB2 HIGH
        _delay_ms(3000);
        PORTB &= (~mask); // PB2 LOW
        _delay_ms(3000);
    }
}