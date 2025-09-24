#include "shared_types.h"
#include "peripherals_utils.h"

// [BEGIN] INTERNAL INTERRUPT ------------------------------------------
volatile uint64_t timer1_overflow_count = 0;

void timer_internal_init() {
    // Set Timer 1 to Normal Mode
    TCCR1A = 0;
    TCCR1B = 0;

    // Set prescaler to 8
    TCCR1B |= (1 << CS11);

    // Enable Timer 1 overflow interrupt
    TIMSK1 |= (1 << TOIE1);

    // Reset the counter
    TCNT1 = 0;
}

// Interrupt Service Routine for Timer 1 Overflow
ISR(TIMER1_OVF_vect) {
    timer1_overflow_count++;
}

// returns microseconds passed since call of timer_internal_init()
uint64_t micros() {
    uint64_t overflow_count_before;
    uint64_t overflow_count;
    uint16_t timer_val;

    // CONDIZIONE RARA: timer_val e overflow_count sono letti non atomicamente quindi se subito dopo aver letto timer_val il timer fa overflow
    //                  si considereranno timer_val tick in più 
    // SOLUZIONE: prima di andare avanti bisogna continuare a leggere timer_val fino a che l'overflow_count è lo stesso che c'era prima di leggere
    do {
        overflow_count_before = timer1_overflow_count;
        timer_val = TCNT1;
        overflow_count = timer1_overflow_count;
    } while (overflow_count_before != overflow_count);

    // Timer1 settato per fare 1 tick ogni 0.5us --> dividi per 2
    return (overflow_count * 65536 + timer_val) / 2;
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
    
    // set Waveform Generation Mode 14 (Fast PWM with TOP = ICR4)
    TCCR4A |= (1 << WGM41);
    TCCR4B |= (1 << WGM43) | (1 << WGM42);

    // set the TOP value to set resolution 2^16-1 = 65535 --> 16bit resolution
    ICR4 = 65535;

    // set  prescaler to 1 --> frequency = clock / (prescaler * ICR4) = 16MHz / (1 * 65535) = 244Hz
    TCCR4B |= (1 << CS40);
    
    /*
    TCCR4A |= (1 << WGM41) | (1 << WGM40); // set Fast PWM mode
    TCCR4B |= (1 << CS41) | (1 << CS40); // set prescaler to 64
    */

    // Setting del canale
    switch (pin) {
        case PH3:
            TCCR4A |= (1 << COM4A1); // set non-inverting PWM on OC4A (pin 6)
            OCR4A = 0; // initial duty cycle at 0
            break;
        case PH4:
            TCCR4A |= (1 << COM4B1); // set non-inverting PWM on OC4B (pin 7)
            OCR4B = 0; // initial duty cycle at 0
            break;
        case PH5:
            TCCR4A |= (1 << COM4C1); // set non-inverting PWM on OC4C (pin 8)
            OCR4C = 0; // initial duty cycle at 0
            break;
        default:
            break;
    }

    // Setting del pin come output
    const uint8_t mask = (1 << pin);
    DDRH |= mask;
}
// [END] ---------------------------------------------------------------
