//#ifndef MOTOR_H
//#define MOTOR_H

#include "shared_types.h"
#include "peripherals_utils.h"
#include "encoder.h"

// tempo di campionamento 20ms per avere freq campionamento 50Hz
#define DELTA_T_MS 20

// PID parameters
#define KP 3.0
#define KD 0.0 //0.025
#define KI 0.5

// e_integral clamping to prevent enourmous accumulation
#define E_INTEGRAL_MAX 9999.0
#define E_INTEGRAL_MIN -9999.0

typedef struct {
    int in1_pin;
    int in2_pin;
    int pwm_pin;

    Encoder *encoder;
} Motor;

void Motor_init(Motor *motor, int in1, int in2, int pwm, Encoder *encoder);
void Motor_set_speed(Motor *motor, int dir, int duty_cycle);
void Motor_PID_position(Motor *motor, int target_pos);

//#endif