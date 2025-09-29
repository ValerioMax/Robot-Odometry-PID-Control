#ifndef ENCODER_H
#define ENCODER_H

#include "shared_types.h"
#include "peripherals_utils.h"

#define TICKS_PER_REV 2200 // measured empirically

typedef struct {
    int pin_a;
    int pin_b;
    int port; // port of pin_a and pin_b (we assume they are in same port)

    int32_t pos;        // current measured rpm
    int32_t pos_prev;   // only used to compute rpm and for odometry
    int32_t pos_diff;   // ticks done since last reading
    int rpm;            // current measured rpm
    int dir;            // current measured dir
} Encoder;

void    Encoder_init(Encoder *encoder, int pin_a, int pin_b, int port);
void    Encoder_read(Encoder *encoder);
void    Encoder_update_rpm(Encoder *encoder, uint64_t time_passed_us);

#endif