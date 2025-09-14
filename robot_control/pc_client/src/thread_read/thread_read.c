#include "thread_read.h"

void* thread_read(void* args) {
    t_windata windata;
    t_info axis_info;
    t_sample *sample_data[NUM_SAMPLES];

    // init window for plotting
    init_window(&windata);
    sample_data_init(sample_data, NUM_SAMPLES);

    int serial_port = uart_init("/dev/ttyACM0", O_RDWR, B19200, 0, 1); // blocking = 0, timeout = 1 --> 1*0.1s = 0.1s 

    while (1) {
        // READ FROM UART
        uart_readline(serial_port);

        get_data_from_tsv(sample_data, DATA_FILE_NAME);
        get_axis_info(sample_data, &axis_info);
        draw_data(sample_data, &(windata.img), &axis_info);

        // draw the image pixels on the window
        mlx_put_image_to_window(windata.mlx, windata.win, windata.img.img, 0, 0);

        // for reasons strings have to be drawn after the rest
        draw_info(windata.mlx, windata.win, &axis_info);
    }
    
    // deallocate dynamic data
    free_sample_data(sample_data, NUM_SAMPLES);

    // Close the port
    close(serial_port);

    return NULL;
}
