#include "data_utils.h"

void data_init(int **sample_data){
    sample_data = malloc(NUM_ROWS * sizeof(int*));
    for (int i = 0; i < NUM_ROWS; i++)
        sample_data[i] = malloc(NUM_COLUMNS * sizeof(int));
}

// reads one line of a file (until '\n')
void read_line(int fd, int *buf) {
    int read_bytes = 0;
    int ret = 0;

    do {
        ret = read(fd, buf + read_bytes, 1);
        if (ret == -1 && errno == EINTR) continue;                          // se syscall read da errore ma è stata interrotta vai avanti
        if (ret == -1) {printf("errore nella read da file tsv\n"); break;}  // se syscall read da altro errore esci
        if (ret == 0) break;                                                // se syscall read ha letto 0 byte (ha finito il file)
        read_bytes += ret;
    }
    while (buf[read_bytes - 1] != '\n');
}

void get_data_from_tsv(int **sample_data, const char *filename) {
    int fd = open(filename, O_RDONLY);

    // read last NUM_ROWS of tsv file
    for (int i = 0; i < NUM_ROWS; i++)
        read_line(fd, sample_data[i]);
}