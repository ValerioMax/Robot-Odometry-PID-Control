#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "shared_types.h"

// tempo di campionamento 20ms per avere freq campionamento 50Hz
#define DELTA_T_MS 20

// tempo di logging su seriale 2000ms
#define DELTA_T_LOG_MS 2000

// PID parameters
#define KP 1.0
#define KD 0.0 //0.025
#define KI 0.005

void encoder_PCINT0_init();
void read_encoder1();
void read_encoder2();
void set_motor(int in1, int in2, int dir, int u_pwm);
void PID1(int target);