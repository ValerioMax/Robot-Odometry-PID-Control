#include "robot.h"

void Robot_init(Robot *robot, Motor *motor_left, Motor *motor_right, float b, float kr, float kl) {
    robot->b = b;
    robot->kr = kr;
    robot->kl = kl;

    robot->theta = 0;
    robot->x = 0;
    robot->y = 0;
    robot->x_prev = 0;
    robot->y_prev = 0;
    robot->vx = 0;
    robot->vy = 0;

    robot->target_x = 0;
    robot->target_y = 0;
    robot->target_theta = 0;

    robot->wasd_control = 0;
    robot->auto_control = 0;

    robot->pos_control = 1;
    robot->rpm_control = 0;

    robot->motor_left = motor_left;
    robot->motor_right = motor_right;
}

void Robot_get_commands(Robot *robot) {
    char *command_pt = (char*) UART_getstring_non_blocking();

    if (!command_pt)
        return ;
    
    char command[MAX_BUF_SIZE];
    memcpy(command, command_pt, MAX_BUF_SIZE);
    command[MAX_BUF_SIZE - 1] = '\0';

    //printf("IN%sFIN\n", command); // DEBUG
    
    // remove newline (maybe i should do it directly in UART_getstring_non_blocking)
    command[strcspn(command, "\n\r")] = '\0';
    
    //printf("IN%sFIN\n", command); // DEBUG

    // parse command
    char *cmd_args[MAX_COMMAND_ARGS];
    int arg_count = 0;

    char *token = strtok(command, " ");

    while (token && arg_count < MAX_COMMAND_ARGS) {
        cmd_args[arg_count++] = token;
        token = strtok(NULL, " ");
    }

    if (arg_count == 0)
        return ;

    // COMMAND INTERFACE:

    /* [MOTOR OPEN LOOP COMMANDS] */ 
    // attach <motor1> <motor2>
    if (!strcmp(cmd_args[0], MOTOR_ATTACH_COMMAND) && arg_count == MOTOR_ATTACH_COMMAND_ARGS) {
        Motor_attach(robot->motor_left, atoi(cmd_args[1]));
        Motor_attach(robot->motor_right, atoi(cmd_args[2]));
    }
    // pwm <dir_motor1_left> <duty_cycle%_motor_left> <dir_motor_right> <duty_cycle%_motor_right>
    else if (!strcmp(cmd_args[0], PWM_COMMAND) && arg_count == PWM_COMMAND_ARGS) {
        robot->pos_control = 0;
        robot->rpm_control = 0;
        robot->motor_left->manual_control = 1;
        robot->motor_right->manual_control = 1;
        Motor_set_pwm(robot->motor_left, atoi(cmd_args[1]), (uint16_t) atoi(cmd_args[2]));
        Motor_set_pwm(robot->motor_right, atoi(cmd_args[3]), (uint16_t) atoi(cmd_args[4]));
    }


    /* [MOTOR CLOSED LOOP COMMANDS] */
    // gain <kp> <ki> <kd>
    else if (!strcmp(cmd_args[0], GAIN_COMMAND) && arg_count == GAIN_COMMAND_ARGS) {
        Motor_PID_params(robot->motor_left, atol(cmd_args[1]), atol(cmd_args[2]), atol(cmd_args[3]));
        Motor_PID_params(robot->motor_right, atol(cmd_args[1]), atol(cmd_args[2]), atol(cmd_args[3]));
    }
    // pos	<pos_motor_1> <pos_motor_2> 
    else if (!strcmp(cmd_args[0], POS_COMMAND) && arg_count == POS_COMMAND_ARGS) {
        robot->pos_control = 1;
        robot->rpm_control = 0;
        robot->motor_left->manual_control = 0;
        robot->motor_right->manual_control = 0;
        robot->motor_left->target_pos = atol(cmd_args[1]);
        robot->motor_right->target_pos = atol(cmd_args[2]);
    }
    // rpm	<rpm_motor_1> <rpm_motor_2> 
    else if (!strcmp(cmd_args[0], RPM_COMMAND) && arg_count == RPM_COMMAND_ARGS) {
        robot->pos_control = 0;
        robot->rpm_control = 1;
        robot->motor_left->manual_control = 0;
        robot->motor_right->manual_control = 0;
        robot->motor_left->target_rpm = atoi(cmd_args[1]);
        robot->motor_right->target_rpm = atoi(cmd_args[2]);
    }


    /* [ROBOT COMMANDS] */
    // setwasd <1/0>
    else if (!strcmp(cmd_args[0], SETWASD_COMMAND) && arg_count == SETWASD_COMMAND_ARGS) {
        robot->wasd_control = atoi(cmd_args[1]);
    }
    // goto <x> <y>
    else if (!strcmp(cmd_args[0], ROBOT_GOTO_COMMAND) && arg_count == ROBOT_GOTO_COMMAND_ARGS) {
        robot->wasd_control = 0;
        robot->auto_control = 1;
        robot->pos_control = 0;
        robot->rpm_control = 1;
        //robot->pos_control = 0; // UNCOMMENT IF USING DIRECT PWM CONTROL
        //robot->rpm_control = 0; // UNCOMMENT IF USING DIRECT PWM CONTROL
        robot->target_x = (float) atoi(cmd_args[1]);
        robot->target_y = (float) atoi(cmd_args[2]);
    }
}

// direct WASD control in case WASD control is active
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

void Robot_update_odometry(Robot *robot) {
    float theta = robot->theta;

    float d_tick_l = robot->motor_left->encoder->pos_diff;
    float d_tick_r = robot->motor_right->encoder->pos_diff;
    
    // CALIBRATION PARAMS
    // nominally    kr = kl = (robot->wheel_circ / TICKS_PER_REV) 
    //              b = distance between wheels
    // but to achieve precision we calibrate these values
    float b = robot->b;
    float kr = robot->kr;
    float kl = robot->kl;

    float d_pl = kl * d_tick_l;
    float d_pr = kr * d_tick_r;

    float d_p = (d_pr + d_pl) / 2.0;

    float d_theta = (d_pr - d_pl) / b; // in radians

    float d_x_local;
    float d_y_local;

    // NOTA: delta_x e delta_y sono invertiti rispetto a come l'ho disegnato negli appunti, ma così è coerente con la matrice di rotazione

    // Handle della divisione per 0 (ESSENZIALE se no succede un casino)
    if (fabs(d_theta) < 0.000001) {
        d_y_local = d_p;
        d_x_local = 0;
    }
    else {
        d_y_local = d_p * (sin(d_theta) / d_theta);
        d_x_local = d_p * ((1 - cos(d_theta)) / d_theta);
    }

    // Xnew = Xold + R(theta_ass) * t
    robot->x = robot->x + (cos(theta) * d_x_local - sin(theta) * d_y_local);
    robot->y = robot->y + (sin(theta) * d_x_local + cos(theta) * d_y_local);

    robot->theta += d_theta;
}

void Robot_update_odometry_taylor(Robot *robot) {
    float theta = robot->theta;

    float d_tick_l = robot->motor_left->encoder->pos_diff;
    float d_tick_r = robot->motor_right->encoder->pos_diff;
    
    // CALIBRATION PARAMS
    float b = robot->b;
    float kr = robot->kr;
    float kl = robot->kl;

    float d_pl = kl * d_tick_l;
    float d_pr = kr * d_tick_r;

    float d_p = (d_pr + d_pl) / 2.0;

    float d_theta = (d_pr - d_pl) / b; // in radians

    float d_x_local;
    float d_y_local;

    // NOTA: delta_x e delta_y sono invertiti rispetto a come l'ho disegnato negli appunti, ma così è coerente con la matrice di rotazione
    d_x_local = d_p * (d_theta/2 - (d_theta*d_theta*d_theta)/24);
    d_y_local = d_p * (1 - (d_theta*d_theta)/6 + (d_theta*d_theta*d_theta*d_theta)/120);

    float sin_theta = theta - (theta*theta*theta)/6;
    float cos_theta = (1 - (theta*theta)/2 + (theta*theta*theta*theta)/24);

    // Xnew = Xold + R(theta_ass) * t
    robot->x = robot->x + (cos_theta * d_x_local - sin_theta * d_y_local);
    robot->y = robot->y + (sin_theta * d_x_local + cos_theta * d_y_local);

    robot->theta += d_theta;
}

void Robot_goto_position(Robot *robot) {
    if (!robot->auto_control)
        return ;

    float x = robot->x;
    float y = robot->y;
    float theta = robot->theta;

    float Rw = robot->b / 2.0;

    float target_x = robot->target_x;
    float target_y = robot->target_y;
    //int target_theta = (int) robot->target_theta; // for now i do only position control

    //float k = 500.0;
    float k = 0.7;

    // Put in fixed coord the left and right wheel coordinates
    float wl_x = x + cos(theta) * (-Rw) - sin(theta) * 0;
    float wl_y = y + sin(theta) * (-Rw) + cos(theta) * 0;
    float wr_x = x + cos(theta) * Rw - sin(theta) * 0;
    float wr_y = y + sin(theta) * Rw + cos(theta) * 0;

    // distance between wheels and target point
    float dl = sqrt((wl_x - target_x)*(wl_x - target_x) + (wl_y - target_y)*(wl_y - target_y));
    float dr = sqrt((wr_x - target_x)*(wr_x - target_x) + (wr_y - target_y)*(wr_y - target_y));
 
    float ul = k * dl;
    float ur = k * dr;

    // if you are not alligned with the target circumpherence robot has to rotate
    if (fabs(dr - dl) > 1.0) {
        if (dr > dl)
            ul = 0;
        else
            ur = 0;
    }

    // "singolarità": sei allineato ma la circonferenza è dietro di te e comando u diverge
    // --> ruota un pò fino a uscire dalla singolarità
    // ( in realtà ruoto un pò di più: fino a che d_prev > d, perché mettere (x >= target_x || y >= target_y) causa overshoot )
    
    // command divergence is caused by target being straightly behind robot, so robot has to rotate a little to exit this condition 
    float d      = sqrt((x - target_x)*(x - target_x) + (y - target_y)*(y - target_y));
    float d_prev = sqrt((robot->x_prev - target_x)*(robot->x_prev - target_x) + (robot->y_prev - target_y)*(robot->y_prev - target_y));
    robot->x_prev = x;
    robot->y_prev = y;

    if (fabs(dr - dl) <= 1.0 && d > d_prev)
        ul = 0;

    // if wheel touch target circumpherence (plus 1cm for approximation) stop moving
    if (dl <= Rw + 1.0)
        ul = 0;
    if (dr <= Rw + 1.0)
        ur = 0;

    robot->motor_left->target_rpm = (int) ul;
    robot->motor_right->target_rpm = (int) ur;

    // CONTROLLO DIRETTO SULLA PWM
    // float ul_pwm = fabs(ul);
    // float ur_pwm = fabs(ur);

    // if(ul_pwm > MAX_PWM_TICKS)
    //     ul_pwm = MAX_PWM_TICKS;
    // if(ur_pwm > MAX_PWM_TICKS)
    //     ur_pwm = MAX_PWM_TICKS;

    // Motor_set_pwm(robot->motor_left, 1, (uint16_t) ul_pwm);
    // Motor_set_pwm(robot->motor_right, 1, (uint16_t) ur_pwm);
}