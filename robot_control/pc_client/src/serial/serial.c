#include "serial.h"

int serial_init(const char *tty_device, int mode, int baud, int blocking, int timeout) {
    // open serial port
    int serial_port = open(tty_device, mode);

    if (serial_port < 0) {
        printf("Error serial_init: %s\n", strerror(errno));
        return -1;
    }

    // configure port settings
    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error serial_init: %s\n", strerror(errno));
        return -1;
    }

    // set baudrate
    cfsetospeed(&tty, baud); // output bitrate
    cfsetispeed(&tty, baud); // input bitrate

    // set data bits, stop bits, parity bit
    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // One stop bit
    tty.c_cflag &= ~CSIZE;  // Clear size bits
    tty.c_cflag |= CS8;     // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // No hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Enable receiver and ignore modem control lines

    tty.c_lflag &= ~ICANON; // Disable canonical mode
    tty.c_lflag &= ~ECHO;   // Disable echo
    tty.c_lflag &= ~ECHOE;  // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable newline echo
    tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT, etc.

    // TODO: UNCOMMENT
    // tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    // tty.c_oflag &= ~(OPOST | ONLCR);
    
    // set timeout
    tty.c_cc[VTIME] = timeout; // waits up to 1 sec (10 * 0.1s) to read a byte

    // set blocking/non-blocking read
    tty.c_cc[VMIN] = blocking;  // minimum num of byte to read before unlocking
                                // --> 0 sets read() calls as not blocking 

    // NOTE: its actually "blocking" because of timeout, but not indefinetly blocking, also timeout can be set to 0

    // save settings
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    return serial_port;
}

void set_non_canon_mode(int fd) {
    // configure stdin to non-canonical mode
    struct termios tty_curr;
    tcgetattr(fd, &tty_curr);
    tty_curr.c_lflag &= ~ICANON;
    //tty_curr.c_lflag &= ~ECHO;
    tcsetattr(fd, TCSANOW, &tty_curr);

    printf("Non Canonical Mode. Press 'q' to quit.\n");
}

void set_canon_mode(int fd) {
    // configure stdin to canonical mode
    struct termios tty_curr;
    tcgetattr(fd, &tty_curr);
    tty_curr.c_lflag &= ICANON;
    //tty_curr.c_lflag &= ECHO;
    tcsetattr(fd, TCSANOW, &tty_curr);

    printf("Returning to Canonical Mode.\n");
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