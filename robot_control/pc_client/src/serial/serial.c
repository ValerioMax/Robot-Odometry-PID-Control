#include "serial.h"

int serial_init(const char *tty_device, int mode, int baud, int blocking, int timeout) {
    // open serial port
    int serial_port = open(tty_device, mode);
    if (serial_port < 0) {
        printf("Error uart_init(): %s\n", strerror(errno));
        return -1;
    }

    // configure port settings
    struct termios tty;
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error uart_init(): %s\n", strerror(errno));
        return -1;
    }

    // set baudrate
    cfsetospeed(&tty, baud); // output bitrate
    cfsetispeed(&tty, baud); // input bitrate

    // set data bits, stop bits, parity bit
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= (CREAD | CLOCAL);
    
    // set local, input, output settings
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    tty.c_oflag &= ~(OPOST | ONLCR);
    
    // set timeout
    tty.c_cc[VTIME] = timeout; // waits up to 1 sec (10 * 0.1s) to read a byte

    // set blocking/non-blocking read
    tty.c_cc[VMIN] = blocking; // 0: non blocking, 1: blocking

    // NOTE: its actually "blocking" because of timeout, but not indefinetly blocking, also timeout can be set to 0

    // save settings
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    return serial_port;
}

int serial_readline(int serial_port, char *buf) {
    int read_bytes = 0;
    int ret = 0;

    do {
        ret = read(serial_port, buf + read_bytes, 1); // TODO: inefficente, prova a leggere più byte alla volta e a gestire il buffer se c'è il '\n' in mezzo
        if (ret == -1 && errno == EINTR) continue;                                  // se syscall read da errore ma è stata interrotta vai avanti
        if (ret == -1) {printf("Errore nella read da tty serial port\n"); break;}   // se syscall read da altro errore esci
        if (ret == 0) break;                                                        // se syscall read ha letto 0 byte (ha finito il file)
        read_bytes += ret;
    }
    while (buf[read_bytes - 1] != '\n');

    if (read_bytes > 0) {
        // terminate string to use strtok()
        buf[read_bytes - 1] = '\0';

        //printf("received: %s\n", buf); // DEBUG
    }

    return read_bytes;
}

// just writes a buffer: its user duty to '\0' terminate and/or put '\n' at the end of it
int serial_writebuf(int serial_port, const char *buf, int buf_len) {
    int written_bytes = 0;
    int ret = 0;

    while (written_bytes < buf_len) {
        ret = write(serial_port, buf + written_bytes, buf_len - written_bytes);
        if (ret == -1 && errno == EINTR) continue;                                  // se syscall write da errore ma è stata interrotta vai avanti
        if (ret == -1) {printf("Errore nella write su tty serial port\n"); break;}  // se syscall write da altro errore esci
        written_bytes += ret;
    }
    
    // DEBUG
    if (written_bytes == buf_len)
        printf("sent\n");

    return written_bytes;
}