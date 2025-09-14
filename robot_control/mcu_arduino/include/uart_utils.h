#ifndef UART_UTILS_H
#define UART_UTILS_H

#include "shared_types.h"

#define BAUD 19600
#define MYUBRR (F_CPU/16/BAUD-1) // meno precisa
//#define MYUBRR ((F_CPU + BAUD * 8) / (BAUD * 16) - 1) // più precisa

int     UART_putchar_printf(char var, FILE *stream);
void    printf_init();

void    UART_init();
void    UART_putchar(uint8_t c);
uint8_t UART_getchar_polling();
uint8_t UART_getstring_polling(uint8_t* buf);
uint8_t *UART_getstring_int();
void    UART_putstring(uint8_t *buf);

#endif
