#include "uart.h"

extern volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
extern volatile uint16_t rx_idx;
extern volatile uint16_t rx_ready;

int main() {
    uart_init();
    
    while (1) {
        if (rx_ready) {
            //uart_putline((const char *) rx_buffer);
            uart_puts("prova\n");
            rx_ready = 0;
        }
    }
    return 0;
}