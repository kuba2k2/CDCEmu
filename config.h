#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG       1

#define MCP_CS_PORT PORTB
#define MCP_CS_PIN  3

#define SCL_PORT    PORTB
#define SCL_PIN     3
#define SDA_PORT    PORTB
#define SDA_PIN     1

#define RX_PORT     PORTB
#define RX_PIN      4
#define TX_PORT     PORTB
#define TX_PIN      4

#define PCF_ADDR    0x22

#define UART_BAUD_RATE   115200L


#define PU_RX B,4
#define PU_TX B,4
#define PU_BAUD_RATE UART_BAUD_RATE

#endif
