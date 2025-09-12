#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "uart_utils.h"
#include "peripherals_utils.h"
#include "motor_utils.h"

// FORSE VA IMPLEMENTATA LA FUNZIONE micros(), COSI' COM'E' FORSE
// PUO DARE PROBLEMI (AL MOMENTO E' CON UN INTERRUPT INTERNO OGNI 20ms PER IL TASK)

// from motor_utils.c
extern int pos; // starting shaft position in ticks
extern int dir; // motor direction 1: CW, -1: CCW, 0: still
extern float eprev;
extern float eintegral;

// from peripheral_utils.c
extern volatile uint8_t internal_int_sample_occured;
extern uint16_t internal_int_sample_count;
extern volatile uint8_t internal_int_log_occured;
extern uint16_t internal_int_log_count;
extern const uint8_t enc_pin_mask;
extern volatile uint8_t external_int_occurred;
extern uint16_t external_int_count;

extern volatile uint8_t internal_int_occured;
extern uint64_t internal_int_count;

int main() {
    // initialize UART and printf wrapper
    UART_init(19200);
    printf_init();

    timer_internal_init();

    // initialize timer 1 for tempo di campionamento at 50Hz per fare il task
    //timer_internal_sample_init();

    // initialize timer 2 for logging on Serial at 0.5Hz
    //timer_internal_log_init();

    // initialize timer 0 for pwm
    timer_pwm_init();

    // initialize external interrupts for encoder
    external_int_init();

    int target = 1000;

    while (1) {
        if (external_int_occurred) {
            external_int_occurred = 0; // reset flag
            read_encoder(); // do task
            //printf("ex %d, in %d, pos %d\n", external_int_count, internal_int_count, pos);
            //printf("%d %d\n", external_int_count, pos);
        }
        if (internal_int_sample_occured) {
            internal_int_sample_occured = 0; // reset flag
            PID(target); // do task
            //printf("int1\n");
        }
        if (internal_int_log_occured) {
            internal_int_log_occured = 0; // reset flag
            //printf("int2\n");
            printf("ext %d, trg %d, pos %d, err %d, dtc %u, dir %d\n", external_int_count, target, pos, pos-target, OCR0A, dir);
        }
        if (internal_int_occured) {
            internal_int_occured = 0;
            printf("%ld\n", internal_int_count);
        }
        
        // blocca l'esecuzione a tempo indeterminato, si sblocca con un nuovo interrupt
        sleep_cpu();
    }
}