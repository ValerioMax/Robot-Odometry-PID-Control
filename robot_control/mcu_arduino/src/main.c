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

int main() {
    // initialize UART and printf wrapper
    UART_init(19200);
    printf_init();

    // initialize timer that counts to get time with micros()
    timer_internal_init();

    Robot robot;

    Encoder_init(&encoder1, PB0, PB2, PORT_B);
    Encoder_init(&encoder2, PB1, PB3, PORT_B);
    Motor_init(&motor1, PA1, PA3, PH3, &encoder1);
    Motor_init(&motor2, PA0, PA2, PH4, &encoder2);

    Robot_init(&robot, &motor1, &motor2, 15.0, 28.9); // NOTE: measured empirically distance between wheels is 30.0cm and wheel circumpherence 28.9cm

    uint64_t prev_log_time = 0;
    uint64_t prev_loop_time = 0;

    while (1) {
        uint64_t time_passed_us = micros() - prev_loop_time;
        prev_loop_time = micros();

        if (robot.wasd_control)
            Robot_get_wasd(&robot);
        else
            Robot_get_commands(&robot);

        Encoder_update_rpm(&encoder1, time_passed_us);
        Encoder_update_rpm(&encoder2, time_passed_us);

        // if (robot.pos_control) {
        //     Motor_PID_position(&motor1, time_passed_us);
        //     Motor_PID_position(&motor2, time_passed_us);
        // }
        // else if (robot.rpm_control) {
        //     Motor_PID_speed(&motor1, time_passed_us);
        //     Motor_PID_speed(&motor2, time_passed_us);
        // }

        Robot_update_odometry(&robot);

        // every DELTA_T_LOG_US data gets logged on UART
        if (micros() > prev_log_time + DELTA_T_LOG_US + 9000) {
            prev_log_time = micros();
            
            // printf("%ld %ld, %ld %ld %ld\n", 
            //     motor1.encoder->pos,
            //     motor2.encoder->pos,
            //     (long) (robot.x * 1000),
            //     (long) (robot.y * 1000),
            //     (long)(robot.theta*1000)
            // );

            //motor 1 everything
            printf("%ld %ld %ld %d %d %d, %u,,, %ld %ld %ld\n", 
                motor1.encoder->pos,
                motor1.target_pos,
                motor1.err_pos,
                motor1.encoder->rpm,
                motor1.target_rpm,
                motor1.err_rpm,
                OCR4A,

                (long) (robot.x * 1000),
                (long) (robot.y * 1000),
                (long) (robot.theta * 1000)
            );

            // printf("%d %d %d\n", 
            //     motor1.encoder->pos,
            //     motor1.target_pos,
            //     motor1.err_pos);

            // printf("%d %d %d\n", 
            //     motor1.encoder->rpm,
            //     motor1.target_rpm,
            //     motor1.err_rpm);
        }
    }
}