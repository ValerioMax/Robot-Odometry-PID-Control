#include "shared_types.h"
#include "motor.h"
#include "communication.h"

typedef struct {
    // posizione, angolo, velocità assulute nel sistema non inerziale
    float theta;
    float x;
    float y;
    float vx;
    float vy;

    // odometry params
    float b;
    float kr;
    float kl;
    
    // for manual control of robot itself
    int wasd_control;
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