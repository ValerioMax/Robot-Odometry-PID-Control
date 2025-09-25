#include "thread_write.h"
#include "data_utils.h"

#include <sched.h>

// from main.c
extern pthread_mutex_t serial_port_mutex;

int plot_pos = 1;
int plot_rpm = 0;

void *thread_write(void *args) {

    int serial_port = *((int *) args); // TODO: little hardcoded imo
    char *line;
    size_t len;
    long long last_time = 0;

    sleep(1);   // FONDAMENTALE PER LA SINCRONIZZAZIONE MORTACCI SUA
                // SE NON LA METTO E SE DA QUI SCRIVO A FREQ ALTA ARDUINO NON RICEVE PIU
                // APPENA APRO LA SERIALE QUESTO THREAD INIZIA A ESEGUIRE
                // ARDUINO APPENA RICEVE UN TENTATIVO DI CONNESSIONE SERIALE SI RESETTA E CI METTE UNA FRAZIONE DI TEMPO
                // QUESTO SLEEP SERVE PER FAR ASPETTARE CHE ARDUINO COMPLETI QUESTO PROCESSO DEL TUTTO SENNO SI SFANCULA TUTTO A QUANTO PARE

    while (1) {
        //printf("AAA\n");
        //fflush(stdout);

        printf("Enter command: \n");

        ssize_t command = getline(&line, &len, stdin);

        //pthread_mutex_lock(&serial_port_mutex);
        serial_writebuf(serial_port, (const char *) line, (int) strlen(line));
        //pthread_mutex_unlock(&serial_port_mutex);

        if (!strncmp(line, "pos", 3)) {
            plot_pos = 1;
            plot_rpm = 0;
        }
        else if (!strncmp(line, "rpm", 3)) {
            plot_pos = 0;
            plot_rpm = 1;
        }
        else if (!strncmp(line, "setwasd 1", 9))
            start_wasd_interface(serial_port); // TODO: MAYBE UNCOMMENT the ECHO and also use printf("%c\n") so chars are printed sempre a capo
            
        free(line);
        line = NULL;
        len = 0;
    }
    
    return NULL;
}

void start_wasd_interface(int serial_port) {

    set_non_canon_mode(STDIN_FILENO);

    char c;

    while (1) {
        // read a single character from stdin (non blocking because termios setted like that)
        int bytes_read = read(STDIN_FILENO, &c, 1);

        if (bytes_read > 0) {
            // immediately write the char to the serial port
            write(serial_port, &c, 1);
            //printf("%c\n", c);
            
            if (c == 'q') {
                set_canon_mode(STDIN_FILENO);
                return ;
            }
        }
    }
}