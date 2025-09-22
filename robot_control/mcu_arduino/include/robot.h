#include "shared_types.h"
#include "motor.h"
#include "communication.h"

typedef struct {
    float Rw; // distanza tra ruote diviso due
    float wheel_circ; // lunghezza della circonferenza della ruota

    // posizione, angolo, velocità assulute nel sistema non inerziale
    float theta;
    float x;
    float y;
    float vx;
    float vy;

    // for manual control of robot itself
    int wasd_control;
    int pos_control;
    int rpm_control;

    Motor *motor_left;
    Motor *motor_right;
} Robot;

void Robot_init(Robot *robot, Motor *motor_left, Motor *motor_right, float wheel_radius, float wheel_circ);
void Robot_get_commands(Robot *robot);
void Robot_get_wasd(Robot *robot);
void Robot_update_odometry(Robot *robot);