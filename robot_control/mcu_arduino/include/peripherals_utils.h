#ifndef PERIPHERALS_UTILS_H
#define PERIPHERALS_UTILS_H

#include "shared_types.h"

// INTERNAL INTERRUPT
void        timer_internal_init();
uint64_t    millis();

// EXTERNAL INTERRUPT
void        external_int_PCINT_init(const uint8_t pin_mask, int port);

// PWM
void        pwm_TIMER4_init(int pin);

#endif