#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <avr/pgmspace.h>

extern "C" {
    void uart_enable();
    void uart_disable();
}

uint8_t uart_readable();
uint8_t uart_writable();
char uart_peek();
void uart_rx_flush();

char uart_getc();
void uart_putc(const char c);
void uart_gets(char * str, uint8_t num);
void uart_puts(const char * str);
void uart_puts_p(const char * str);

#define uart_puts_P(str) uart_puts_p(PSTR(str))

#endif
