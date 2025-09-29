#include "circular_buffer.h"

void cb_init(CircularBuffer* cb, int size) {
    cb->samples = (t_sample *) malloc(sizeof(t_sample) * size);
    cb->head = 0;
    cb->tail = 0;
    cb->size = size;
}

void cb_print(CircularBuffer *cb) {
    int num_elems = cb_get_num_elements(cb);

    for (int i = 0; i < num_elems; i++) {
        int idx = cb_get_idx(cb, i);

        long long time_stamp = cb->samples[idx].timestamp;
        int value = cb->samples[idx].pos;
        printf("e%d(%lld, %d) ", i, time_stamp, value);
    }
    printf("\n");
}

int cb_is_empty(CircularBuffer* cb) {
    return cb->head == cb->tail;
}

int cb_is_full(CircularBuffer* cb) {
    return ((cb->tail + 1) % cb->size) == cb->head;
}

void cb_enqueue(CircularBuffer* cb, t_sample value) {
    if (cb_is_full(cb)) {
        printf("Buffer is full. Cannot enqueue\n");
        return;
    }
    cb->samples[cb->tail] = value;
    cb->tail = (cb->tail + 1) % cb->size;
    //printf("Enqueued: %d\n", value);
}

void cb_dequeue(CircularBuffer* cb) {
    if (cb_is_empty(cb)) {
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

int cb_get_num_elements(CircularBuffer *cb) {
    return (cb->tail - cb->head + cb->size) % cb->size;
}

int cb_get_idx(CircularBuffer *cb, int idx) {
    return (cb->head + idx) % cb->size;
}

int cb_get_idx_last_elem(CircularBuffer *cb) {
    return (cb->tail - 1 + cb->size) % cb->size;
}