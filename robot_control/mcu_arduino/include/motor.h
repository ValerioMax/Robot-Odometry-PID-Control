//#ifndef MOTOR_H
//#define MOTOR_H

#include "shared_types.h"
#include "peripherals_utils.h"
#include "encoder.h"

// Default PID parameters
#define DEFAULT_KP 300
#define DEFAULT_KI 0
#define DEFAULT_KD 0

// top value for N tick resolution
#define MAX_PWM_TICKS 65535 // 8bit: 255, 9bit: 511, 10bit: 1023, 16bit: 65535

// e_integral clamping values to prevent enourmous error accumulation
#define E_INTEGRAL_MAX 9999.0
#define E_INTEGRAL_MIN -9999.0

typedef struct {
    // pins
    int in1_pin;
    int in2_pin;
    int pwm_pin;

    // flags
    int attached;
    int manual_control;

    // values
    int target_pos;
    int target_rpm;
    int err_pos;
    int err_rpm;

    // PID parameters
    int kp;
    int ki;
    int kd;

    Encoder *encoder;
} Motor;

void Motor_init(Motor *motor, int in1, int in2, int pwm, Encoder *encoder);
void Motor_attach(Motor *motor, int state);
void Motor_set_speed(Motor *motor, int dir, int duty_cycle);
void Motor_PID_params(Motor *motor, int kp, int ki, int kd);
void Motor_PID_position(Motor *motor);

//#endif