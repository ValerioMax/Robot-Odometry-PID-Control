#include "motor_utils.h"

int pos_prev = 0;
int pos = 0; // starting shaft position in ticks
float v_tick = 0;
float v_rpm = 0;
float v_filt = 0;
float v_filt_prev = 0;
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
    // Compute velocity
    v_tick = (pos - pos_prev) / (DELTA_T_MS / 1000.0);
    pos_prev = pos;

    // Convert velocity from tick/sec to rpm
    v_rpm = v_tick / 600.0*60.0;

    // Low-pass filter (25 Hz cutoff frequency (w0))
    v_filt = (0.854 * v_filt) + (0.0728 * v_rpm) + (0.0728 * v_filt_prev);
    v_filt_prev = v_rpm;

    // error
    //int e = target - v_rpm;
    int e = target - v_filt;

    // derivative
    float dedt = (e - eprev) / (DELTA_T_MS / 1000.0);

    // integral
    eintegral = eintegral + e * (DELTA_T_MS / 1000.0);

    // control signal
    float u = (KP * e) + (KD * dedt) + (KI * eintegral);

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