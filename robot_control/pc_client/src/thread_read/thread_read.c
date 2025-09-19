#include "thread_read.h"

// from main.c
extern pthread_mutex_t serial_port_mutex;

long long start_time_ms = 0;
long long prev_plot_time_ms = 0;
long long prev_log_time_ms = 0;

void *thread_read(void *args) {
    t_windata windata;
    t_info axis_info;
    CircularBuffer cbuf;

    // init circular buffer for sampling
    cb_init(&cbuf, NUM_SAMPLES);

    // init window for plotting
    window_init(&windata, "plotting");

    // set y axis range
    set_y_axis_info(&axis_info, MAX_SAMPLE_VALUE_DEFAULT);

    // init serial commication
    int serial_port = *((int *) args); // TODO: hardcoded, but i think apposite thread_types.h and struct is unnecessaty overhead (?)

    // set start time
    timer_init();

    // open tsv file for logging
    int fd = open(DATA_FILE_NAME, O_RDWR);
    dprintf(fd, "%s\n", TSV_HEADER);

    taskdata task_data;
    task_data.windata = &windata;
    task_data.cbuf = &cbuf;
    task_data.axis_info = &axis_info;
    task_data.serial_port = serial_port;
    task_data.fd = fd;

    // while (1) { ... }

    // using mlx_loop_hook() instead of while(1) loop cause otherwise i can't use mlx_key_hook()
    mlx_loop_hook(windata.mlx, loop_task, &task_data);
    mlx_key_hook(windata.win, key_handler, &axis_info);
    mlx_loop(windata.mlx);

    // deallocate dynamic data
    cb_destroy(&cbuf);

    // close file tsv
    close(fd);

    return NULL;
}

int loop_task(taskdata *data) {
    char line[MAX_BUFFER_SIZE];

    // read from Serial (UART or Bluetooth) as fast as possible
    //pthread_mutex_lock(&serial_port_mutex);
    int recv_bytes = serial_readline(data->serial_port, line);
    //pthread_mutex_unlock(&serial_port_mutex);

    if (recv_bytes > 0) {
        fill_one_sample(data->cbuf, line);
        //cb_print(&cbuf); // DEBUG
    }
    
    // plot data if DELTA_T_PLOT_MS has passed
    if (millis() > prev_plot_time_ms + DELTA_T_PLOT_MS) {
        //get_data_from_tsv(sample_data, fd);
        
        set_x_axis_info(data->axis_info, data->cbuf);

        plot_data(data->windata, data->cbuf, data->axis_info);

        prev_plot_time_ms = millis();
    }

    // TODO: NON va bene per due motivi:
    //       1. loggando ogni DELTA_LOG >> tempo_riempimento_cbuf si introduce del lag nel logging
    //          (dati sono molto granulari a intervalli: durante il tempo in cui non ha loggato si sono persi dei dati)
    //       2. non appena entra il programma viene rallentato molto tutto insieme
    //      --> SOL: Fai il logging con UN ALTRO THREAD (Consumer che legge) (questo è il Producer)
    // write log data into tsv file every DELTA_T_LOG_MS
    // if (millis() > prev_log_time_ms + DELTA_T_LOG_MS) {
    //     log_data_to_tsv(fd, &cbuf);

    //     prev_log_time_ms = millis();
    // }
}

