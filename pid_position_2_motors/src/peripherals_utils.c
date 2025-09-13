#include "shared_types.h"
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

// [BEGIN] EXTERNAL INTERRUPT PCINT ------------------------------------
void external_int_PCINT_init(const uint8_t pin_mask, int port) {
    // switch on every port with a PCINT
    switch (port) {
        // PCINT0 port
        case PORT_B:
            //set pin_mask pins as input
            DDRB &= ~pin_mask;

            //enable pull up resistors
            PORTB |= pin_mask;

            // set interrupt on change, looking up PCMSK0
            PCICR |= (1 << PCIE0);

            // set some of the PCINT0 group pins to trigger an interrupt on state change
            PCMSK0 |= pin_mask;

            // enable global interrupts
            sei();
            break;
        // PCINT1 port
        case PORT_J:
            break;
        // PCINT2 port
        case PORT_K:
            break;
    }
}
// [END] ---------------------------------------------------------------

// [BEGIN] PWM ---------------------------------------------------------
void pwm_TIMER4_init(int pin) {
    TCCR4A |= (1 << WGM41) | (1 << WGM40); // set Fast PWM mode
    TCCR4B |= (1 << CS41) | (1 << CS40); // set prescaler to 64

    // Setting del canale
    switch (pin) {
        case PH3:
            TCCR4A |= (1 << COM4A1);
            break;
        case PH4:
            TCCR4A |= (1 << COM4B1);
            break;
        case PH5:
            TCCR4A |= (1 << COM4C1);
            break;
    }
    
    // Setting dell'OCR 
    // initial duty cycle at 0
    OCR4A = 0;
    OCR4B = 0;
    OCR4C = 0;

    // Setting del pin come output
    const uint8_t mask = (1 << pin);
    DDRH |= mask;
}
// [END] ---------------------------------------------------------------
