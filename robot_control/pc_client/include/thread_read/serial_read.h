#ifndef SERIAL_READ_H
# define SERIAL_READ_H

# include <string.h>
# include <fcntl.h>
# include <termios.h>

# include "shared_types.h"

# define MAX_BUFFER_SIZE 256

int serial_init(const char *tty_device, int mode, int baud, int blocking, int timeout);
int serial_readline(int serial_port, char *buf);

#endif