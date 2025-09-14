// sshared types on multiple .h files to prevent circual includes
#ifndef SHARED_TYPES_H
# define SHARED_TYPES_H

# include <stdint.h>
# include <stdlib.h>
# include <math.h>
# include <stdio.h>
# include <unistd.h>
# include <errno.h>

typedef struct s_sample {
    int value;
    long long timestamp;
} t_sample;

typedef struct s_info {
    long long time_min;
    long long time_max;
    long long time_range;
    int value_max;
} t_info;

# define NUM_SAMPLES 10
# define MAX_SAMPLE_VALUE 500 // TODO cambiare questo valore costante

# define FRAME_WIDTH 1200
# define FRAME_HEIGHT 700
# define PADDING_X 60
# define PADDING_Y 20
# define PLANE_WIDTH (FRAME_WIDTH - 2 * PADDING_X)
# define PLANE_HEIGHT (FRAME_HEIGHT - 2 * PADDING_Y)
# define ORIGIN_X PADDING_X
# define ORIGIN_Y FRAME_HEIGHT / 2
# define RECT_X (FRAME_WIDTH - 2 * PADDING_X) / 10
# define RECT_Y (FRAME_HEIGHT - 2 * PADDING_Y) / 10

#endif