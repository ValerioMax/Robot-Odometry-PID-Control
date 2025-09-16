#include "robot.h"

void Robot_init(Robot *robot, Motor *motor_left, Motor *motor_right) {
    robot->x = 0;
    robot->y = 0;
    robot->vx = 0;
    robot->vy = 0;
    robot->motor_left = motor_left;
    robot->motor_right = motor_right;
}

void Robot_get_commands(Robot *robot) {
    // non blocking read: returns arrived string, if string didnt arrive or didnt arrive entirely returns NULL immediately
    char *command = (char*) UART_getstring_int();

    if (!command)
        return ;

    // parse command
    char *cmd_args[MAX_COMMAND_ARGS];
    int arg_count = 0;

    char *token = strtok(command, " ");

    while (token && arg_count < MAX_COMMAND_ARGS) {
        //printf("%s\n", token);
        cmd_args[arg_count++] = token;
        token = strtok(NULL, " ");
    }

    if (arg_count == 0)
        return ;

    // [OPEN LOOP]
    // pwm <dir_motor1_left> <duty_cycle%_motor_left> <dir_motor_right> <duty_cycle%_motor_right>
    if (!strcmp(cmd_args[0], PWM_COMMAND) && arg_count == 5) {
        printf("PWM\n");
        robot->motor_left->manual_control = 1;
        robot->motor_right->manual_control = 1;
        Motor_set_speed(robot->motor_left, atoi(cmd_args[1]), atoi(cmd_args[2]));
        Motor_set_speed(robot->motor_right, atoi(cmd_args[3]), atoi(cmd_args[4]));
    }
    // [CLOSED LOOP]
    // pos	<pos_motor_1> <pos_motor_2> 
    else if (!strcmp(cmd_args[0], POS_COMMAND) && arg_count == 3) {
        printf("POS\n");
        robot->motor_left->manual_control = 0;
        robot->motor_right->manual_control = 0;
        robot->motor_left->target_pos = atoi(cmd_args[1]);
        robot->motor_right->target_pos = atoi(cmd_args[2]);
    }
    // [CLOSED LOOP]
    // rpm	<rpm_motor_1> <rpm_motor_2> 
    else if (!strcmp(cmd_args[0], RPM_COMMAND) && arg_count == 3) {
        robot->motor_left->manual_control = 0;
        robot->motor_right->manual_control = 0;
        // TODO
        //Motor_PID_speed()
    }
    // setmotor <motor1> <motor2>
    else if (!strcmp(cmd_args[0], SETMOTOR_COMMAND) && arg_count == 3) {
        Motor_attach(robot->motor_left, atoi(cmd_args[1]));
        Motor_attach(robot->motor_right, atoi(cmd_args[2]));
    }
}