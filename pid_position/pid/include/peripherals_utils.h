#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "shared_types.h"

// tempo di logging su seriale 2000ms
#define DELTA_T_LOG_MS 2000

// INTERNAL INTERRUPT
void timer_internal_sample_init();
void timer_internal_log_init();
void timer_internal_init();

// EXTERNAL INTERRUPT;
void external_int_init();

// PWM
#define TCCRA_MASK (1 << COM0A1) | (1 << WGM01) | (1 << WGM00)
#define TCCRB_MASK (1 << CS01) | (1 << CS00) // Set prescaler to 64

void timer_pwm_init();