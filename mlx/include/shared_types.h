// sshared types on multiple .h files to prevent circual includes
#ifndef SHARED_TYPES_H
# define SHARED_TYPES_H

# include <math.h>

# define NUM_SAMPLES 50

typedef struct s_sample {
    int value;
    long timestamp;
} t_sample;

#endif