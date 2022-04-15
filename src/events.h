// Copyright (c) Kuba Szczodrzyński 2022-4-15.

#pragma once

// CAN events
void on_bsi_ignition(bool state);
void on_radio_enabled(bool state);
void on_radio_playing(bool state);

// Bluetooth events
#if CONFIG_FEAT_BT
void on_bt_connected(bool state);
void on_bt_playing(bool state);
#endif

// User requested events
#if CONFIG_FEAT_AUX_RADIO_CONTROLS || CONFIG_FEAT_BT_RADIO_CONTROLS
void on_req_track_next();
void on_req_track_previous();
void on_req_track_ff();
void on_req_track_rew();
#endif
