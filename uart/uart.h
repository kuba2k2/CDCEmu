#pragma once

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
void uart_nl();

#define uart_puts_P(str) uart_puts_p(PSTR(str))

#if DEBUG
#define debug_putc(c) uart_putc(c)
#define debug_puts(s) uart_puts(s)
#define debug_puts_P(str) uart_puts_P(str)
#define debug_nl() uart_nl()
#else
#define debug_putc(c)
#define debug_puts(s)
#define debug_puts_P(str)
#define debug_nl()
#endif
