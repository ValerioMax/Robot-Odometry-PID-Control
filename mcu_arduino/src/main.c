#include "uart_utils.h"
#include "peripherals_utils.h"

#include "robot.h"

// from encoder.c
extern Encoder encoder1;
extern Encoder encoder2;

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

    // nominal (measured empirically): b = 30.0cm, kr = kl = 0.013136 [ d_pl = (d_tick_l * robot->wheel_circ) / TICKS_PER_REV; ]
    Robot_init(&robot, &motor1, &motor2, 31.87, 0.014792, 0.014792);

    uint64_t prev_control_time = 0;
    uint64_t prev_log_time = 0;
    //uint64_t prev_loop_time = 0;

    while (1) {
        //uint64_t time_passed_us = micros() - prev_loop_time;
        //prev_loop_time = micros();

        if (robot.wasd_control)
            Robot_get_wasd(&robot);
        else
            Robot_get_commands(&robot);

        if (micros() > prev_control_time + DELTA_T_US) {
            prev_control_time = micros();

            Robot_update_odometry(&robot);

            Encoder_update_rpm(&encoder1, DELTA_T_US);
            Encoder_update_rpm(&encoder2, DELTA_T_US);

            // robot autonomous position control task
            if (robot.auto_control)
                Robot_goto_position(&robot);

            // Robot_goto_position() task use one of these two tasks for actuation 
            if (robot.pos_control) {
                Motor_PID_position(&motor1, DELTA_T_US);
                Motor_PID_position(&motor2, DELTA_T_US);
            }
            else if (robot.rpm_control) {
                Motor_PID_speed(&motor1, DELTA_T_US);
                Motor_PID_speed(&motor2, DELTA_T_US);
            }
        }

        // every DELTA_T_LOG_US data gets logged on UART
        if (micros() > prev_log_time + DELTA_T_LOG_US) {
            prev_log_time = micros();
            
            // UART slows down a lot so i split log info in different cases
            if (robot.log_odometry)
                printf("%d %d %d\n",
                    (int) (robot.x), // in cm
                    (int) (robot.y), // in cm
                    (int) (robot.theta * 57.324) // in degrees (180 / 3.14 = 57.324)
                );
            else if (robot.log_rpm_motor_left)
                printf("R%d r%d\n",
                    robot.motor_left->target_rpm,
                    robot.motor_left->rpm_filt
                );
            else if (robot.log_rpm_motor_right)
                printf("R%d r%d\n",
                    robot.motor_right->target_rpm,
                    robot.motor_right->rpm_filt
                );
            else if (robot.log_pos_motor_left)
                printf("P%ld p%ld\n",
                    robot.motor_left->target_pos,
                    robot.motor_left->encoder->pos
                );
            else if (robot.log_pos_motor_right)
                printf("P%ld p%ld\n",
                    robot.motor_right->target_pos,
                    robot.motor_right->encoder->pos
                );

            // // robot motor left logging
            // else if (robot.log_motor_left) {
            //     printf("%ld %ld %ld %d %d %d %u\n",
            //         robot.motor_left->encoder->pos,
            //         robot.motor_left->target_pos,
            //         robot.motor_left->err_pos,
            //         robot.motor_left->encoder->rpm,
            //         robot.motor_left->target_rpm,
            //         robot.motor_left->err_rpm,
            //         OCR4A
            //     );
            // }
            // // robot motor right logging
            // else if (robot.log_motor_right) {
            //     printf("%ld %ld %ld %d %d %d %u\n",
            //         robot.motor_right->encoder->pos,
            //         robot.motor_right->target_pos,
            //         robot.motor_right->err_pos,
            //         robot.motor_right->encoder->rpm,
            //         robot.motor_right->target_rpm,
            //         robot.motor_right->err_rpm,
            //         OCR4B
            //     );
            // }

            // motor 1 & 2 logging
            // printf("%ld %d %ld %d\n", 
            //     motor1.encoder->pos,
            //     motor1.encoder->rpm,
            //     motor2.encoder->pos,
            //     motor2.encoder->rpm
            // );
        }
    }
}