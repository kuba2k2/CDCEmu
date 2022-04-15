// Copyright (c) Kuba Szczodrzyński 2022-4-15.

#if CONFIG_FEAT_BT

#include "bluetooth.h"

#include <avr/pgmspace.h>
#include <uart.h>

#include "events.h"

bool connected = false;
bool playing = false;

void bt_parse_data(char cmd[3]) {
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

void bt_set_connected(bool state) {
	if (connected == state)
		return;
	connected = state;
	//	on_bt_connected(state);
}

void bt_set_playing(bool state) {
	if (playing == state)
		return;
	playing = state;
	//	on_bt_playing(state);
}

void bt_request_connection_state() {}

void bt_request_playback_state() {}

void bt_connect_last_device() {}

void bt_disconnect() {}

void bt_pairing_enable() {}

void bt_pairing_disable() {}

void bt_music_play_pause() {}

void bt_music_stop() {}

void bt_track_next() {}

void bt_track_prev() {}

void bt_track_ff() {}

void bt_track_rew() {}

#endif
