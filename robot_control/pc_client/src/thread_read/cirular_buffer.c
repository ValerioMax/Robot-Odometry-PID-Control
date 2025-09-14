#include "circular_buffer.h"

void cb_init(CircularBuffer* cb, int size) {
    cb->samples = (t_sample *) malloc(sizeof(t_sample) * size);
    cb->head = 0;
    cb->tail = 0;
    cb->size = size;
}

void cb_print(CircularBuffer *cb) {
    for (int i = 0; i < cb->size; i++) {
        int time_stamp = cb->samples[cb->head + i].timestamp;
        int value = cb->samples[cb->head + i].value;
        printf("s%d(%d, %d) ", i, time_stamp, value);
    }
    printf("\n");
}

int is_empty(CircularBuffer* cb) {
    return cb->head == cb->tail;
}

int is_full(CircularBuffer* cb) {
    return ((cb->tail + 1) % cb->size) == cb->head;
}

void enqueue(CircularBuffer* cb, t_sample value) {
    if (is_full(cb)) {
        printf("Buffer is full. Cannot enqueue\n");
        return;
    }
    cb->samples[cb->tail] = value;
    cb->tail = (cb->tail + 1) % cb->size;
    //printf("Enqueued: %d\n", value);
}

void dequeue(CircularBuffer* cb) {
    if (is_empty(cb)) {
        printf("Buffer is empty. Cannot dequeue\n");
        return ;
    }
    //t_sample value = cb->sample[cb->head];
    cb->head = (cb->head + 1) % cb->size;
    //printf("Dequeued: %d\n", value);
}

void cb_destroy(CircularBuffer* cb) {
    free(cb->samples);
}
