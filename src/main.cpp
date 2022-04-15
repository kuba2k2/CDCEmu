#include "main.h"

#include <uart.h>
#include <util/delay.h>

#include "can.h"
#include "gpio.h"
#include "radio.h"
#include "timers.h"
#include "bluetooth.h"

uint8_t uart_rx_count = 0;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int main() {
	uart_enable();
	uart_puts_P("CDCEmu v" VERSION_STR);
	uart_nl();
	timer_start();
	can_init();

	// main application loop
	while (true) {
		// process incoming & outgoing CAN frames
		can_receive_all();
		can_send_all();

		// tick the track timer
		radio_tick();

		// update LED states
		led_update_all();

#if CONFIG_FEAT_BT
		// check UART availability
		uint8_t readable = uart_readable();
		if (readable >= 2) {
			timer_reset(TIMER_BUFFER_FLUSH); // keep the buffer for a bit more time
			char command[3];
			uart_gets(command, 2);
			command[2] = 0;
			bt_parse_data(command);
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
#endif

		// give the CPU some time
		_delay_ms(1);
	}
}
#pragma clang diagnostic pop
