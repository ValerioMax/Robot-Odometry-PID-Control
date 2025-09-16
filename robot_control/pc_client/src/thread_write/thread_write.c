#include "thread_write.h"

// from main.c
extern pthread_mutex_t serial_port_mutex;

void *thread_write(void *args) {
    int serial_port = *((int *) args); // TODO: little hardcoded imo
    char *line;
    size_t len;

    while (1) {
        //printf("AAA\n");
        //fflush(stdout);
        printf("Enter command: \n");

        ssize_t command = getline(&line, &len, stdin);

        pthread_mutex_lock(&serial_port_mutex);
        int written = serial_writebuf(serial_port, (const char *) line, (int) strlen(line));
        pthread_mutex_unlock(&serial_port_mutex);

        // pthread_mutex_lock(&serial_port_mutex);
        // write(serial_port, "Halo\n", strlen("Halo\n"));
        // pthread_mutex_unlock(&serial_port_mutex);
        
        //int written = serial_writebuf(serial_port, "HELLO\n", (int) strlen("HELLO\n"));
        //printf("LINE: %ld %ld\n", sizeof(line), strlen(line));
        
        free(line);

        line = NULL;
        len = 0;

        sleep(2);
    }
    
    return NULL;
}