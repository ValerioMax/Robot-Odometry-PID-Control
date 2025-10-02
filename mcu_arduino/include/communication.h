#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "shared_types.h"
#include "uart_utils.h"

#include <string.h>

#define MAX_COMMAND_ARGS 6

#define MOTOR_ATTACH_COMMAND "attach"
#define MOTOR_ATTACH_COMMAND_ARGS 3

#define PWM_COMMAND "pwm"
#define PWM_COMMAND_ARGS 5

#define GAIN_COMMAND "gain"
#define GAIN_COMMAND_ARGS 4

#define POS_COMMAND "pos"
#define POS_COMMAND_ARGS 3

#define RPM_COMMAND "rpm"
#define RPM_COMMAND_ARGS 3

#define SETWASD_COMMAND "setwasd"
#define SETWASD_COMMAND_ARGS 1

#define ROBOT_GOTO_COMMAND "goto"
#define ROBOT_GOTO_COMMAND_ARGS 3

#define ROBOT_LOG_COMMAND "log"
#define ROBOT_LOG_COMMAND_ARGS 2

#endif