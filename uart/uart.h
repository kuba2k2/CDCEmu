#ifndef UART_H
#define UART_H

extern "C" {
    #include "single_wire_UART.h"

    void uart_enable();
    void uart_disable();
    void uart_putc(const char c);
    char uart_getc();
}

//void uart_puts(const char * str);
//void uart_gets(char * str, uint8_t num);

void uart_puts(const char* str)
{
  while( *str != '\0' )
  {
    if( !READ_FLAG(SW_UART_status, SW_UART_TX_BUFFER_FULL) )
    {
      uart_putc(*str++);
    }
  }
}

#if DEBUG
    #include <stdio.h>
    #define print printf
#else
    #define print
#endif

#endif
