#ifndef DATA_UTILS_H
# define DATA_UTILS_H

# include <sys/time.h>
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
# include <string.h>

# include "shared_types.h"
# include "circular_buffer.h"

//# define DATA_FILE_NAME "tsv/data_linear.tsv"
# define DATA_FILE_NAME "tsv/data_sine.tsv"
//# define DATA_FILE_NAME "tsv/data_sine_floats.tsv"

# define READ_BUFFER_SIZE 255

//void sample_data_init(t_sample *sample_data[], int len);
//void free_sample_data(t_sample *sample_data[], int len);
//void get_data_from_tsv(t_sample *sample_data[], int fd);
//void get_axis_info(t_sample *sample_data[], t_info *axis_info);
//void fill_one_sample(t_sample *sample_data, char *line);

void timer_init();
long long millis();

void set_axis_info(CircularBuffer *cbuf, t_info *axis_info);
void fill_one_sample(CircularBuffer *cbuf, char *line);

#endif