// Copyright (c) Kuba Szczodrzyński 2022-4-16.

#if CONFIG_FEAT_BT

#include "bluetooth.h"

#include <avr/pgmspace.h>
#include <uart.h>
#include <util/delay.h>

#include "data.h"
#include "gpio.h"
#include "timers.h"

// internal event helpers
void bt_set_connected(bool state);
void bt_set_playing(bool state);

// action queue
#define ACTIONS_MAX 8
ok_action pending_actions[ACTIONS_MAX];
uint8_t pending_actions_num = 0;
bool action_running = false;

void bt_parse_data(char cmd[3]) {
	// any command means BT is active
	analog_enable(true);
	if (cmd[0] == 'O') {
		// got OK or ON - can run an action
		_delay_ms(100); // apparently it doesn't like commands right after the OK response
		bt_queue_next();
		return;
	}
	if (!strcmp_P(cmd, PSTR(CONFIG_BT_AT_CONN_START))) {
		bt_set_connected(true);
	} else if (!strcmp_P(cmd, PSTR(CONFIG_BT_AT_CONN_END))) {
		bt_set_connected(false);
	} else if (!strcmp_P(cmd, PSTR(CONFIG_BT_AT_PLAY_START))) {
		bt_set_playing(true);
	} else if (!strcmp_P(cmd, PSTR(CONFIG_BT_AT_PLAY_END))) {
		bt_set_playing(false);
	} else if (!strcmp_P(cmd, PSTR(CONFIG_BT_AT_CONN_YES))) {
		bt_set_connected(true);
	} else if (!strcmp_P(cmd, PSTR(CONFIG_BT_AT_CONN_NO))) {
		bt_set_connected(false);
	} else if (!strcmp_P(cmd, PSTR(CONFIG_BT_AT_PLAY_YES))) {
		bt_set_playing(true);
	} else if (!strcmp_P(cmd, PSTR(CONFIG_BT_AT_PLAY_NO))) {
		bt_set_playing(false);
	} else if (!strcmp_P(cmd, PSTR(CONFIG_BT_AT_PLAY_DISCONN))) {
		bt_set_connected(false);
	}
}

void bt_queue_append(ok_action action) {
	// do not clear the queue for another second
	timer_reset(TIMER_BT_TICK);
	if (!DATA(ANALOG_POWER) || action_running) {
		// enqueue the action if BT is powered off
		// or an action is currently pending a result
		if (pending_actions_num >= ACTIONS_MAX)
			return;
		pending_actions[pending_actions_num++] = action;
		return;
	}
	// block further actions
	action_running = true;
	switch (action) {
		case CONNECT_LAST_DEVICE:
			uart_puts_P(CONFIG_BT_AT_CONNECT);
			break;
		case DISCONNECT:
			uart_puts_P(CONFIG_BT_AT_DISCONNECT);
			break;
		case PAIRING_ON:
			uart_puts_P(CONFIG_BT_AT_PAIR_ON);
			break;
		case PAIRING_OFF:
			uart_puts_P(CONFIG_BT_AT_PAIR_OFF);
			break;
		case MUSIC_PLAY:
			uart_puts_P(CONFIG_BT_AT_PLAY);
			break;
		case MUSIC_STOP:
			uart_puts_P(CONFIG_BT_AT_STOP);
			break;
		case TRACK_NEXT:
			uart_puts_P(CONFIG_BT_AT_NEXT);
			break;
		case TRACK_PREV:
			uart_puts_P(CONFIG_BT_AT_PREV);
			break;
		case TRACK_FF:
			uart_puts_P(CONFIG_BT_AT_FF);
			break;
		case TRACK_REW:
			uart_puts_P(CONFIG_BT_AT_REW);
			break;
		case SHUTDOWN:
			uart_puts_P(CONFIG_BT_AT_SHUTDOWN);
			break;
		case ANALOG_POWER_OFF:
			analog_enable(false);
			return;
		default:
			data[action & 0b11111] = action & DATA_TRUE >> 5;
			bt_queue_next();
			return;
	}
	uart_nl();
}

void bt_queue_next() {
	action_running = false;
	if (pending_actions_num > 0) { // check pending actions
		ok_action action = pending_actions[0];
		pending_actions_num--;
		// move all subsequent actions to the left
		for (uint8_t i = 0; i < pending_actions_num; i++) {
			pending_actions[i] = pending_actions[i + 1];
		}
		bt_queue_append(action); // run the first action
	}
}

void bt_queue_clear() {
	if (pending_actions_num || action_running) {
		debug_puts_P("actions timeout ");
		debug_putc(pending_actions_num + action_running + '0');
		debug_nl();
		action_running = false;
		pending_actions_num = 0;
	}
}

#endif
