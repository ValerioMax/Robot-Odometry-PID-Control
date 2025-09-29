#include "uart_utils.h"

static FILE mystdout = FDEV_SETUP_STREAM(UART_putchar_printf, NULL, _FDEV_SETUP_WRITE);

void UART_init(uint16_t baud) {
    // calcola MYUBRR relativo al baudrate desiderato
    uint16_t myubrr = F_CPU/16/baud - 1;

    // Setta baudrate: spezza MYUBRR in due registri 8 bit
    UBRR0H = (uint8_t) (myubrr >> 8);
    UBRR0L = (uint8_t) myubrr;

    // Setta frame size: 
    // Mettere i bit UCSZ01 e UCSZ00 a 1 contemporaneamente dice all'arduino di usare 8-bit data frame
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // 8 bit data frame
    
    // Enable TX e RX: ...
    //  RXEN0: abilita la UART a ricevere dati
    //  TXEN0: abilita la UART a trasmettere dati
    //  RXCIE0: abilita il trigger di interrupt quando la UART ha ricevuto 1 byte completo (riempito il buffer di ricezione) con successo 
    UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0); // enable TX, RX, RX complete interrupt

    sei();
}

void UART_putchar(uint8_t c) {
    // aspetta finché il buffer di trasmissione è pieno
    // (si stanno ancora trasmettendo dati precedentemente scritti)
    while ( !(UCSR0A & (1 << UDRE0)) );

    // quando il buffer di trasmissione è vuoto ci scrive il carattere
    UDR0 = c;
}

void UART_putstring(uint8_t *buf) {
    while (*buf) {
        UART_putchar(*buf);
        ++buf;
    }
}

// BLOCKING getchar()
uint8_t UART_getchar_blocking() {
    // aspetta finché il buffer di ricezione (1 byte) è in fase di riempimento
    // (devono ancora essere scritti tutti i bit del char in arrivo)
    while ( !(UCSR0A & (1<<RXC0)) );

    // quando il buffer di ricezione è pieno ci legge il carattere
    return UDR0;
}

// BLOCKING getstring()
// legge stringa fino al successivo '\n' '\r' o 0
// ritorna la size letta
uint8_t UART_getstring_blocking(uint8_t *buf) {
    uint8_t *b0 = buf;

    while (1) {
        uint8_t c = UART_getchar_blocking();
        *buf = c;
        ++buf;

        if (c == 0)
            return buf - b0;
        if (c == '\n' || c == '\r') {
            *buf = 0; // termina la stringa forzatamente con '\0'
            ++buf;
            return buf - b0;
        }
    }
}

volatile char recv_byte;
volatile uint8_t rx_buffer[MAX_BUF_SIZE];
volatile int rx_byte_ready = 0;
volatile int rx_string_ready = 0;
volatile int rx_idx = 0;

ISR(USART0_RX_vect) {
    recv_byte = UDR0;
    rx_byte_ready = 1;

    // overflow control (this is critical, espacially for WASD continous char receiving!!!)
    if (rx_idx >= MAX_BUF_SIZE - 1)
        return ;

    rx_buffer[rx_idx++] = recv_byte;

    if (recv_byte == '\0')
        rx_string_ready = 1;
    else if (recv_byte == '\n' || recv_byte == '\r') {
        rx_buffer[rx_idx] = '\0';
        rx_string_ready = 1;
        rx_idx = 0;
    }
}

// NON BLOCKING, POLLING, ON INTERRUPT getchar()
uint8_t UART_getchar_non_blocking() {
    if (rx_byte_ready) {
        cli();
        rx_byte_ready = 0;
        sei();

        return recv_byte;
    }

    return 0;
}

// NON BLOCKING, POLLING, ON INTERRUPT getstring()
uint8_t *UART_getstring_non_blocking() {
    if (rx_string_ready) {
        cli();
        rx_string_ready = 0;
        sei();

        return (uint8_t *) rx_buffer;
        //memcpy(buf, rx_buffer, MAX_BUF_SIZE);
    }

    return NULL;
}

// this function is called by printf as a stream handler
int UART_putchar_printf(char var, FILE *stream) {
    // translate \n to \r for br@y++ terminal
    if (var == '\n') UART_putchar('\r');
    UART_putchar(var);
    return 0;
}

void printf_init() {
    stdout = &mystdout;
}