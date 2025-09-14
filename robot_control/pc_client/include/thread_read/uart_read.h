#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#define MAX_BUFFER_SIZE 256

int uart_init(const char *tty_device, int mode, int baud, int blocking, int timeout);
int uart_readline(int serial_port, char *buf);