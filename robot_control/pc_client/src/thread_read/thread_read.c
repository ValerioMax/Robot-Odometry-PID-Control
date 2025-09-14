#include "thread_read.h"

long start_time_ms;

// set start time when called
void timer_init() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    start_time_ms = (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// get elapsed milliseconds since start_time_ms
long millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long current_time_ms = (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return current_time_ms - start_time_ms;
}

void* thread_read(void* args) {
    t_windata windata;
    t_info axis_info;
    //t_sample *sample_data[NUM_SAMPLES]; // TODO ma poi perche cazz l'avevo fatta t_sample *[] e non semplicemente t_sample *
    t_sample_queue sample_queue;

    // init window for plotting
    init_window(&windata);

    int serial_port = uart_init(TTY_DEVICE_NAME, O_RDWR, B19200, 0, 1); // blocking = 0, timeout = 1 --> 1*0.1s = 0.1s 

    // set start time
    timer_init();
    long prev_plot_time_ms = 0;

    int fd = open(DATA_FILE_NAME, O_RDONLY);

    int sample_idx = 0;

    while (1) {
        char line[MAX_BUFFER_SIZE];
    
        // read from UART as fast as possible
        int recv_bytes = uart_readline(serial_port, line);

        // // fresh new buffer for sample data
        // sample_data_init(sample_data, NUM_SAMPLES);
        // if (recv_bytes > 0) {
        //     // this parse received string and assign it to sample_data[idx]
        //     fill_one_sample(sample_data[sample_idx], line);
        //     sample_idx++;
        //     printf("IDX %d\n", sample_idx);
        // }
        
        // plot data if DELTA_T_PLOT_MS has passed anf NUM_SAMPLES was collected
        if (millis() > prev_plot_time_ms + DELTA_T_PLOT_MS && sample_idx >= NUM_SAMPLES) { // TODO maybe i can take off idx >= NUM_SAMPLES
            printf("enterd!!!!\n");
            //get_data_from_tsv(sample_data, fd);
            
            get_axis_info(sample_data, &axis_info);

            plot_data(sample_data, &windata, &axis_info);

            // // reset sample data vector and idx
            // free_sample_data(sample_data, NUM_SAMPLES); // TODO: should only clean it, not totally deallocate it
            // sample_idx = 0;

            prev_plot_time_ms = millis();
        }


        // writee data into log tsv file every DELTA_T_LOG_MS
        // ...
    }
    
    // deallocate dynamic data
    free_sample_data(sample_data, NUM_SAMPLES);

    // close the port
    close(serial_port);

    // close file tsv
    close(fd);

    return NULL;
}
