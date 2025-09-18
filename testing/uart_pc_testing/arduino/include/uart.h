#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#define F_CPU 16000000UL

#define BAUD 19200
#define UBRR_VAL ((F_CPU / 16UL / BAUD) - 1)
#define RX_BUFFER_SIZE 256

void uart_init();
void uart_puts(const char* s);
void uart_putline(const char* s);
