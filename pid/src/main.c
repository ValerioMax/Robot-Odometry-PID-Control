#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "uart_utils.h"
#include "peripherals_utils.h"
#include "motor_utils.h"

// from motor_utils.c
extern int pos_prev;
extern int pos; // starting shaft position in ticks
extern float v_tick;
extern float v_rpm;
extern float v_filt;
extern float v_filt_prev;
extern int dir; // motor direction 1: CW, -1: CCW, 0: still
extern float eprev;
extern float eintegral;

// from peripheral_utils.c
extern volatile uint8_t internal_int_occured;
extern uint64_t internal_int_count;
extern const uint8_t enc_pin_mask;
extern volatile uint8_t external_int_occurred;
extern uint16_t external_int_count;

int main() {
    // initialize UART and printf wrapper
    UART_init(19200);
    printf_init();

    // initialize timer that counts to get time with millis()
    timer_internal_init();

    // initialize timer 0 for pwm
    timer_pwm_init();

    // initialize external interrupts for encoder
    external_int_init();

    int target_speed_1 = 100;
    int target_speed_2 = 200;

    uint64_t prev_sample_time = 0;
    uint64_t prev_log_time = 0;

    while (1) {
        // when encoder signals external interrupt is triggered
        if (external_int_occurred) {
            external_int_occurred = 0; // reset flag
            read_encoder(); // do task
        }
        // every DELTA_T_MS = 20ms (at 50Hz) command gets generated and actuated
        if (millis() > prev_sample_time + DELTA_T_MS) {
            // do task
            PID(target_speed_1);
            PID(target_speed_2);
            prev_sample_time = millis();
        }
        // every DELTA_T_LOG_MS data gets logged on UART
        if (millis() > prev_log_time + DELTA_T_LOG_MS) {
            printf("m1 %d %d, m2 %d %d\n", target_speed_1, (int) v_filt);
            prev_log_time = millis();
        }
    }
}