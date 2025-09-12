#include "motor_utils.h"

int pos = 0; // starting shaft position in ticks
int dir = 1; // motor direction 1: CW, -1: CCW, 0: still
float eprev = 0;
float eintegral = 0;

void read_encoder() {
    const int mask = (1 << 3);

    // read ENCB pin state
    int b = !((PIND & mask) == 0); // not per via del pullup

    if (b)
        pos++;
    else
        pos--;
}

void set_motor(int dir, int u_pwm){
    const uint8_t in1_pin_mask = (1 << 2);
    const uint8_t in2_pin_mask = (1 << 3);

    DDRB |= (in1_pin_mask | in2_pin_mask); // PB2,3 as output

    // setting dutycycle (and so voltage intensity)
    uint16_t duty_cycle = (uint16_t) u_pwm;
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
    // error
    int e = pos - target;

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
    dir = 1;
    if (u < 0)
        dir = -1;

    // actuate command
    set_motor(dir, u_pwm);

    // store previous error
    eprev = e;

    //printf("%d, %d\n", target, pos);
}