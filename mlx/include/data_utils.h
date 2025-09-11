#ifndef DATA_UTILS_H
# define DATA_UTILS_H
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <errno.h>
# include <string.h>

# include "shared_types.h"

//# define DATA_FILE_NAME "tsv/data_linear.tsv"
# define DATA_FILE_NAME "tsv/data_sine.tsv"
//# define DATA_FILE_NAME "tsv/data_sine_floats.tsv"

# define READ_BUFFER_SIZE 255

void sample_data_init(t_sample *sample_data[]);
void free_sample_data(t_sample *sample_data[]);
void get_data_from_tsv(t_sample *sample_data[], const char *filename);
void get_axis_info(t_sample *sample_data[], t_info *axis_info);

#endif