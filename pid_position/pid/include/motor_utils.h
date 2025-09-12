#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "shared_types.h"

// PID parameters
#define KP 1.0
#define KD 0.0 //0.025
#define KI 0.1

void read_encoder();
void set_motor(int dir, int u_pwm);
void PID(int target);