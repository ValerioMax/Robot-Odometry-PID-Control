#include <stdio.h>
#include <stdlib.h>
#include "thread_read.h"
#include "thread_write.h"
#include <pthread.h>

int main() {
    pthread_t th_read, th_write;

    // this thread reads from UART from arduino, plots data on screen and logs data on tsv file
    pthread_create(&th_read, NULL, thread_read, NULL);

    // this thread reads command from terminal and send it via Bluetooth (Serial) to esp32 [or bluetooth module for arduino]
    pthread_create(&th_write, NULL, thread_write, NULL);

    pthread_join(th_read, NULL);
    pthread_join(th_write, NULL);

    return 0;
}