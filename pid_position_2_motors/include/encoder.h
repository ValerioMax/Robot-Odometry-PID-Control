//#ifndef ENCODER_H
//#define ENCODER_H

#include "shared_types.h"
#include "peripherals_utils.h"

typedef struct {
    int pos;
    //int rpm;
    int dir;

    int port; // port of pin_a and pin_b (we assume they are in same port)
    int pin_a;
    int pin_b;

    // forse dovrebbero stare in Motor
    int target_pos;
    int target_rpm;
    int err_pos;
    int err_rpm;
} Encoder;

void    Encoder_init(Encoder *encoder, int pin_a, int pin_b, int port);
void    Encoder_read(Encoder *encoder);
// TODO
/*
void    Encoder_get_rpm
void    Encoder_get_dir
void    Encoder_get_target_pos(Encoder *encoder);
void    Encoder_get_target_rpm(Encoder *encoder);
void    Encoder_get_error_pos(Encoder *encoder);
void    Encoder_get_error_rpm(Encoder *encoder);
*/


//#endif