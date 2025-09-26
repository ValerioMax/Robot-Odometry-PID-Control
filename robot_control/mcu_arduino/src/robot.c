#include "robot.h"

void Robot_init(Robot *robot, Motor *motor_left, Motor *motor_right, float b, float kr, float kl) {
    robot->b = b;
    robot->kr = kr;
    robot->kl = kl;

    robot->theta = 0;
    robot->x = 0;
    robot->y = 0;
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
    
    //printf("IN%sFIN\n", command); // DEBUG

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
        robot->pos_control = 1;
        robot->rpm_control = 0;

        robot->motor_left->manual_control = 0;
        robot->motor_right->manual_control = 0;
        robot->motor_left->target_pos = atol(cmd_args[1]);
        robot->motor_right->target_pos = atol(cmd_args[2]);
    }
    // [CLOSED LOOP]
    // rpm	<rpm_motor_1> <rpm_motor_2> 
    else if (!strcmp(cmd_args[0], RPM_COMMAND) && arg_count == RPM_COMMAND_ARGS) {
        robot->pos_control = 0;
        robot->rpm_control = 1;
    
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

// // from encoder readings compute the arc of circumpherence (delta_p) did by the center of the wheels
// // and the angle (delta_theta) done along the "circonferenza osculatrice" of the arc percurrend in current in last iteration 
// void Robot_compute_arc_and_theta(Robot *robot) {
//     float d_pl = (float) Encoder_get_tick_diff(robot->motor_left->encoder);
//     float d_pr = (float) Encoder_get_tick_diff(robot->motor_right->encoder);
    
//     robot->d_p = (d_pr + d_pl) / 2;
//     robot->d_theta = abs(d_pr - d_pl) / (2 * robot->Rw);
// }

// // from delta_p and delta_t computes current delta_x, delta_y and sets delta_x, delta_y, delta_theta 
// void Robot_m2t(Robot *robot) {
//     // t = [d_x, d_y]'
//     robot->d_x = robot->d_p * (sin(robot->d_theta) / robot->d_theta);
//     robot->d_y = robot->d_p * ((1 - cos(robot->d_theta)) / robot->d_theta);
// }

void Robot_update_odometry(Robot *robot) {
    float theta = robot->theta;

    float d_tick_l = robot->motor_left->encoder->pos_diff;
    float d_tick_r = robot->motor_right->encoder->pos_diff;
    
    // CALIBRATION PARAMS
    // these in ideal case are (robot->wheel_circ / TICKS_PER_REV) but could be different so we got to calibrate them
    // [ (robot->wheel_circ / TICKS_PER_REV) è il valore "nominale", poi però va calibrato ]
    float b = robot->b;
    float kr = robot->kr;
    float kl = robot->kl;

    float d_pl = kl * d_tick_l;
    float d_pr = kr * d_tick_r;

    float d_p = (d_pr + d_pl) / 2.0; // computes in radians

    float d_theta = (d_pr - d_pl) / b;

    float d_x_local;
    float d_y_local;


    // NOTA: delta_x e delta_y sono invertiti rispetto a come l'ho disegnato negli appunti, ma così è coerente con la matrice di rotazione

    // Handle della divisione per 0 (ESSENZIALE se no succede un casino)
    // TODO: POI MAGARI USA LO SVILUPPO DI TAYLOR che non ha denominatori
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
    // these in ideal case are (robot->wheel_circ / TICKS_PER_REV) but could be different so we got to calibrate them
    // [ (robot->wheel_circ / TICKS_PER_REV) è il valore "nominale", poi però va calibrato ]
    float b = robot->b;
    float kr = robot->kr;
    float kl = robot->kl;

    float d_pl = kl * d_tick_l;
    float d_pr = kr * d_tick_r;

    float d_p = (d_pr + d_pl) / 2.0; // computes in radians

    float d_theta = (d_pr - d_pl) / b;

    float d_x_local;
    float d_y_local;

    // NOTA: delta_x e delta_y sono invertiti rispetto a come l'ho disegnato negli appunti, ma così è coerente con la matrice di rotazione
    d_x_local = d_p * (d_theta/2 - (d_theta*d_theta*d_theta)/24);
    d_y_local = d_p * (1 - (d_theta*d_theta)/6 + (d_theta*d_theta*d_theta*d_theta)/120);

    // Xnew = Xold + R(theta_ass) * t
    float sin_theta = theta - (theta*theta*theta)/6;
    float cos_theta = (1 - (theta*theta)/2 + (theta*theta*theta*theta)/24);

    robot->x = robot->x + (cos_theta * d_x_local - sin_theta * d_y_local);
    robot->y = robot->y + (sin_theta * d_x_local + cos_theta * d_y_local);

    robot->theta += d_theta;
}

void Robot_goto_position(Robot *robot) {
    float x = robot->x;
    float y = robot->y;
    float theta = robot->theta;

    float Rw = robot->b / 2.0;

    float target_x = robot->target_x;
    float target_y = robot->target_y;
    //int target_theta = (int) robot->target_theta; // for now i do only position control

    float k = 500.0;

    // PORTA CON MATRICE DI ROTAZIONE NEL SISTEMA DELLA TRAIETTORIA (con asse y la il segmento fra A e B)
    float wl_x = x + cos(theta) * (-Rw) - sin(theta) * 0;
    float wl_y = y + sin(theta) * (-Rw) + cos(theta) * 0;
    float wr_x = x + cos(theta) * Rw - sin(theta) * 0;
    float wr_y = y + sin(theta) * Rw + cos(theta) * 0;

    // CALCOLA d(WL, B) d(WR, B)
    float dl = sqrt((wl_x - target_x)*(wl_x - target_x) + (wl_y - target_y)*(wl_y - target_y));
    float dr = sqrt((wr_x - target_x)*(wr_x - target_x) + (wr_y - target_y)*(wr_y - target_y));
 
    float ul = k * dl;
    float ur = k * dr;

    

    if (dl <= Rw + 2) // più un intorno di 2cm per approssimare
        ul = 0;
    if (dr <= Rw + 2) // più un intorno di 2cm per approssimare
        ur = 0;

    printf("%d %d (%d %d, %d %d) %d %d, %ld %ld\n",
            (int)robot->target_x,
            (int)robot->target_y,
            (int)wl_x,
            (int)wl_y,
            (int)wr_x,
            (int)wr_y,
            (int)dl,
            (int)dr,
            (long)ul,
            (long)ur
        );




    // AL MOMENTO STO FACENDO UN CONTROLLO DIRETTO SULLA PWM, POI DOVRò USARE PID_RPM COME AZIONE
    float ul_pwm = fabs(ul);
    float ur_pwm = fabs(ur);

    if(ul_pwm > MAX_PWM_TICKS)
        ul_pwm = MAX_PWM_TICKS;
    if(ur_pwm > MAX_PWM_TICKS)
        ur_pwm = MAX_PWM_TICKS;

    Motor_set_pwm(robot->motor_left, 1, (uint16_t) ul_pwm);
    Motor_set_pwm(robot->motor_right, 1, (uint16_t) ur_pwm);
}