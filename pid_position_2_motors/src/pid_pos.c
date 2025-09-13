#include "uart_utils.h"
#include "peripherals_utils.h"

#include "robot.h"

// from encoder.c
extern Encoder encoder1;
extern Encoder encoder2;

extern volatile uint8_t external_int_enc1_occurred;
extern volatile uint8_t external_int_enc2_occurred;

// from motor.c
extern Motor motor1;
extern Motor motor2;

// from peripheral_utils.c
extern volatile uint8_t internal_int_occured;
extern uint64_t internal_int_count;

int main() {
    // initialize UART and printf wrapper
    UART_init(19200);
    printf_init();

    // initialize timer that counts to get time with millis()
    timer_internal_init();

    Robot robot;

    Encoder_init(&encoder1, PB0, PB2, PORT_B);
    Encoder_init(&encoder2, PB1, PB3, PORT_B);
    Motor_init(&motor1, PA1, PA3, PH3, &encoder1);
    Motor_init(&motor2, PA0, PA2, PH4, &encoder2);

    Robot_init(&robot, &motor1, &motor2);

    uint64_t prev_sample_time = 0;
    uint64_t prev_log_time = 0;

    while (1) {
        Robot_get_commands(&robot);

        // when encoder signals external interrupt is triggered
        if (external_int_enc1_occurred) {
            external_int_enc1_occurred = 0;
            Encoder_read(&encoder1); // read encoder motor 1
        }
        if (external_int_enc2_occurred) {
            external_int_enc2_occurred = 0;
            Encoder_read(&encoder2); // read encoder motor 2
        }
        // every DELTA_T_MS = 20ms (at 50Hz) command gets generated and actuated
        if (millis() > prev_sample_time + DELTA_T_MS) {
            Encoder_update_rpm(&encoder1, DELTA_T_MS);
            //Encoder_update_rpm(&encoder2, DELTA_T_MS);
            Motor_PID_position(&motor1);
            //Motor_PID_position(&motor2);
            prev_sample_time = millis();
        }
        // every DELTA_T_LOG_MS data gets logged on UART
        if (millis() > prev_log_time + DELTA_T_LOG_MS) {
            printf("trg %d, pos %d, err %d, u_pwm %u, dir %d, rpm %d\n", motor1.target_pos, motor1.encoder->pos, motor1.target_pos - motor1.encoder->pos, OCR4A, motor1.encoder->dir, motor1.encoder->rpm);
            prev_log_time = millis();
        }




        // attempt to fill buffer all at once and then send one byte at a time to not create a bottleneck with serial (doesnt work properly for now)
        /*
        // every DELTA_T_LOG_MS fill data logging buffer
        if (millis() > prev_fill_buf_time + DELTA_T_FILL_BUF_MS) {
            sprintf(log_buf, "trg %d, pos %d, err %d, dtc %u, dir %d\n",
                                target_pos, motor1.encoder->pos, target_pos - motor1.encoder->pos, OCR4A, motor1.encoder->dir);
            prev_fill_buf_time = millis();
        }
        // every DELTA_T_LOG_BYTE_MS log on UART a byte at a timeù
        if (millis() > prev_log_time + DELTA_T_LOG_MS) {
            if (*log_buf_pt) {
                UART_putchar(*log_buf_pt);
                log_buf_pt++;
            }   
            prev_log_time = millis();
        }
        */
    }
}