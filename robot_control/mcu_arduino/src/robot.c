#include "robot.h"

void Robot_init(Robot *robot, Motor *motor_left, Motor *motor_right) {
    robot->x = 0;
    robot->y = 0;
    robot->vx = 0;
    robot->vy = 0;
    robot->wasd_control = 0;
    robot->motor_left = motor_left;
    robot->motor_right = motor_right;
}

void Robot_get_commands(Robot *robot) {
    // non blocking read: returns arrived string, if string didnt arrive or didnt arrive entirely returns NULL immediately
    
    char *command_pt = (char*) UART_getstring_non_blocking();

    if (!command_pt)
        return ;
    
    char command[MAX_BUF_SIZE];
    memcpy(command, command_pt, MAX_BUF_SIZE);
    command[MAX_BUF_SIZE - 1] = '\0';

    //printf("IN%sFIN\n", command); // DEBUG
    
    // remove newline (maybe i should do it directly in UART_getstring_non_blocking)
    command[strcspn(command, "\n\r")] = '\0';
    
    printf("IN%sFIN\n", command); // DEBUG

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

    // COMMAND INTERFACE:
    // [OPEN LOOP]
    // pwm <dir_motor1_left> <duty_cycle%_motor_left> <dir_motor_right> <duty_cycle%_motor_right>
    if (!strcmp(cmd_args[0], PWM_COMMAND) && arg_count == PWM_COMMAND_ARGS) {
        //printf("PWM\n");
        robot->motor_left->manual_control = 1;
        robot->motor_right->manual_control = 1;
        Motor_set_pwm(robot->motor_left, atoi(cmd_args[1]), (uint16_t) atoi(cmd_args[2]));
        Motor_set_pwm(robot->motor_right, atoi(cmd_args[3]), (uint16_t) atoi(cmd_args[4]));
    }
    // [CLOSED LOOP]
    // pos	<pos_motor_1> <pos_motor_2> 
    else if (!strcmp(cmd_args[0], POS_COMMAND) && arg_count == POS_COMMAND_ARGS) {
        //printf("POS\n");
        robot->motor_left->manual_control = 0;
        robot->motor_right->manual_control = 0;
        robot->motor_left->target_pos = atol(cmd_args[1]);
        robot->motor_right->target_pos = atol(cmd_args[2]);
    }
    // [CLOSED LOOP]
    // rpm	<rpm_motor_1> <rpm_motor_2> 
    else if (!strcmp(cmd_args[0], RPM_COMMAND) && arg_count == RPM_COMMAND_ARGS) {
        robot->motor_left->manual_control = 0;
        robot->motor_right->manual_control = 0;
        robot->motor_left->target_rpm = atoi(cmd_args[1]);
        robot->motor_right->target_rpm = atoi(cmd_args[2]);
    }
    // setmotor <motor1> <motor2>
    else if (!strcmp(cmd_args[0], SETMOTOR_COMMAND) && arg_count == SETMOTOR_COMMAND_ARGS) {
        Motor_attach(robot->motor_left, atoi(cmd_args[1]));
        Motor_attach(robot->motor_right, atoi(cmd_args[2]));
    }
    // gain <kp> <ki> <kd>
    else if (!strcmp(cmd_args[0], GAIN_COMMAND) && arg_count == GAIN_COMMAND_ARGS) {
        Motor_PID_params(robot->motor_left, atol(cmd_args[1]), atol(cmd_args[2]), atol(cmd_args[3]));
        Motor_PID_params(robot->motor_right, atol(cmd_args[1]), atol(cmd_args[2]), atol(cmd_args[3]));
    }
    // setmanual <1/0>
    else if (!strcmp(cmd_args[0], SETMANUAL_COMMAND) && arg_count == SETMANUAL_COMMAND_ARGS) {
        robot->motor_left->manual_control = atoi(cmd_args[1]);
        robot->motor_right->manual_control = atoi(cmd_args[1]);
    }
    // setwasd <1/0>
    else if (!strcmp(cmd_args[0], SETWASD_COMMAND) && arg_count == SETWASD_COMMAND_ARGS) {
        robot->wasd_control = atoi(cmd_args[1]);
    }
}

// direct WASD control in case manual control is active
void Robot_get_wasd(Robot *robot) {
    char c = UART_getchar_non_blocking();

    if (!c)
        return ;

    int32_t step = 50;
    
    if (c == 'w') {
        robot->motor_left->target_pos += step;
        robot->motor_right->target_pos += step;
    }
    else if (c == 'a') {
        robot->motor_left->target_pos += 0;
        robot->motor_right->target_pos += step;
    }
    else if (c == 's') {
        robot->motor_left->target_pos -= step;
        robot->motor_right->target_pos -= step;
    }
    else if (c == 'd') {
        robot->motor_left->target_pos += step;
        robot->motor_right->target_pos += 0;
    }
    else if (c == 'q') {
        robot->wasd_control = 0;
    }
}