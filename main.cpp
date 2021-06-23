#include "main.h"
#include <avr/io.h>
#include <util/delay.h>

#include <pcf8574.h>
#include <uart.h>
#include <mcp2515.h>
#include <timers.h>

#include "data.h"
#include "can.h"
#include "radio.h"

uint8_t uart_rx_count = 0;

void ensure_spi() {
    spi_begin();
}

void ensure_i2c() {
    i2c_init();
}

int main() {
    uart_enable();
    timer_start();
    can_init();

    while (1) {
        can_receive_all();
        can_send_all();

        radio_tick();

        uint8_t readable = uart_readable();

        if (readable >= 2) {
            char command[3];
            uart_gets(command, 2);
            command[2] = 0;
            if (!strcmp_P(command, PSTR("MA"))) {
                uart_puts_P("bt paused");
            }
            else if (!strcmp_P(command, PSTR("MB"))) {
                uart_puts_P("bt playing");
            }
        }

        // discard any newline characters
        if (uart_peek() == '\r' || uart_peek() == '\n')
            uart_getc();

        // flush any leftover bytes
        if (timer_check(TIMER_BUFFER_FLUSH, T_MS(1000))) {
            if (readable && uart_rx_count) {
                uart_rx_flush();
                readable = 0;
            }
            uart_rx_count = readable;
        }

        _delay_ms(1);
    }
    return 0;
}
