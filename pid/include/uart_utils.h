#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

#define BAUD 19600
#define MYUBRR (F_CPU/16/BAUD-1) // meno precisa
//#define MYUBRR ((F_CPU + BAUD * 8) / (BAUD * 16) - 1) // più precisa

int UART_putchar_printf(char var, FILE *stream);
void printf_init();

void UART_init();
void UART_putchar(uint8_t c);
uint8_t UART_getchar();
uint8_t UART_getstring(uint8_t* buf);
void UART_putstring(uint8_t *buf);
