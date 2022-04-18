// Copyright (c) Kuba Szczodrzyński 2022-4-15.

#if CONFIG_FEAT_BT

#include "bluetooth.h"

#include <avr/pgmspace.h>
#include <uart.h>

#include "data.h"
#include "events.h"
#include "timers.h"

uint8_t bt_tick_count = 0;
bool was_connected = false;
// value between 0 and CONFIG_BT_RECONNECT_AUX_WAIT
// if 0, AUX is playing
// if CONFIG_BT_RECONNECT_AUX_WAIT, AUX is not playing for a safe period
uint8_t aux_unused_ticks = 0;

void bt_status_tick() {
	if (timer_check(TIMER_BT_TICK, T_MS(1000)))
		bt_tick_count++; // tick count can't be zero after this
	else
		return;

	// actions here run every 1 second
	bt_queue_clear();

#if CONFIG_FEAT_AUXDET && CONFIG_BT_RECONNECT_AUX_WAIT
	if (DATA(AUDIO_PLAYING)) {
		aux_unused_ticks = 0;
	} else if (aux_unused_ticks < CONFIG_BT_RECONNECT_AUX_WAIT) {
		aux_unused_ticks++;
	}
#endif

#if CONFIG_BT_POLL_CONN_STATE
	if ((bt_tick_count + CONFIG_BT_POLL_CONN_STATE / 2) % CONFIG_BT_POLL_CONN_STATE == 0) {
		bt_request_connection_state();
	}
#endif

#if CONFIG_BT_POLL_PLAY_STATE
	if ((bt_tick_count + CONFIG_BT_POLL_PLAY_STATE / 3) % CONFIG_BT_POLL_PLAY_STATE == 0) {
		bt_request_playback_state();
	}
#endif

#if CONFIG_BSI_ON_BT_CONNECT
#define RECONNECT_ITEM DATA(IGNITION)
#elif CONFIG_RADIO_ON_BT_CONNECT
#define RECONNECT_ITEM DATA(RADIO_ENABLED)
#elif CONFIG_RADIO_PLAY_BT_CONNECT
#define RECONNECT_ITEM DATA(RADIO_PLAYING)
#endif

#if CONFIG_BT_RECONNECT_INTERVAL && defined(RECONNECT_ITEM)
	if (bt_tick_count % CONFIG_BT_RECONNECT_INTERVAL == 0 && RECONNECT_ITEM) {
		bt_reconnect();
	}
#endif

	if (bt_tick_count > 60) {
		bt_tick_count %= 60;
	}
}

void bt_reset_previous_connection() {
	was_connected = false;
}

void bt_set_connected(bool state) {
	DATA(BT_PAIRING) = false;
	if (DATA(BT_CONNECTED) == state)
		return;
	DATA(BT_CONNECTED) = state;
	DATA(BT_PLAYING) &= state;
	on_bt_connected(state);
	// disable automatic reconnection, not to perform it after manual disconnection
	was_connected |= state;
}

void bt_set_playing(bool state) {
	if (state)
		DATA(BT_PAIRING) = false;
	if (DATA(BT_PLAYING) == state)
		return;
	DATA(BT_PLAYING) = state;
	on_bt_playing(state);
}

void bt_request_connection_state() {
	uart_puts_P(CONFIG_BT_AT_CONN_STATUS);
	uart_nl();
}

void bt_request_playback_state() {
	uart_puts_P(CONFIG_BT_AT_PLAY_STATUS);
	uart_nl();
}

void bt_reconnect() {
	if (DATA(BT_CONNECTED) || aux_unused_ticks < CONFIG_BT_RECONNECT_AUX_WAIT || was_connected)
		return; // refuse if BT connected, AUX is playing or disconnected manually
	bt_disconnect();
	bt_pairing_enable(); // for some reason it needs pairing mode to reconnect...
	bt_connect_last_device();
}

void bt_connect_last_device() {
	bt_queue_append(CONNECT_LAST_DEVICE);
}

void bt_disconnect() {
	bt_queue_append(DISCONNECT);
}

void bt_shutdown() {
	bt_queue_append(SHUTDOWN);
	bt_queue_append(ANALOG_POWER_OFF);
}

void bt_pairing_enable() {
	if (DATA(BT_CONNECTED))
		return;
	bt_queue_append(PAIRING_ON);
	bt_queue_append((ok_action)((uint8_t)DATA_TRUE | (uint8_t)DATA_BT_PAIRING));
}

void bt_pairing_disable() {
	if (DATA(BT_CONNECTED))
		return;
	bt_queue_append(PAIRING_OFF);
	bt_queue_append((ok_action)((uint8_t)DATA_FALSE | (uint8_t)DATA_BT_PAIRING));
}

void bt_music_play() {
	if (DATA(BT_PLAYING))
		return;
	// WHY doesn't BK8000L return OK after this command ...
	// this command is useless, but the error can probably be ignored
	bt_queue_append(MUSIC_PLAY);
}

void bt_music_stop() {
	if (!DATA(BT_PLAYING))
		return;
	// set playing=false after OK, as the event comes delayed
	bt_queue_append(MUSIC_STOP);
	bt_queue_append((ok_action)((uint8_t)DATA_FALSE | (uint8_t)DATA_BT_PLAYING));
}

void bt_track_next() {
	if (!DATA(BT_PLAYING))
		return;
	bt_queue_append(TRACK_NEXT);
}

void bt_track_prev() {
	if (!DATA(BT_PLAYING))
		return;
	bt_queue_append(TRACK_PREV);
}

void bt_track_ff() {
	if (!DATA(BT_PLAYING))
		return;
	bt_queue_append(TRACK_FF);
}

void bt_track_rew() {
	if (!DATA(BT_PLAYING))
		return;
	bt_queue_append(TRACK_REW);
}

#endif
