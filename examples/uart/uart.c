#include "uart_utils.h"

#define MAX_BUF 256

// simple echo server
int main() {
    UART_init(19600);
    printf_init();

    //UART_putstring((uint8_t *) "scrivi qualcosa, lo ripeto\n");
    printf("scrivi qualcosa, lo ripeto\n");

    uint8_t buf[MAX_BUF];
    
    while(1) {
        //UART_getstring(buf);
        //UART_putstring((uint8_t*) "ricevuto\n");
        //UART_putstring(buf);

        UART_getstring(buf);
        printf("ricevuto\n");
        printf("%s", buf);
    }
}