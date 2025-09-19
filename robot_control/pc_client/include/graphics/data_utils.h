#ifndef DATA_UTILS_H
# define DATA_UTILS_H

# include <sys/time.h>
# include <fcntl.h>
# include <string.h>

# include "shared_types.h"
# include "circular_buffer.h"

# define READ_BUFFER_SIZE 255
# define NUM_TOKENS 7 // number of tokens we expect from arduino

//void get_data_from_tsv(t_sample *sample_data[], int fd);

void timer_init();
long long millis();

void set_x_axis_info(t_info *axis_info, CircularBuffer *cbuf);
void set_y_axis_info(t_info *axis_info, int y_max);
void fill_one_sample(CircularBuffer *cbuf, char *line);
void log_data_to_tsv(int fd, CircularBuffer *cbuf);

#endif