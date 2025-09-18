#include <stdio.h>
#include <stdlib.h>
#include "serial.h"
#include "thread_read.h"
#include "thread_write.h"

pthread_mutex_t serial_port_mutex;

int main() {
    // init serial communication
    int serial_port = serial_init(TTY_DEVICE_NAME, O_RDWR, B19200, 0, 5); // blocking = 0, timeout = 5 --> 5*0.1s = 0.5s
    
    //sleep(1);

    // init mutex to prevent race condition in concurrent access of serial port by threads
    // TODO: ACTUALLY they should be NOT NEEDED cause writing a and reading concurrently on same serial port its not a problem
    //       because serial ports are full duplex (serial driver hadles it)
    //pthread_mutex_init(&serial_port_mutex, NULL);

    pthread_t th_read, th_write;

    // this thread reads via Bluetooth (Serial) from esp32, plots data on screen, logs data on tsv file
    pthread_create(&th_read, NULL, thread_read, &serial_port);

    // this thread reads command from terminal and send it via Bluetooth (Serial) to esp32
    pthread_create(&th_write, NULL, thread_write, &serial_port);

    pthread_join(th_read, NULL);
    pthread_join(th_write, NULL);

    // close the port
    close(serial_port);

    return 0;
}
