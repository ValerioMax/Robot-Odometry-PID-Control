#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define DATA_FILE_NAME "data.tsv"
#define NUM_ROWS 50
#define NUM_COLUMNS 2 // value, timestamp

void data_init(int **sample_data);
void get_data_from_tsv(int **sample_data, const char *filename);