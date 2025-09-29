#ifndef SERIAL_H
# define SERIAL_H

# include <string.h>
# include <fcntl.h>
# include <termios.h>

# include "shared_types.h"

# define TTY_DEVICE_NAME "/dev/rfcomm0" // "/dev/ttyACM0"

int     serial_init(const char *tty_device, int mode, int baud, int blocking, int timeout);
int     serial_readline(int serial_port, char *buf);
int     serial_writebuf(int serial_port, const char *buf, int buf_len);
void    set_non_canon_mode(int fd);
void    set_canon_mode(int fd);

#endif