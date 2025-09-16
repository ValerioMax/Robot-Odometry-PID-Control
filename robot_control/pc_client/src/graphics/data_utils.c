#include "data_utils.h"

extern long long start_time_ms;

// set start time when called
void timer_init() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    start_time_ms = (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// get elapsed milliseconds since start_time_ms
long long millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long current_time_ms = (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return current_time_ms - start_time_ms;
}

void set_axis_info(CircularBuffer *cbuf, t_info *axis_info) {
    if (!cbuf)
		return ;

    // get time_min and time_max to get y-axis range
	axis_info->time_min = (long long) (cbuf->samples[cbuf->head]).timestamp;
	axis_info->time_max = (long long) (cbuf->samples[cb_get_idx_last_elem(cbuf)]).timestamp;
    axis_info->time_range = abs(axis_info->time_max - axis_info->time_min);

    axis_info->value_max = MAX_SAMPLE_VALUE; // TODO change this value
    //printf("%lf %lf %lf\n", axis_info->time_min, axis_info->time_max, axis_info->time_range);
}

void fill_one_sample(CircularBuffer *cbuf, char *line) {
    if (!line)
        return ;
    //printf("%lld\n", millis());
    //printf("%s\n", line);

    t_sample sample;

    // parse line
    char *token;
    
    token = strtok(line, "\t "); // TODO con "\t " funziona, perche?? (dovrebbe andare anche solo con "\t")
    
    int i = 0;

    while (token) {
        switch (i) {
            // pos
            case 0:
                sample.pos = atoi(token);
                break;
            // target pos
            case 1:
                sample.pos_target = atoi(token);
                break;
            // error pos
            case 2:
                sample.pos_error = atoi(token);
                break;
            // rpm
            case 3:
                sample.rpm = atoi(token);
                break;
            // target rpm
            case 4:
                sample.rpm_target = atoi(token);
                break;
            // error rpm
            case 5:
                sample.rpm_error = atoi(token);
                break;
            default:
                break;
        }
        token = strtok(NULL, "\t ");
        i++;
    }

    sample.timestamp = millis();

    // add element to buffer
    if (cb_is_full(cbuf))
        cb_dequeue(cbuf);
    cb_enqueue(cbuf, sample);
}

void log_data_to_tsv(int fd, CircularBuffer *cbuf) {
    int cbuf_len = cb_get_num_elements(cbuf);

    for (int i = 0; i < cbuf_len; i++) {
        char write_buf[MAX_BUFFER_SIZE];

        int idx = cb_get_idx(cbuf, i);

        t_sample sample = cbuf->samples[idx];

        dprintf(fd, "%lld\t%d\t%d\t%d\t%d\t%d\t%d\n", sample.timestamp,
                                                        sample.pos,
                                                        sample.pos_target,
                                                        sample.pos_error,
                                                        sample.rpm,
                                                        sample.rpm_target,
                                                        sample.rpm_error);
    }
}