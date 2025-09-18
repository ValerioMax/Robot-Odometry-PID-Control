#include "thread_write.h"
#include "data_utils.h"

#include <sched.h>

// from main.c
extern pthread_mutex_t serial_port_mutex;


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

        free(line);
        line = NULL;
        len = 0;

        // for (int i = -300; i < 300; i += 5) {
        //     dprintf(serial_port, "pos %d 0\n", i);
        //     // char buf[] = "ciaocomevatuttobene\n";
        //     // write(serial_port, buf, strlen(buf));
        //     usleep(50000);
        // }
        
    }
    
    return NULL;
}