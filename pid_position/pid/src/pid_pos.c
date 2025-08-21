#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdlib.h>
#include <math.h>
#include "uart_utils.h"

// FORSE VA IMPLEMENTATA LA FUNZIONE micros(), COSI' COM'E' FORSE
// PUO DARE PROBLEMI (AL MOMENTO E' CON UN INTERRUPT INTERNO OGNI 20ms PER IL TASK)

// tempo di campionamento 20ms per avere freq campionamento 50Hz
#define DELTA_T_MS 20

int pos = 0; // starting shaft position in ticks
float eprev = 0;
float eintegral = 0;

// [BEGIN] INTERNAL INTERRUPT ------------------------------------------
volatile uint8_t interrupt_occured = 0;
uint16_t interrupt_count = 0;

// routine eseguita ogni delta_t
ISR(TIMER0_COMPA_vect) {
    interrupt_occured = 1;
    //interrupt_count++;
}

void timer_internal_int_init() {
    // Usiamo timer 0

    // setta prescaler a 1024
    TCCR0A = 0;
    TCCR0B = (1 << WGM02) | (1 << CS00) | (1 << CS02); 

    // clock = 16MHz , prescaler = 1024 --> freq = 16M / 1024 = 15625
    // Ogni secondo il timer fa 15625 tick
    // Se voglio un evento ogni k secondi deve essere OCR = 15625 * k
    uint16_t ocrval = (uint16_t) (15.625 * DELTA_T_MS);

    OCR0A = ocrval;

    cli(); // disabilita interrupt
    TIMSK0 |= (1 << OCIE0A); // abilita possibilità del timer di provocare interrupt
    sei(); // riabilita interrupt
}
// [END] ---------------------------------------------------------------



// [BEGIN] EXTERNAL INTERRUPT ------------------------------------------
const uint8_t enc_pin_mask = (1 << 2) | (1 << 3);
volatile uint8_t external_int_occurred = 0;

void external_int_init() {
    DDRD &= ~enc_pin_mask; // PD2 as input

    // abilita interrupt 0 (abilitiamo interrupt solo su uno dei due pin dell'encoder)
    EIMSK |= 1 << INT0;

    // configura int0 per triggerarsi su rising edge
    EICRA = (1 << ISC01) | (1 << ISC00);

    // abilita gli interrupt globali
    sei();
}

ISR(INT0_vect) {
  external_int_occurred = 1;
}
// [END] ---------------------------------------------------------------



// [BEGIN] PWM ---------------------------------------------------------
// inverted logic (?) perchè pwm va al contrario
//#define TCCRA_MASK (1 << WGM10) | (1 << COM1A0) | (1 << COM1A1)
#define TCCRA_MASK (1 << WGM10) | (1 << COM1A1)
#define TCCRB_MASK ((1 << WGM12) | (1 << CS10))

void timer_pwm_init() {
    TCCR1A = TCCRA_MASK;
    TCCR1B = TCCRB_MASK;

    // Setting dell'OCR 
    OCR1AH = 0;
    OCR1BH = 0;
    //OCR1CH = 0; // ATMega only

    // initial duty cycle at 0
    OCR1AL = 0;
}
// [END] ---------------------------------------------------------------



void read_encoder() {
    const int mask = (1 << 3);

    // read ENCB pin state
    int b = !((PIND & mask) == 0); // not per via del pullup

    if (b)
        pos++;
    else
        pos--;
}

void setMotor(int dir, int duty_cycle){
    const uint8_t pwm_pin_mask = (1 << 1);
    const uint8_t in1_pin_mask = (1 << 2);
    const uint8_t in2_pin_mask = (1 << 3);
    const uint8_t mask = pwm_pin_mask | in1_pin_mask | in2_pin_mask;

    DDRB |= mask; // PB1,2,3 as output

    // setting dutycycle (and so voltage intensity)
    uint16_t my_duty_cycle = (uint16_t) duty_cycle;
    OCR1AL = my_duty_cycle;

    // setting direction
    if (dir == 1) {
        PORTB |= in1_pin_mask; // in1 HIGH
        PORTB &= ~in2_pin_mask; // in2 LOW
    }
    else if (dir == -1) {
        PORTB &= ~in1_pin_mask; // in1 LOW
        PORTB |= in2_pin_mask; // in2 HIGH
    }
    else {
        PORTB &= ~in1_pin_mask; // in1 LOW
        PORTB &= ~in2_pin_mask; // in2 LOW
    }  
}

void PID(int target) {
    // PID parameters
    float kp = 1;
    float kd = 0.0; //0.025;
    float ki = 0.0005;

    // error
    int e = pos - target;

    // derivative
    float dedt = (e - eprev) / (DELTA_T_MS * 1000);

    // integral
    eintegral = eintegral + e * (DELTA_T_MS * 1000);

    // control signal
    float u = (kp * e) + (kd * dedt) + (ki * eintegral);

    // motor speed signal 
    // (it has to be a pwm duty cycle so it has to be positive and between 0 and 255 to prevent overflow)
    float u_pwm = fabs(u);

    // cap it to max value
    if( u_pwm > 255 )
        u_pwm = 255;
    // SE E' MINORE DI 0? o -255?

    // motor direction
    int dir = 1;
    if (u < 0)
        dir = -1;

    // actuate command
    setMotor(dir, u_pwm);

    // store previous error
    eprev = e;

    printf("%d, %d\n", target, pos);
}

int main() {
    // initialize UART and printf wrapper
    UART_init(19200);
    printf_init();

    // initialize timer 0 for tempo di campionamento at 50Hz per fare il task
    timer_internal_int_init();

    // initialize timer 1 for pwm
    timer_pwm_init();

    // initialize external interrupts for encoder
    external_int_init();

    int target = 1000;

    while (1) {
        if (interrupt_occured) {
            interrupt_occured = 0; // reset flag
            PID(target); // do task
        }
        if (external_int_occurred) {
            external_int_occurred = 0; // reset flag
            read_encoder(); // do task
        }

        // blocca l'esecuzione a tempo indeterminato, si sblocca con un nuovo interrupt
        sleep_cpu();
    }
}