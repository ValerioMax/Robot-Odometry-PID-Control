#ifndef THREAD_READ_H
# define THREAD_READ_H

# include "shared_types.h"
# include "graphics_utils.h"
# include "data_utils.h"
# include "uart_read.h"
# include "circular_buffer.h"

# include <sys/time.h>

# define TTY_DEVICE_NAME "/dev/ttyACM0"
# define DELTA_T_PLOT_MS 50 

void *thread_read(void* args);

#endif