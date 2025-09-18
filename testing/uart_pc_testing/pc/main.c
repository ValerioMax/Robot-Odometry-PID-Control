#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

int main() {
    // Open the serial port
    int serial_port = open("/dev/ttyACM0", O_RDWR);

    if (serial_port < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return 1;
    }

    // Configure the serial port settings
    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    // Set the baud rate to 19200
    cfsetospeed(&tty, B19200);
    cfsetispeed(&tty, B19200);

    // Other settings
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

    // Minimum characters to read and time to wait
    tty.c_cc[VTIME] = 5;    // Wait for 0.5 seconds
    tty.c_cc[VMIN] = 0;     // No minimum characters to read

    // Apply the settings
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }




    sleep(1);

    while (1) {
        char c;
    
        // Write the command
        // c = 'F';
        // write(serial_port, &c, sizeof(c));
        // c = '\n';
        // write(serial_port, &c, sizeof(c));

        char buf[] = "ciaocomevatuttobene\n";
        write(serial_port, buf, strlen(buf));   // NON INCLUDERE '\0' NEI BYTE DA SCRIVERE!!!!!
                                                // ALTRIMENTI ARDUINO NON RICEVE (perche???)


        char read_buf[256];
        int n = read(serial_port, &read_buf, sizeof(read_buf));

        if (n > 0) {
            read_buf[n] = '\0';
            printf("Received: %s\n", read_buf);
        }
        else {
            printf("No response.\n");
        }

        usleep(50000);
    }
    
    // Close the serial port
    close(serial_port);

    return 0;
}