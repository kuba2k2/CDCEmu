// Copyright (c) Kuba Szczodrzyński 2022-4-15.

#ifdef CONFIG_FEAT_BT

#pragma once

enum ok_action {
	NO_ACTION = 0,
	DATA_FALSE = 0b000000, // | data_idx
	DATA_TRUE = 0b100000,  // | data_idx
	CONNECT_LAST_DEVICE = 64,
	DISCONNECT,
	PAIRING_ON,
	PAIRING_OFF,
	MUSIC_PLAY,
	MUSIC_STOP,
	TRACK_NEXT,
	TRACK_PREV,
	TRACK_FF,
	TRACK_REW,
	SHUTDOWN,
	ANALOG_POWER_OFF,
};

void bt_parse_data(char cmd[3]);
void bt_queue_append(ok_action action);
void bt_queue_next();
void bt_queue_clear();
void bt_status_tick();

// state request functions
void bt_request_connection_state();
void bt_request_playback_state();

// connection control functions
void bt_reconnect();
void bt_connect_last_device();
void bt_disconnect();
void bt_shutdown();
#if CONFIG_BSI_ON_BT_PAIRING || CONFIG_RADIO_ON_BT_PAIRING || CONFIG_RADIO_PLAY_BT_PAIRING
void bt_pairing_enable();
#endif
#if CONFIG_RADIO_OFF_BT_PAIRING_OFF || CONFIG_RADIO_PAUSE_BT_PAIRING_OFF
void bt_pairing_disable();
#endif

// music control functions
#if CONFIG_BT_CONN_RADIO_EN_PLAY || CONFIG_BT_CONN_RADIO_SRC_PLAY || CONFIG_AUX_PLAY_BT_PAUSE ||                       \
	CONFIG_RADIO_PAUSE_BT_PAUSE || CONFIG_RADIO_OFF_BT_PAUSE
void bt_music_play();
void bt_music_stop();
#endif
#if CONFIG_FEAT_BT_RADIO_CONTROLS
void bt_track_next();
void bt_track_prev();
void bt_track_ff();
void bt_track_rew();
#endif

#endif
