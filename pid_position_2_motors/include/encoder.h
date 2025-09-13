//#ifndef ENCODER_H
//#define ENCODER_H

#include "shared_types.h"
#include "peripherals_utils.h"

typedef struct {
    int pin_a;
    int pin_b;
    int port; // port of pin_a and pin_b (we assume they are in same port)

    int pos;        // current measured rpm
    int pos_prev;   // only used to compute rpm
    int rpm;        // current measured rpm
    int dir;        // current measured dir
} Encoder;

void    Encoder_init(Encoder *encoder, int pin_a, int pin_b, int port);
void    Encoder_read(Encoder *encoder);
void    Encoder_update_rpm(Encoder *encoder, int time_passed_ms);

//#endif