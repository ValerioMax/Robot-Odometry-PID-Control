#include "shared_types.h"
#include "motor.h"
#include "communication.h"

typedef struct {
    float x;
    float y;
    float vx;
    float vy;

    Motor *motor_left;
    Motor *motor_right;
} Robot;

void Robot_init(Robot *robot, Motor *motor_left, Motor *motor_right);
void Robot_get_commands(Robot *robot);