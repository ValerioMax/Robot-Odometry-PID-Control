#include "shared_types.h"
#include "uart_utils.h"

#include <string.h>

#define MAX_COMMAND_ARGS 6

#define PWM_COMMAND "pwm"
#define PWM_COMMAND_ARGS 5

#define POS_COMMAND "pos"
#define POS_COMMAND_ARGS 3

#define RPM_COMMAND "rpm"
#define RPM_COMMAND_ARGS 3

#define SETMOTOR_COMMAND "setmotor"
#define SETMOTOR_COMMAND_ARGS 3

#define GAIN_COMMAND "gain"
#define GAIN_COMMAND_ARGS 4

#define SETMANUAL_COMMAND "setmanual"
#define SETMANUAL_COMMAND_ARGS 2