#include "peripherals_utils.h"

// [BEGIN] INTERNAL INTERRUPT ------------------------------------------
uint64_t internal_int_count = 0;

ISR(TIMER1_COMPA_vect) {
    internal_int_count++;
}

void timer_internal_init() {
    // Usiamo timer 1 per implementare la funzione millis()

    // setta prescaler a 1024
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);

    // clock = 16MHz , prescaler = 1024 --> freq = 16M / 1024 = 15625
    // Ogni secondo il timer fa 15625 tick
    // Se voglio un evento ogni k secondi deve essere OCR = 15625 * k
    uint16_t ocrval = (uint16_t) (15.625);

    OCR1A = ocrval;

    cli(); // disabilita interrupt
    TIMSK1 |= (1 << OCIE1A); // abilita possibilità del timer di provocare interrupt
    sei(); // riabilita interrupt
}

// return milliseconds passed since start of the program
uint64_t millis() {
    return internal_int_count;
}
// [END] ---------------------------------------------------------------

// [BEGIN] PWM ---------------------------------------------------------
// inverted logic (?) perchè pwm va al contrario
// COM0A1 (non-inverting mode for OC0A), WGM01 and WGM00 (Fast PWM mode)
#define TCCRA_MASK (1 << COM4A1) | (1 << WGM41) | (1 << WGM40)
#define TCCRB_MASK (1 << CS41) | (1 << CS40) // Set prescaler to 64

void timer_pwm_PH_init(int pin) {
    TCCR4A = TCCRA_MASK;
    TCCR4B = TCCRB_MASK;

    // Setting dell'OCR 
    // initial duty cycle at 0
    OCR4A = 0;

    // Setting del pin in output (PD6)
    const uint8_t mask = (1 << pin);
    DDRH |= mask;
}
// [END] ---------------------------------------------------------------
