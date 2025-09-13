#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include <stdio.h>
#include <stdint.h>

#include <stdlib.h>
#include <math.h>

// TODO: è meglio usare una enum forse
// i will use them in the code to make things clearer
#define PORT_A 1
#define PORT_B 2
#define PORT_C 3
#define PORT_D 4
#define PORT_E 5
#define PORT_F 6
#define PORT_G 7
#define PORT_H 8
#define PORT_I 9
#define PORT_J 10
#define PORT_K 11

#define PCINT_0 0
#define PCINT_1 1
#define PCINT_2 2

// log on UART
//#define DELTA_T_FILL_BUF_MS 2000
#define DELTA_T_LOG_MS 2000

#define MAX_BUF_SIZE 200

#endif


