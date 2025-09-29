#ifndef THREAD_WRITE_H
# define THREAD_WRITE_H

# include "shared_types.h"
# include "serial.h"

void *thread_write(void *args);
void start_wasd_interface(int serial_port);

#endif