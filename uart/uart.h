#ifndef UART_H
#define UART_H

extern "C" {
    #include "single_wire_UART.h"
}

#define printc SW_UART_Transmit

void prints(char* str)
{
  while( *str != '\0' )
  {
    if( !READ_FLAG(SW_UART_status, SW_UART_TX_BUFFER_FULL) )
    {
      SW_UART_Transmit((uint8_t) *str++);
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
