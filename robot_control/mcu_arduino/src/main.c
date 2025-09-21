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
        if (robot.wasd_control)
            Robot_get_wasd(&robot);
        else
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
            Encoder_update_rpm(&encoder2, DELTA_T_MS);

            Motor_PID_position(&motor1);
            Motor_PID_position(&motor2);

            // Motor_PID_speed(&motor1);
            // Motor_PID_speed(&motor2);

            prev_sample_time = millis();
        }
        // every DELTA_T_LOG_MS data gets logged on UART
        if (millis() > prev_log_time + DELTA_T_LOG_MS) {

            //motor 1 everything
            printf("%ld %ld %ld %d %d %d, %u\n", 
                motor1.encoder->pos,
                motor1.target_pos,
                motor1.err_pos,
                motor1.encoder->rpm,
                motor1.target_rpm,
                motor1.err_rpm,
                OCR4A
            );

            // printf("%d %d %d\n", 
            //     motor1.encoder->pos,
            //     motor1.target_pos,
            //     motor1.err_pos);

            // printf("%d %d %d\n", 
            //     motor1.encoder->rpm,
            //     motor1.target_rpm,
            //     motor1.err_rpm);
            
            prev_log_time = millis();
        }
    }
}