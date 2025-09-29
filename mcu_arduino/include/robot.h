#ifndef ROBOT_H
#define ROBOT_H

#include "shared_types.h"
#include "motor.h"
#include "communication.h"

typedef struct {
    // posizione, angolo, velocità assolute nel sistema non inerziale
    float theta;
    float x;
    float y;
    float x_prev; // for robot_goto()
    float y_prev; // for robot_goto()
    float vx;
    float vy;

    // posizione e orientamento desiderati
    float target_x;
    float target_y;
    float target_theta;

    // odometry params
    float b;
    float kr;
    float kl;
    
    int wasd_control; // manual WASD control of robot itself
    int auto_control; // autonomous control of robot itself

    // type of control action on the motors
    int pos_control;
    int rpm_control;

    Motor *motor_left;
    Motor *motor_right;
} Robot;

void Robot_init(Robot *robot, Motor *motor_left, Motor *motor_right, float b, float kr, float kl);
void Robot_get_commands(Robot *robot);
void Robot_get_wasd(Robot *robot);
void Robot_update_odometry(Robot *robot);
void Robot_update_odometry_taylor(Robot *robot);
void Robot_goto_position(Robot *robot);

#endif