#include "thread_read.h"

// from data_utils.c
long long start_time_ms = 0;

void* thread_read(void* args) {
    t_windata windata;
    t_info axis_info;
    CircularBuffer cbuf;

    // init circular buffer for sampling
    cb_init(&cbuf, NUM_SAMPLES);

    // init window for plotting
    window_init(&windata, "plotting");

    // init serial commication
    int serial_port = serial_init(TTY_DEVICE_NAME, O_RDWR, B19200, 0, 1); // blocking = 0, timeout = 1 --> 1*0.1s = 0.1s 

    // set start time
    timer_init();
    long prev_plot_time_ms = 0;

    int fd = open(DATA_FILE_NAME, O_RDONLY);

    while (1) {
        char line[MAX_BUFFER_SIZE];
    
        // read from Serial (UART or Bluetooth) as fast as possible
        int recv_bytes = serial_readline(serial_port, line);

        if (recv_bytes > 0) {
            fill_one_sample(&cbuf, line);
            cb_print(&cbuf); // DEBUG
        }
        
        // plot data if DELTA_T_PLOT_MS has passed
        if (millis() > prev_plot_time_ms + DELTA_T_PLOT_MS) {
            //get_data_from_tsv(sample_data, fd);
            
            set_axis_info(&cbuf, &axis_info);

            plot_data(&cbuf, &windata, &axis_info);

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
