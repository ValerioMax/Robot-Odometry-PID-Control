#ifndef CIRCULAR_BUFFER_H
# define CIRCULAR_BUFFER_H

# include <stdio.h>
# include <stdlib.h>

# include "shared_types.h"

typedef struct {
    t_sample *samples; // list of samples (actual buffer)
    int head;
    int tail;
    int size;
} CircularBuffer;

void    cb_init(CircularBuffer *cb, int size);
void    cb_print(CircularBuffer *cb);
int     cb_is_empty(CircularBuffer *cb);
int     cb_is_full(CircularBuffer *cb);
void    enqueue(CircularBuffer *cb, t_sample value);
void    dequeue(CircularBuffer *cb);
void    cb_destroy(CircularBuffer *cb);

#endif
