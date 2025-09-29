// sshared types on multiple .h files to prevent circual includes
#ifndef SHARED_TYPES_H
# define SHARED_TYPES_H

# include <pthread.h>
# include <stdint.h>
# include <stdlib.h>
# include <math.h>
# include <stdio.h>
# include <unistd.h>
# include <errno.h>

// TODO: potrei anche mettere solo pos e rpm e il resto calcolarlo da me senza intasare comunicazione

typedef struct s_sample {
    int pos;
    int pos_target;
    int pos_error;
    int rpm;
    int rpm_target;
    int rpm_error;
    long long timestamp;
} t_sample;

typedef struct s_info {
    long long time_min;
    long long time_max;
    long long time_range;
    int value_max;
} t_info;

# define NUM_SAMPLES 200
# define MAX_SAMPLE_VALUE_DEFAULT 2000 // TODO cambiare questo valore costante

# define MAX_BUFFER_SIZE 256

#endif