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
// tempo di logging su seriale 2000ms
#define DELTA_T_LOG_MS 2000

int pos_prev = 0;
int pos = 0; // starting shaft position in ticks
float v_tick = 0;
float v_rpm = 0;
int dir = 1; // motor direction 1: CW, -1: CCW, 0: still
float eprev = 0;
float eintegral = 0;

// [BEGIN] INTERNAL INTERRUPT ------------------------------------------
volatile uint8_t internal_int_sample_occured = 0;
uint16_t internal_int_sample_count = 0;
volatile uint8_t internal_int_log_occured = 0;
uint16_t internal_int_log_count = 0;

// routine eseguita ogni delta_t
ISR(TIMER1_COMPA_vect) {
    internal_int_sample_occured = 1;
    internal_int_sample_occured++;
}

ISR(TIMER2_COMPA_vect) {
    internal_int_log_count++;
    if (internal_int_log_count >= 200) { // Check if 200 * 10ms = 2000ms has passed
        internal_int_log_count = 0;
        internal_int_log_occured = 1;
        // This code runs every 2000ms
    }
}

void timer_internal_sample_init() {
    // Usiamo timer 1 per il tempo di campionamento a 50Hz

    // setta prescaler a 1024
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);

    // clock = 16MHz , prescaler = 1024 --> freq = 16M / 1024 = 15625
    // Ogni secondo il timer fa 15625 tick
    // Se voglio un evento ogni k secondi deve essere OCR = 15625 * k
    uint16_t ocrval = (uint16_t) (15.625 * DELTA_T_MS);

    OCR1A = ocrval;

    cli(); // disabilita interrupt
    TIMSK1 |= (1 << OCIE1A); // abilita possibilità del timer di provocare interrupt
    sei(); // riabilita interrupt
}

void timer_internal_log_init() {
    // Usiamo timer 2 per il logging ogni 1sec

    // Set CTC mode (WGM21) and prescaler to 1024
    TCCR2A |= (1 << WGM21);
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
    
    uint8_t ocrval = (uint8_t) (15.625 * (DELTA_T_LOG_MS / 200));
    OCR2A = ocrval; // Set the compare value

    cli();
    TIMSK2 |= (1 << OCIE2A); // Enable the Timer2 Compare A interrupt
    sei();
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



void read_encoder() {
    const int mask = (1 << 3);

    // read ENCB pin state
    int b = (PIND & mask) == 0; // not per via del pullup

    if (b)
        pos++;
    else
        pos--;

    // FORSE COSI' E' IMPRECISO (IMPLEMENTARE micros())

}

void set_motor(int dir, int u_pwm){
    const uint8_t in1_pin_mask = (1 << 2);
    const uint8_t in2_pin_mask = (1 << 3);

    DDRB |= (in1_pin_mask | in2_pin_mask); // PB2,3 as output

    // setting dutycycle (and so voltage intensity)
    uint8_t duty_cycle = (uint8_t) u_pwm;
    OCR0A = duty_cycle;

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
    float kp = 3;
    float kd = 0.0; //0.025;
    float ki = 0.5; //0.0005;

    // Compute velocity
    v_tick = (pos - pos_prev) / (DELTA_T_MS / 1000.0);
    pos_prev = pos;

    // Convert velocity from tick/sec to rpm
    v_rpm = v_tick / 600.0*60.0;

    // TODO AGGIUNGI FILTRO PASSA BASSO
    // ...

    // error
    //int e = target - v_rpm;
    int e = target - v_rpm;

    // derivative
    float dedt = (e - eprev) / (DELTA_T_MS / 1000.0);

    // integral
    eintegral = eintegral + e * (DELTA_T_MS / 1000.0);

    // control signal
    float u = (kp * e) + (kd * dedt) + (ki * eintegral);

    // motor power
    float u_pwm = fabs(u);

    if(u_pwm > 255)
        u_pwm = 255;
    // SE E' MINORE DI 0? o -255?

    // motor direction
    dir = 1;
    if (u < 0)
        dir = -1;

    // signal the motor
    set_motor(dir, u_pwm);

    // store previous error
    eprev = e;

    //printf("%d, %d\n", target, pos);
}

int main() {
    // initialize UART and printf wrapper
    UART_init(19200);
    printf_init();

    // initialize timer 1 for tempo di campionamento at 50Hz per fare il task
    timer_internal_sample_init();

    timer_internal_log_init();

    // initialize timer 0 for pwm
    timer_pwm_init();

    // initialize external interrupts for encoder
    external_int_init();

    int target = 100;

    while (1) {
        if (external_int_occurred) {
            external_int_occurred = 0; // reset flag
            read_encoder(); // do task
            //printf("ex %d, in %d, pos %d\n", external_int_count, internal_int_count, pos);
            //printf("%d %d\n", external_int_count, pos);
        }
        if (internal_int_sample_occured) {
            internal_int_sample_occured = 0; // reset flag
            PID(target); // do task
            //printf("int1\n");
        }
        if (internal_int_log_occured) {
            internal_int_log_occured = 0; // reset flag
            //printf("int2\n");
            printf("ext %d, trg %d, vel %d, pos %d, err %d, dtc %u, dir %d\n", external_int_count, target, (int) v_rpm, pos, (int) (target-v_rpm), OCR0A, dir);
            // NOTA: external_int_count va in overflow dopo un pò (not a problem)
        }
        
        // blocca l'esecuzione a tempo indeterminato, si sblocca con un nuovo interrupt
        sleep_cpu();
    }
}