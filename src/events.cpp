// Copyright (c) Kuba Szczodrzyński 2022-4-15.

#include "events.h"
#include "bluetooth.h"
#include "data.h"
#include "gpio.h"
#include "utils.h"

void on_bsi_ignition(bool state) {
	if (state) {
#if CONFIG_BSI_ON_ANALOG_PWR
		analog_enable(true);
#endif
#if CONFIG_BSI_ON_BT_PAIRING && !CONFIG_BSI_ON_BT_CONNECT
		bt_pairing_enable(); // will not disconnect if connected
#endif
#if CONFIG_BSI_ON_BT_CONNECT
		bt_reset_previous_connection(); // re-enable automatic reconnection
		bt_reconnect();					// will not disconnect if connected
#endif
	} else {
#if CONFIG_BSI_OFF_ANALOG_PWR
		analog_enable(false);
#endif
	}
}

void on_sleep_mode() {
	analog_enable(false);
	enter_sleep();
}

// radio enabled + radio playing, when called simultaneously, will try to play AUX music twice
// this is handled by setting the AUX playback state in DATA() instantly after requesting button presses,
// and disabling AUX signal detection until the action completes

void on_radio_enabled(bool state) {
	if (state) {
#if CONFIG_RADIO_ON_ANALOG_PWR
		analog_enable(true);
#endif
#if CONFIG_RADIO_ON_BT_PAIRING && !CONFIG_RADIO_ON_BT_CONNECT
		bt_pairing_enable(); // will not disconnect if connected
#endif
#if CONFIG_RADIO_ON_BT_CONNECT
		bt_reset_previous_connection(); // re-enable automatic reconnection
		bt_reconnect();					// will not disconnect if connected
#endif
#if CONFIG_BT_CONN_RADIO_EN_PLAY
		if (DATA(BT_CONNECTED))
			bt_music_play();
#endif
#if CONFIG_RADIO_ON_AUX_PLAY
		aux_play();
#endif
	} else {
#if CONFIG_RADIO_OFF_BT_PAUSE
		bt_music_stop();
#endif
#if CONFIG_RADIO_OFF_AUX_PAUSE
		aux_pause();
#endif
#if CONFIG_RADIO_OFF_BT_DISCONNECT
		bt_disconnect();
#endif
#if CONFIG_RADIO_OFF_BT_PAIRING_OFF
		bt_pairing_disable();
#endif
#if CONFIG_RADIO_OFF_ANALOG_PWR
		analog_enable(false);
#endif
	}
}

void on_radio_playing(bool state) {
	if (state) {
#if CONFIG_RADIO_PLAY_ANALOG_PWR
		analog_enable(true);
#endif
#if CONFIG_RADIO_PLAY_BT_PAIRING && !CONFIG_RADIO_PLAY_BT_CONNECT
		bt_pairing_enable(); // will not disconnect if connected
#endif
#if CONFIG_RADIO_PLAY_BT_CONNECT
		bt_reset_previous_connection(); // re-enable automatic reconnection
		bt_reconnect();					// will not disconnect if connected
#endif
#if CONFIG_BT_CONN_RADIO_SRC_PLAY
		if (DATA(BT_CONNECTED))
			bt_music_play();
#endif
#if CONFIG_RADIO_PLAY_AUX_PLAY
		aux_play();
#endif
	} else {
#if CONFIG_RADIO_PAUSE_BT_PAUSE
		bt_music_stop();
#endif
#if CONFIG_RADIO_PAUSE_AUX_PAUSE
		aux_pause();
#endif
#if CONFIG_RADIO_PAUSE_BT_DISCONNECT
		bt_disconnect();
#endif
#if CONFIG_RADIO_PAUSE_BT_PAIRING_OFF
		bt_pairing_disable();
#endif
#if CONFIG_RADIO_PAUSE_ANALOG_PWR
		analog_enable(false);
#endif
	}
}

void on_bt_connected(bool state) {
	if (!state) {
#if CONFIG_BT_DISCONN_PAIRING
		bt_pairing_enable();
#endif
		return;
	}
#if CONFIG_BT_CONN_RADIO_EN_PLAY
	if (DATA(RADIO_ENABLED)) {
		bt_music_play();
		return;
	}
#elif CONFIG_BT_CONN_RADIO_SRC_PLAY
	if (DATA(RADIO_PLAYING)) {
		bt_music_play();
		return;
	}
#endif
}

void on_bt_playing(bool state) {
#if CONFIG_BT_PLAY_AUX_PAUSE
	if (state)
		aux_pause();
#endif
}

void on_aux_playing(bool state) {
#if CONFIG_AUX_PLAY_BT_PAUSE
	if (state)
		bt_music_stop();
#endif
}

void on_req_track_next() {
#if CONFIG_FEAT_BT_RADIO_CONTROLS
	bt_track_next();
#endif
#if CONFIG_FEAT_AUX_RADIO_CONTROLS
	aux_next();
#endif
}

void on_req_track_previous() {
#if CONFIG_FEAT_BT_RADIO_CONTROLS
	bt_track_prev();
#endif
}

void on_req_track_ff() {
#if CONFIG_FEAT_BT_RADIO_CONTROLS
	bt_track_ff();
#endif
}

void on_req_track_rew() {
#if CONFIG_FEAT_BT_RADIO_CONTROLS
	bt_track_rew();
#endif
}
