#include "motor_utils.h"

uint64_t    pos1 = 0; // starting shaft position in ticks
int         dir1 = 1; // motor direction 1: CW ; -1: CCW ; 0: still
uint64_t    pos2 = 0;
int         dir2 = 0;

float eprev = 0;
float eintegral = 0;

volatile uint8_t external_int_enc1_occurred = 0;
volatile uint8_t external_int_enc2_occurred = 0;

volatile uint8_t previous_portb_state = 0;
volatile int32_t encoder1_pos = 0;
volatile int32_t encoder2_pos = 0;

ISR(PCINT0_vect) {
    uint8_t current_portb_state = PINB;
    uint8_t changed_pins = current_portb_state ^ previous_portb_state; // xor to get different bits

    // filter for only the pins that had a rising edge [its not possible to set rising edge directly on PCINTs]
    uint8_t rising_edge_pins = changed_pins & current_portb_state;
    
    // check if ENCA for encoder 1 changed
    if (rising_edge_pins & ((1 << PB0)))
        external_int_enc1_occurred = 1;

    // check if ENCA for encoder 2 changed
    if (rising_edge_pins & (1 << PB1))
        external_int_enc2_occurred = 1;

    // store the new state for the next interrupt
    previous_portb_state = current_portb_state;
}

void encoder_PCINT0_init() {
    const uint8_t pin_mask = 0x0F; // first 4 pins (PB0,1,2,3)
    
    //set pin_mask pins as input
    DDRB &= ~pin_mask;

    //enable pull up resistors
    PORTB |= pin_mask;

    // set interrupt on change, looking up PCMSK0
    PCICR |= (1 << PCIE0);

    // set PCINT0 group (pins PCINT0,1,2,3) to trigger an interrupt on state change
    PCMSK0 |= pin_mask;

    // enable global interrupts
    sei();
}

// if ENCA rised: a step has been done.
// Then, depending on ENCB state, the direction is deduced
void read_encoder1() {
    const int mask = (1 << PB2);

    // read ENCB pin state
    int b = !((PINB & mask) == 0); // not per via del pullup

    if (b)
        pos1++;
    else
        pos1--;
}

void read_encoder2() {
    const int mask = (1 << PB3);

    // read ENCB pin state
    int b = !((PINB & mask) == 0); // not per via del pullup

    if (b)
        pos2++;
    else
        pos2--;
}

void set_motor(int in1, int in2, int dir, int u_pwm) {
    const uint8_t in1_pin_mask = (1 << in1);
    const uint8_t in2_pin_mask = (1 << in2);

    DDRA |= (in1_pin_mask | in2_pin_mask); // PB2,3 as output

    // setting dutycycle (and so voltage intensity)
    uint16_t duty_cycle = (uint16_t) u_pwm;
    OCR4A = duty_cycle;

    // setting direction
    if (dir == 1) {
        PORTA |= in1_pin_mask; // in1 HIGH
        PORTA &= ~in2_pin_mask; // in2 LOW
    }
    else if (dir == -1) {
        PORTA &= ~in1_pin_mask; // in1 LOW
        PORTA |= in2_pin_mask; // in2 HIGH
    }
    else {
        PORTA &= ~in1_pin_mask; // in1 LOW
        PORTA &= ~in2_pin_mask; // in2 LOW
    }
}

void PID1(int target) {
    // error
    int e = target - pos1;

    // derivative
    float dedt = (e - eprev) / (DELTA_T_MS / 1000.0);

    // integral
    eintegral = eintegral + e * (DELTA_T_MS / 1000.0);

    // control signal
    float u = (KP * e) + (KD * dedt) + (KI * eintegral);

    // motor speed signal 
    // (it has to be a pwm duty cycle so it has to be positive and between 0 and 255 to prevent overflow)
    float u_pwm = fabs(u);

    // cap it to max value
    if(u_pwm > 255)
        u_pwm = 255;
    // SE E' MINORE DI 0? o -255?

    // motor direction
    dir1 = 1;
    if (u < 0)
        dir1 = -1;

    // actuate command
    set_motor(PA0, PA2, dir1, u_pwm);

    // store previous error
    eprev = e;

    //printf("%d, %d\n", target, pos);
}