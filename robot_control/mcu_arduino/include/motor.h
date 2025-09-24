//#ifndef MOTOR_H
//#define MOTOR_H

#include "shared_types.h"
#include "peripherals_utils.h"
#include "encoder.h"

// Default PID parameters
#define DEFAULT_KP 300
#define DEFAULT_KI 0
#define DEFAULT_KD 15

// top value for N tick resolution
#define MAX_PWM_TICKS 65535 // 8bit: 255, 9bit: 511, 10bit: 1023, 16bit: 65535

// e_integral clamping values to prevent enourmous error accumulation
#define E_INTEGRAL_MAX 16000.0
#define E_INTEGRAL_MIN -16000.0

typedef struct {
    // pins
    int in1_pin;
    int in2_pin;
    int pwm_pin;

    // flags
    int attached;
    int manual_control;

    // state values
    int32_t target_pos;
    int target_rpm;
    int32_t err_pos;
    int err_rpm;

    // state integral values for PID
    int32_t e_pos_integral;
    int e_rpm_integral;

    // state values to compute low pass filter
    int rpm_prev;
    int rpm_filt;

    Encoder *encoder;

    // PID parameters
    int32_t kp;
    int32_t ki;
    int32_t kd;
} Motor;

void Motor_init(Motor *motor, int in1, int in2, int pwm, Encoder *encoder);
void Motor_attach(Motor *motor, int state);
void Motor_set_pwm(Motor *motor, int dir, uint16_t duty_cycle);
void Motor_PID_params(Motor *motor, int32_t kp, int32_t ki, int32_t kd);
void Motor_PID_position(Motor *motor, uint64_t time_passed_us);
void Motor_PID_speed(Motor *motor, uint64_t time_passed_us);

//#endif