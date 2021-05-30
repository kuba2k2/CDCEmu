#include "uart.h"

extern "C" {
    #include "single_wire_UART.h"
}

/*! \brief  Get the reception buffer element count.
 *
 *  This function returns the number of characters
 *  available to read from the reception buffer.
 *
 *  \return Number of characters available.
 */
uint8_t uart_available() {
    return (UART_RX_BUFFER_SIZE + UART_Rx_head - UART_Rx_tail) % UART_RX_BUFFER_SIZE;
}

/*! \brief  Receive one byte.
 *
 *  This function receives one byte of data,
 *  but does not increment the reception position.
 *
 *  If no data is available in the Rx buffer,
 *  the function returns 0.
 *
 *  \return Data received.
 */
char uart_peek() {
    if (UART_Rx_head == UART_Rx_tail) {
        return '\0';
    }
    return UART_Rx_buffer[UART_Rx_tail];
}

/*! \brief  Receive one byte.
 *
 *  This function receives one byte of data
 *  by accessing the Rx buffer.
 *
 *  If no data is available in the Rx buffer,
 *  the function waits until any byte is received.
 *
 *  \return Data received.
 */
char uart_getc() {
    while (UART_Rx_head == UART_Rx_tail);

    char c = UART_Rx_buffer[UART_Rx_tail];
    UART_Rx_tail = (UART_Rx_tail + 1) % UART_RX_BUFFER_SIZE;
    return c;
}

bool uart_busy() {
    return READ_FLAG(SW_UART_status, SW_UART_TX_BUFFER_FULL);
}

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
