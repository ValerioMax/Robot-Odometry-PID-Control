#ifndef DATA_UTILS_H
# define DATA_UTILS_H
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <errno.h>
# include <stdio.h>
# include <string.h>

# include "shared_types.h"

#define DATA_FILE_NAME "data.tsv"

#define READ_BUFFER_SIZE 255

void data_init(t_sample *sample_data[]);
void get_data_from_tsv(t_sample *sample_data[], const char *filename);

#endif