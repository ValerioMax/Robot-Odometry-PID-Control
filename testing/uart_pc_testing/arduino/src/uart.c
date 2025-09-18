#include "uart.h"

volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_idx = 0;
volatile uint16_t rx_ready = 0;

void uart_init() {
    UBRR0H = (unsigned char)(UBRR_VAL >> 8);
    UBRR0L = (unsigned char)UBRR_VAL;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (3 << UCSZ00);
    sei();
}

ISR(USART0_RX_vect) {
    uint8_t received_byte = UDR0;
    
    rx_buffer[rx_idx] = received_byte;
    rx_idx++;

    if (received_byte == '\n') {
        rx_buffer[rx_idx] = '\0';
        rx_ready = 1;
        rx_idx = 0;
    }
}

void uart_puts(const char* s) {
    while (*s) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = *s++;
    }
    //UDR0 = '\n';
}

void uart_putline(const char* s) {
    while (*s) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = *s;

        if (*s == '\n')
            return;
        
        s++;
    }
}
