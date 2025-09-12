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



// [BEGIN] EXTERNAL INTERRUPT ------------------------------------------
const uint8_t enc_pin_mask = (1 << 2) | (1 << 3);
volatile uint8_t external_int_occurred = 0;
uint16_t external_int_count = 0;

ISR(INT0_vect) {
  external_int_occurred = 1;
  external_int_count++;
}

void external_int_init() {
    DDRD &= ~enc_pin_mask; // PD2 as input

    PORTD |= enc_pin_mask; // pullup su PD0 (NOTA: COSI' LAVORIAMO A LOGICA INVERTITA CON L'ENCODER)

    // abilita interrupt 0 (abilitiamo interrupt solo su uno dei due pin dell'encoder)
    EIMSK |= 1 << INT0;

    // configura int0 per triggerarsi su rising edge
    EICRA = (1 << ISC01) | (1 << ISC00);

    // abilita gli interrupt globali
    sei();
}
// [END] ---------------------------------------------------------------


// [BEGIN] PWM ---------------------------------------------------------
// inverted logic (?) perchè pwm va al contrario
// COM0A1 (non-inverting mode for OC0A), WGM01 and WGM00 (Fast PWM mode)
#define TCCRA_MASK (1 << COM0A1) | (1 << WGM01) | (1 << WGM00)
#define TCCRB_MASK (1 << CS01) | (1 << CS00) // Set prescaler to 64

void timer_pwm_init() {
    TCCR0A = TCCRA_MASK;
    TCCR0B = TCCRB_MASK;

    // Setting dell'OCR 
    // initial duty cycle at 0
    OCR0A = 0;

    // Setting del pin in output (PD6)
    const uint8_t mask = (1 << 6);
    DDRD |= mask;
}
// [END] ---------------------------------------------------------------
