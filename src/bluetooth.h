// Copyright (c) Kuba Szczodrzyński 2022-4-15.

#ifdef CONFIG_FEAT_BT

#pragma once

void bt_parse_data(char cmd[3]);

// internal event helpers
void bt_set_connected(bool state);
void bt_set_playing(bool state);

// state request functions
void bt_request_connection_state();
void bt_request_playback_state();

// connection control functions
void bt_connect_last_device();
void bt_disconnect();
void bt_pairing_enable();
void bt_pairing_disable();

// music control functions
#if CONFIG_PAUSE_BT_ON_RADIO || CONFIG_PAUSE_BT_ON_AUX
void bt_music_play_pause();
void bt_music_stop();
#endif
#if CONFIG_FEAT_BT_RADIO_CONTROLS
void bt_track_next();
void bt_track_prev();
void bt_track_ff();
void bt_track_rew();
#endif

#endif
