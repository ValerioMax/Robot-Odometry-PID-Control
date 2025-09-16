#ifndef THREAD_READ_H
# define THREAD_READ_H

# include "shared_types.h"
# include "graphics_utils.h"
# include "data_utils.h"
# include "circular_buffer.h"
# include "serial.h"

//# define DATA_FILE_NAME "tsv/data_linear.tsv"
# define DATA_FILE_NAME "tsv/data_sine.tsv"
//# define DATA_FILE_NAME "tsv/data_sine_floats.tsv"

# define DELTA_T_PLOT_MS 1
# define DELTA_T_LOG_MS 3000

# define TSV_HEADER "pos\tp_trg\tp_err\trpm\tr_trg\tr_err"

void *thread_read(void *args);

#endif