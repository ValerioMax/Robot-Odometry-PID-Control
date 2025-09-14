#include "thread_read.h"

// from data_utils.c
long long start_time_ms = 0;

void* thread_read(void* args) {
    t_windata windata;
    t_info axis_info;
    //t_sample *sample_data[NUM_SAMPLES]; // TODO ma poi perche cazz l'avevo fatta t_sample *[] e non semplicemente t_sample *
    CircularBuffer cbuf;

    // init window for plotting
    window_init(&windata);

    // init circular buffer
    cb_init(&cbuf, NUM_SAMPLES);

    int serial_port = uart_init(TTY_DEVICE_NAME, O_RDWR, B19200, 0, 1); // blocking = 0, timeout = 1 --> 1*0.1s = 0.1s 

    // set start time
    timer_init();
    long prev_plot_time_ms = 0;

    int fd = open(DATA_FILE_NAME, O_RDONLY);

    while (1) {
        char line[MAX_BUFFER_SIZE];
    
        // read from UART as fast as possible
        int recv_bytes = uart_readline(serial_port, line);
        //sleep(1);
        // // fresh new buffer for sample data
        // sample_data_init(sample_data, NUM_SAMPLES);
        // if (recv_bytes > 0) {
        //     // this parse received string and assign it to sample_data[idx]
        //     fill_one_sample(sample_data[sample_idx], line);
        //     sample_idx++;
        //     printf("IDX %d\n", sample_idx);
        // }

        if (recv_bytes > 0) {
            fill_one_sample(&cbuf, line);
            cb_print(&cbuf); // DEBUG
        }
        //printf("%lld\n", millis());
        
        // plot data if DELTA_T_PLOT_MS has passed anf NUM_SAMPLES was collected
        if (millis() > prev_plot_time_ms + DELTA_T_PLOT_MS) {
            //get_data_from_tsv(sample_data, fd);
            
            set_axis_info(&cbuf, &axis_info);

            plot_data(&cbuf, &windata, &axis_info);

            // // // reset sample data vector and idx
            // free_sample_data(sample_data, NUM_SAMPLES); // TODO: should only clean it, not totally deallocate it
            // sample_idx = 0;

            prev_plot_time_ms = millis();
        }


        // write data into log tsv file every DELTA_T_LOG_MS
        // ...
    }
    
    // deallocate dynamic data
    cb_destroy(&cbuf);

    // close the port
    close(serial_port);

    // close file tsv
    close(fd);

    return NULL;
}
