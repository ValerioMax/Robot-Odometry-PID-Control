#include "data_utils.h"

// void sample_data_init(t_sample *sample_data[], int len) {
//     for (int i = 0; i < len; i++)
//         sample_data[i] = malloc(sizeof(t_sample));
// }

// void free_sample_data(t_sample *sample_data[], int len) {
//     if (!sample_data)
//         return ;

//     for (int i = 0; i < len; i++) {
//         if (sample_data[i]) {
//             free(sample_data[i]);
//             sample_data[i] = NULL;
//         }
//     }
// }

// void read_line(int fd, t_sample *sample_data) {
//     // read one line (until '\n')
//     char buf[READ_BUFFER_SIZE];
//     int read_bytes = 0;
//     int ret = 0;

//     do {
//         ret = read(fd, buf + read_bytes, 1); // TODO: inefficente, prova a leggere più byte alla volta e a gestire il buffer se c'è il '\n' in mezzo
//         if (ret == -1 && errno == EINTR) continue;                          // se syscall read da errore ma è stata interrotta vai avanti
//         if (ret == -1) {printf("errore nella read da file tsv\n"); break;}  // se syscall read da altro errore esci
//         if (ret == 0) break;                                                // se syscall read ha letto 0 byte (ha finito il file)
//         read_bytes += ret;
//     }
//     while (buf[read_bytes - 1] != '\n');

//     // terminate string to use strtok()
//     buf[read_bytes - 1] = '\0';
    
//     // parse line
//     char *token;

//     token = strtok(buf, "\t "); // TODO con "\t " funziona, perche?? (dovrebbe andare anche solo con "\t")
//     if (token) sample_data->timestamp = atol(token);
//     token = strtok(NULL, "\t ");
//     if (token) sample_data->value = atoi(token);

//     //printf("%ld %d\n", sample_data->timestamp, sample_data->value);
// }

// void get_data_from_tsv(t_sample *sample_data[], int fd) {
//     // read last NUM_ROWS of tsv file
//     for (int i = 0; i < NUM_SAMPLES; i++){
//         read_line(fd, sample_data[i]); //TODO capire se la read legge dall'ultimo byte letto alla chiamata precedente oppure si resetta dall'inizio del file
//                                         // -> in questo caso bisogna passare un puntatore a dove era rimasta
//     }   
// }

// void get_axis_info(t_sample *sample_data[], t_info *axis_info) {
//     if (!sample_data || !sample_data[0] || !sample_data[NUM_SAMPLES - 1])
// 		return ;

//     // get time_min and time_max to get y-axis range
// 	axis_info->time_min = (double) sample_data[0]->timestamp;
// 	axis_info->time_max = (double) sample_data[NUM_SAMPLES - 1]->timestamp;
//     axis_info->time_range = fabs(axis_info->time_max - axis_info->time_min);

//     axis_info->value_max = MAX_SAMPLE_VALUE; // TODO change this value
//     //printf("%lf %lf %lf\n", axis_info->time_min, axis_info->time_max, axis_info->time_range);
// }






// void fill_one_sample(t_sample *sample_data, char *line) {
//     if (!line)
//         return ;

//     printf("%s\n", line);

//     // parse line
//     char *token;

//     token = strtok(line, "\t "); // TODO con "\t " funziona, perche?? (dovrebbe andare anche solo con "\t")
//     if (token) sample_data->timestamp = atol(token);
//     token = strtok(NULL, "\t ");
//     if (token) sample_data->value = atoi(token);

//     // DEBUGGOOOOOS
//     sample_data->timestamp = millis();
//     sample_data->value = 135;

//     printf("%ld %d\n", sample_data->timestamp, sample_data->value);
// }






void get_axis_info(CircularBuffer *cbuf, t_info *axis_info) {
    if (!cbuf)
		return ;

    // get time_min and time_max to get y-axis range
	axis_info->time_min = (double) (cbuf->samples[cbuf->head]).timestamp;
	axis_info->time_max = (double) (cbuf->samples[cbuf->tail]).timestamp;
    axis_info->time_range = fabs(axis_info->time_max - axis_info->time_min);

    axis_info->value_max = MAX_SAMPLE_VALUE; // TODO change this value
    //printf("%lf %lf %lf\n", axis_info->time_min, axis_info->time_max, axis_info->time_range);
}

void fill_one_sample(CircularBuffer *cbuf, char *line) {
    if (!line)
        return ;

    printf("%s\n", line);

    t_sample sample;

    // parse line
    char *token;

    token = strtok(line, "\t "); // TODO con "\t " funziona, perche?? (dovrebbe andare anche solo con "\t")
    if (token) sample.timestamp = atol(token);
    token = strtok(NULL, "\t ");
    if (token) sample.value = atoi(token);

    // add element to buffer
    enqueue(cbuf, sample);
}