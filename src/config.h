#pragma once

#define DEBUG CONFIG_DEBUG

#define TRACK_MAX 99

#define STRINGIFY(x)	   #x
#define STRINGIFY_MACRO(x) STRINGIFY(x)
#define VERSION_STR		   STRINGIFY_MACRO(VERSION)

// Single wire UART
#if CONFIG_SWU_PORTA
#define SW_UART_PORT          	PORTA
#define SW_UART_DDR				DDRA
#define SW_UART_PIN				PINA
#elif CONFIG_SWU_PORTB
#define SW_UART_PORT          	PORTB
#define SW_UART_DDR				DDRB
#define SW_UART_PIN				PINB
#elif CONFIG_SWU_PORTC
#define SW_UART_PORT          	PORTC
#define SW_UART_DDR				DDRC
#define SW_UART_PIN				PINC
#elif CONFIG_SWU_PORTD
#define SW_UART_PORT         	PORTD
#define SW_UART_DDR				DDRD
#define SW_UART_PIN				PIND
#endif
#define SW_UART_PIN_NUMBER		CONFIG_SWU_PIN

// I2C master
#if CONFIG_I2C_SDA_PORTA
#define SDA_PORT          	PORTA
#elif CONFIG_I2C_SDA_PORTB
#define SDA_PORT          	PORTB
#elif CONFIG_I2C_SDA_PORTC
#define SDA_PORT          	PORTC
#elif CONFIG_I2C_SDA_PORTD
#define SDA_PORT         	PORTD
#endif
#if CONFIG_I2C_SCL_PORTA
#define SCL_PORT          	PORTA
#elif CONFIG_I2C_SCL_PORTB
#define SCL_PORT          	PORTB
#elif CONFIG_I2C_SCL_PORTC
#define SCL_PORT          	PORTC
#elif CONFIG_I2C_SCL_PORTD
#define SCL_PORT         	PORTD
#endif
#define SCL_PIN	 CONFIG_I2C_SCL_PIN
#define SDA_PIN	 CONFIG_I2C_SDA_PIN

// tinySPI
#if CONFIG_SPI_PORTA
#define SPI_DDR_PORT          	DDRA
#elif CONFIG_SPI_PORTB
#define SPI_DDR_PORT          	DDRB
#elif CONFIG_SPI_PORTC
#define SPI_DDR_PORT          	DDRC
#elif CONFIG_SPI_PORTD
#define SPI_DDR_PORT         	DDRD
#endif
#define USCK_DD_PIN CONFIG_SPI_SCK_PIN
#define DO_DD_PIN CONFIG_SPI_DO_PIN
#define DI_DD_PIN CONFIG_SPI_DI_PIN

// MCP2515
#if CONFIG_MCP_CS_PORTA
#define MCP_CS_DDR          DDRB
#define MCP_CS_PORT			PORTB
#elif CONFIG_MCP_CS_PORTB
#define MCP_CS_DDR          DDRB
#define MCP_CS_PORT         PORTB
#elif CONFIG_MCP_CS_PORTC
#define MCP_CS_DDR          DDRC
#define MCP_CS_PORT         PORTC
#elif CONFIG_MCP_CS_PORTD
#define MCP_CS_DDR          DDRD
#define MCP_CS_PORT         PORTD
#endif
#define MCP_CS_PIN          CONFIG_MCP_CS_PIN
