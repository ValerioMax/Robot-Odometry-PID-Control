#include "motor.h"

Motor motor1 = {0};
Motor motor2 = {0};

float e_prev = 0;
float e_integral = 0;

void Motor_init(Motor *motor, int in1_pin, int in2_pin, int pwm_pin, Encoder *encoder) {
    motor->in1_pin = in1_pin;
    motor->in2_pin = in2_pin;
    motor->pwm_pin = pwm_pin;
    motor->target_pos = 0;
    motor->target_rpm = 0;
    motor->err_pos = 0;
    motor->err_rpm = 0;
    motor->kp = DEFAULT_KP;
    motor->ki = DEFAULT_KI;
    motor->kd = DEFAULT_KD;

    motor->encoder = encoder;

    // set in1_pin and in2_pin as output
    const uint8_t in_pins_mask = (1 << in1_pin) | (1 << in2_pin);
    DDRA |= in_pins_mask; // little hardcoded

    // initialize pwm on pwm_pin
    pwm_TIMER4_init(pwm_pin);

    motor->attached = 1;
    motor->manual_control = 0;
}

// state 1 attach motor, state 2 detach motor
void Motor_attach(Motor *motor, int state) {
    motor->attached = state;
}

void Motor_set_speed(Motor *motor, int dir, int duty_cycle) {
    if (!(motor->attached))
        return ;

    const uint8_t in1_pin_mask = (1 << motor->in1_pin);
    const uint8_t in2_pin_mask = (1 << motor->in2_pin);

    // setting dutycycle (and so voltage intensity)
    switch (motor->pwm_pin) {
        case PH3:
            OCR4A = (uint16_t) duty_cycle;
            break;
        case PH4:
            OCR4B = (uint16_t) duty_cycle;
            break;
        case PH5:
            OCR4C = (uint16_t) duty_cycle;
            break;
    }

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

void Motor_PID_params(Motor *motor, int kp, int ki, int kd) {
    motor->kp = kp;
    motor->ki = ki;
    motor->kd = kd;
}

void Motor_PID_position(Motor *motor) {
    if (motor->manual_control)
        return ;

    // get motor state values
    int target_pos = motor->target_pos;
    int pos = motor->encoder->pos;

    // get motor PID parameters (we scale it by 100)
    float kp = motor->kp;
    float ki = motor->ki; 
    float kd = motor->kd;

    // error
    int e = target_pos - pos;

    // derivative
    float de_dt = (e - e_prev) / (DELTA_T_MS / 1000.0);

    // integral
    e_integral = e_integral + e * (DELTA_T_MS / 1000.0);

    // e_integral clamping
    if (e_integral > E_INTEGRAL_MAX) e_integral = E_INTEGRAL_MAX;
    else if (e_integral < E_INTEGRAL_MIN) e_integral = E_INTEGRAL_MIN;

    // control signal
    float u = (kp * e) + (ki * e_integral) + (kd * de_dt);

    // motor speed signal 
    // (it has to be a pwm duty cycle so it has to be positive and between 0 and 255 to prevent overflow)
    float u_pwm = fabs(u);

    // cap it to max value
    if(u_pwm > MAX_PWM_TICKS)
        u_pwm = MAX_PWM_TICKS;

    // motor direction
    int dir = 1; // motor direction 1: CW, -1: CCW, 0: still
    if (u < 0)
        dir = -1;

    // actuate command
    Motor_set_speed(motor, dir, u_pwm);

    // update motor position error
    motor->err_pos = e;

    // store previous error
    e_prev = e;
}