#include "motor.h"

Motor motor1 = {0};
Motor motor2 = {0};

void Motor_init(Motor *motor, int in1_pin, int in2_pin, int pwm_pin, Encoder *encoder) {
    motor->in1_pin = in1_pin;
    motor->in2_pin = in2_pin;
    motor->pwm_pin = pwm_pin;
    motor->target_pos = 0;
    motor->target_rpm = 0;
    motor->err_pos = 0;
    motor->err_rpm = 0;

    motor->e_pos_integral = 0;
    motor->e_rpm_integral = 0;

    motor->rpm_prev = 0;
    motor->rpm_filt = 0;
    
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

void Motor_attach(Motor *motor, int state) {
    motor->attached = state;
}

void Motor_set_pwm(Motor *motor, int dir, uint16_t duty_cycle) {
    if (!(motor->attached))
        return ;

    const uint8_t in1_pin_mask = (1 << motor->in1_pin);
    const uint8_t in2_pin_mask = (1 << motor->in2_pin);

    // setting dutycycle (and so voltage intensity)
    switch (motor->pwm_pin) {
        case PH3:
            OCR4A = duty_cycle;
            break;
        case PH4:
            OCR4B = duty_cycle;
            break;
        case PH5:
            OCR4C = duty_cycle;
            break;
    }

    // setting direction
    if (dir == 1) {
        PORTA |= in1_pin_mask;  // in1 HIGH
        PORTA &= ~in2_pin_mask; // in2 LOW
    }
    else if (dir == -1) {
        PORTA &= ~in1_pin_mask; // in1 LOW
        PORTA |= in2_pin_mask;  // in2 HIGH
    }
    else {
        PORTA &= ~in1_pin_mask; // in1 LOW
        PORTA &= ~in2_pin_mask; // in2 LOW
    }
}

void Motor_PID_params(Motor *motor, int32_t kp, int32_t ki, int32_t kd) {
    motor->kp = kp;
    motor->ki = ki;
    motor->kd = kd;
}

void Motor_PID_position(Motor *motor, uint64_t time_passed_us) {
    if (motor->manual_control)
        return ;

    // get motor state values
    int32_t target_pos = motor->target_pos;
    int32_t pos = motor->encoder->pos;
    int32_t e_pos_prev = motor->err_pos;
    int32_t e_pos_integral = motor->e_pos_integral;

    // get motor PID parameters
    int32_t kp = motor->kp;
    int32_t ki = motor->ki; 
    int32_t kd = motor->kd;

    // error
    int32_t e = target_pos - pos;

    // derivative
    float de_dt = (e - e_pos_prev) / (time_passed_us / 1000000.0);

    // integral
    e_pos_integral = e_pos_integral + e * (time_passed_us / 1000000.0);

    // e_integral clamping
    if (e_pos_integral > E_INTEGRAL_MAX) e_pos_integral = E_INTEGRAL_MAX;
    else if (e_pos_integral < E_INTEGRAL_MIN) e_pos_integral = E_INTEGRAL_MIN;

    // control signal
    float u = (kp * e) + (ki * e_pos_integral) + (kd * de_dt);

    // motor speed signal
    float u_pwm = fabs(u);

    // cap it to max value
    if(u_pwm > MAX_PWM_TICKS)
        u_pwm = MAX_PWM_TICKS;

    // motor direction
    int dir = 1;
    if (u < 0)
        dir = -1;

    // actuate command
    Motor_set_pwm(motor, dir, (uint16_t) u_pwm);

    // update motor position error
    motor->err_pos = e;
    motor->e_pos_integral = e_pos_integral;
}

void Motor_PID_speed(Motor *motor, uint64_t time_passed_us) {
    if (motor->manual_control)
        return ;

    // get motor state values
    int target_rpm = motor->target_rpm;
    int rpm = motor->encoder->rpm;
    int e_rpm_prev = motor->err_rpm;
    int e_rpm_integral = motor->e_rpm_integral;
    int rpm_filt = motor->rpm_filt;
    int rpm_prev = motor->rpm_prev;

    // low-pass filter (25 Hz cutoff frequency (w0))
    rpm_filt = (0.854 * rpm_filt) + (0.0728 * rpm) + (0.0728 * rpm_prev);
    rpm_prev = rpm;

    // get motor PID parameters
    int32_t kp = motor->kp;
    int32_t ki = motor->ki; 
    int32_t kd = motor->kd;

    // error
    int e = target_rpm - rpm_filt; // UNCOMMENT when using Low Pass filter
    //int e = target_rpm - rpm;

    // derivative
    float de_dt = (e - e_rpm_prev) / (time_passed_us / 1000000.0);

    // integral
    e_rpm_integral = e_rpm_integral + e * (time_passed_us / 1000000.0);

    // e_integral clamping
    if (e_rpm_integral > E_INTEGRAL_MAX) e_rpm_integral = E_INTEGRAL_MAX;
    else if (e_rpm_integral < E_INTEGRAL_MIN) e_rpm_integral = E_INTEGRAL_MIN;

    // control signal
    float u = (kp * e) + (ki * e_rpm_integral) + (kd * de_dt);

    // motor speed signal
    float u_pwm = fabs(u);

    // cap it to max value
    if(u_pwm > MAX_PWM_TICKS)
        u_pwm = MAX_PWM_TICKS;

    // motor direction
    int dir = 1;
    if (u < 0)
        dir = -1;

    // actuate command
    Motor_set_pwm(motor, dir, (uint16_t) u_pwm);

    // update motor position error
    motor->err_rpm = e;
    motor->e_rpm_integral = e_rpm_integral;    

    motor->rpm_filt = rpm_filt;
    motor->rpm_prev = rpm_prev;
}