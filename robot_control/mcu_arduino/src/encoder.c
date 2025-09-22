#include "encoder.h"

Encoder encoder1 = {0};
Encoder encoder2 = {0};

volatile uint8_t external_int_enc1_occurred = 0;
volatile uint8_t external_int_enc2_occurred = 0;

volatile uint8_t previous_portb_state = 0;
volatile int32_t encoder1_pos = 0;
volatile int32_t encoder2_pos = 0;

ISR(PCINT0_vect) {
    uint8_t current_portb_state = PINB;
    uint8_t changed_pins = current_portb_state ^ previous_portb_state; // xor to get different bits

    // filter for only the pins that had a rising edge [its not possible to set rising edge directly on PCINTs]
    uint8_t rising_edge_pins = changed_pins & current_portb_state;
    
    // check if ENCA for encoder 1 changed
    if (rising_edge_pins & ((1 << encoder1.pin_a)))
        external_int_enc1_occurred = 1;

    // check if ENCA for encoder 2 changed
    if (rising_edge_pins & (1 << encoder2.pin_a))
        external_int_enc2_occurred = 1;

    // store the new state for the next interrupt
    previous_portb_state = current_portb_state;
}

void Encoder_init(Encoder *encoder, int pin_a, int pin_b, int port) {
    encoder->pin_a = pin_a;
    encoder->pin_b = pin_b;
    encoder->port = port;
    encoder->pos = 0;
    encoder->pos_prev = 0;
    encoder->rpm = 0;
    encoder->dir = 0;

    // initialize external interrupt PCINT0
    const uint8_t mask = (1 << pin_a) | (1 << pin_b);
    external_int_PCINT_init(mask, port);
}

void Encoder_read(Encoder *encoder) {
    const int mask = (1 << encoder->pin_b);

    // read ENCB pin state
    int b = !((PINB & mask) == 0); // not per via del pullup // little hardcoded for PORTB

    if (b) {
        encoder->pos++;
        encoder->dir = 1;
    }
    else {
        encoder->pos--;
        encoder->dir = -1;
    }       
}

void Encoder_update_rpm(Encoder *encoder, int time_passed_ms) {
    // TODO: maybe use abs() cause dir is already specified

    // velocity in tick/s
    float v_tick = (encoder->pos - encoder->pos_prev) / (time_passed_ms / 1000.0);

    // velocity in rpm
    encoder->rpm = (v_tick * 60.0) / TICKS_PER_REV;

    encoder->pos_diff = encoder->pos - encoder->pos_prev; // used for odometry

    encoder->pos_prev = encoder->pos;

    //printf("v_tick %ld, encrpm %ld\n", (long) v_tick, (long) encoder->rpm);
}

