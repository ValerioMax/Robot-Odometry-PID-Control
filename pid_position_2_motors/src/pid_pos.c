#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "uart_utils.h"
#include "peripherals_utils.h"
#include "motor_utils.h"

// from motor_utils.c
extern int pos1; // starting shaft position in ticks
extern int dir1; // motor direction 1: CW, -1: CCW, 0: still
extern float eprev;
extern float eintegral;
extern volatile uint8_t external_int_enc1_occurred;
extern volatile uint8_t external_int_enc2_occurred;

// from peripheral_utils.c
extern volatile uint8_t internal_int_occured;
extern uint64_t internal_int_count;
extern const uint8_t enc_pin_mask;

int main() {
    // initialize UART and printf wrapper
    UART_init(19200);
    printf_init();

    // initialize timer that counts to get time with millis()
    timer_internal_init();

    // initialize timer 0 for pwm on port D pin 5 and 6
    timer_pwm_PH_init(PH3);
    timer_pwm_PH_init(PH4);

    // initialize external interrupts for encoder
    encoder_PCINT0_init();

    int target = 1000;

    uint64_t prev_sample_time = 0;
    uint64_t prev_log_time = 0;

    while (1) {
        // when encoder signals external interrupt is triggered
        if (external_int_enc1_occurred) {
            external_int_enc1_occurred = 0;
            read_encoder1(); // read encoder motor 1
        }
        if (external_int_enc2_occurred) {
            external_int_enc2_occurred = 0;
            read_encoder2(); // read encoder motor 2
        }
        // every DELTA_T_MS = 20ms (at 50Hz) command gets generated and actuated
        if (millis() > prev_sample_time + DELTA_T_MS) {
            PID1(target);
            prev_sample_time = millis();
        }
        // every DELTA_T_LOG_MS data gets logged on UART
        if (millis() > prev_log_time + DELTA_T_LOG_MS) {
            printf("trg %d, pos %d, err %d, dtc %u, dir %d\n", target, pos1, target-pos1, OCR4A, dir1);
            prev_log_time = millis();
        }
    }
}