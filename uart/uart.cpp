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
uint8_t uart_readable() {
    return (UART_RX_BUFFER_SIZE + UART_Rx_head - UART_Rx_tail) % UART_RX_BUFFER_SIZE;
}


/*! \brief  Get the transmission buffer element count.
 *
 *  This function returns the number of characters
 *  ready to be sent from the transmission buffer.
 *
 *  \return Number of characters in the Tx buffer.
 */
uint8_t uart_writable() {
    return (UART_TX_BUFFER_SIZE + UART_Tx_head - UART_Tx_tail) % UART_TX_BUFFER_SIZE;
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


/*! \brief  Flush the Rx buffer.
 *
 *  This function clears the Rx buffer
 *  of any data it contains.
 */
void uart_rx_flush() {
    UART_Rx_head = UART_Rx_tail;
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


/*! \brief  Transmit one byte.
 *
 *  This function transmits one byte of data
 *  by putting it in the Tx buffer. The transmission
 *  is then initiated to start the interrupt handlers.
 */
void uart_putc(const char c) {
    uint8_t i = (UART_Tx_head + 1) % UART_TX_BUFFER_SIZE;

    // wait for space in the Tx buffer
    while (i == UART_Tx_tail);

    UART_Tx_buffer[UART_Tx_head] = c;
    UART_Tx_head = i;
    uart_transmit();
}


/*! \brief  Receive num bytes.
 *
 *  This function receives num bytes of data,
 *  putting them in the str pointer.
 *
 *  If no data is available in the Rx buffer,
 *  the function waits until any byte is received.
 */
void uart_gets(char * str, uint8_t num) {
    uint8_t i = 0;
    while (i < num) {
        *str++ = uart_getc();
        i++;
    }
}


/*! \brief  Transmit a string.
 *
 *  This function transmits the str until
 *  a terminating character is encountered.
 */
void uart_puts(const char * str) {
    while (*str) {
        uart_putc(*str++);
    }
}


/*! \brief  Transmit a string.
 *
 *  This function transmits the str (contained
 *  in the program memory) until a terminating
 *  character is encountered.
 */
void uart_puts_p(const char * str) {
    char c;
    while (c = pgm_read_byte(str++)) {
        uart_putc(c);
    }
}
