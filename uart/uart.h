#ifndef UART_H
#define UART_H

#include <stdint.h>

extern "C" {
    void uart_enable();
    void uart_disable();
    void uart_putc(const char c);
}

uint8_t uart_readable();
char uart_peek();
char uart_getc();
//void uart_puts(const char * str);
//void uart_gets(char * str, uint8_t num);

bool uart_busy();
void uart_puts(const char* str);

#endif
