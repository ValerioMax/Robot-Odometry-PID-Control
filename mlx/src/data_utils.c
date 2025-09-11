#include "data_utils.h"

void data_init(t_sample *sample_data[]) {
    for (int i = 0; i < NUM_SAMPLES; i++)
        sample_data[i] = malloc(sizeof(t_sample));
}

void read_line(int fd, t_sample *sample_data) {
    // read one line (until '\n')
    char buf[READ_BUFFER_SIZE];
    int read_bytes = 0;
    int ret = 0;

    do {
        ret = read(fd, buf + read_bytes, 1); // TODO: inefficente, prova a leggere più byte alla volta e a gestire il buffer se c'è il '\n' in mezzo
        if (ret == -1 && errno == EINTR) continue;                          // se syscall read da errore ma è stata interrotta vai avanti
        if (ret == -1) {printf("errore nella read da file tsv\n"); break;}  // se syscall read da altro errore esci
        if (ret == 0) break;                                                // se syscall read ha letto 0 byte (ha finito il file)
        read_bytes += ret;
    }
    while (buf[read_bytes - 1] != '\n');

    // terminate string to use strtok()
    buf[read_bytes - 1] = '\0';
    
    // parse line
    char *token;

    token = strtok(buf, "\t "); // TODO con "\t " funziona, perche?? (dovrebbe andare anche solo con "\t")
    if (token) sample_data->timestamp = atol(token);
    token = strtok(NULL, "\t ");
    if (token) sample_data->value = atoi(token);

    //printf("%ld %d\n", sample_data->timestamp, sample_data->value);
}

void get_data_from_tsv(t_sample *sample_data[], const char *filename) {
    int fd = open(filename, O_RDONLY);

    // read last NUM_ROWS of tsv file
    for (int i = 0; i < NUM_SAMPLES; i++){
        read_line(fd, sample_data[i]); //TODO capire se la read legge dall'ultimo byte letto alla chiamata precedente oppure si resetta dall'inizio del file
                                        // -> in questo caso bisogna passare un puntatore a dove era rimasta
    }
        
}