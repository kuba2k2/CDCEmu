#include <uart.h>
#include <util/delay.h>

#include "bluetooth.h"
#include "can.h"
#include "data.h"
#include "gpio.h"
#include "radio.h"
#include "timers.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int main() {
	uart_enable();
	uart_puts_P("CDCEmu v" VERSION_STR);
	uart_nl();
	timer_start();
	can_init();

#if CONFIG_FEAT_BT
	// check if Analog is powered on
	_delay_ms(500);
	bt_request_connection_state();
	timer_reset(TIMER_BT_TICK);
	while (!timer_check(TIMER_BT_TICK, T_MS(500))) {
		// give BT 500ms to respond
	}
	// force writing known pin states, disable Analog if BT absent
	analog_enable(uart_readable());
#endif

#if CONFIG_FEAT_HS_BTN
	// release all buttons
	button_set(BTN_A_MASK, 0x00);
#endif

	// main application loop
	while (true) {
		// process incoming & outgoing CAN frames
		can_receive_all();
		can_send_all();

		// tick the track timer
		radio_tick();

		// update LED states
		led_update_all();

#if CONFIG_FEAT_AUXDET || CONFIG_FEAT_HS_BTN
		// perform headset button actions and AUX signal detection
		gpio_update();
#endif

#if CONFIG_FEAT_BT
		// allow BT to do its stuff
		if (DATA(ANALOG_POWER))
			bt_status_tick();

		// check UART availability
		if (uart_readable() >= 2) {
			timer_reset(TIMER_BUFFER_FLUSH); // keep the buffer a bit longer
			char command[3];
			uart_gets(command, 2);
			command[2] = 0;
			bt_parse_data(command);
		}

		// discard all newline characters
		while (uart_peek() == '\r' || uart_peek() == '\n')
			uart_getc();

		// flush any leftover bytes
		if (timer_check(TIMER_BUFFER_FLUSH, T_MS(1000))) {
			/*if (uart_readable()) {
				debug_puts_P("f\r\n");
			}*/
			uart_rx_flush();
		}
#endif

		// give the CPU some time
		_delay_ms(1);
	}
}
#pragma clang diagnostic pop
