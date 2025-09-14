#include <stdio.h>
#include <stdlib.h>
#include "thread_read.h"
#include "thread_write.h"
#include <pthread.h>

int main() {
    pthread_t th_read, th_write;

    pthread_create(&th_read, NULL, thread_read, NULL);
    pthread_create(&th_write, NULL, thread_write, NULL);

    pthread_join(th_read, NULL);
    pthread_join(th_write, NULL);

    return 0;
}