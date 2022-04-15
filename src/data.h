#pragma once

#include <stdint.h>

#define DATA_RADIO_ENABLED 0 // If the radio is turned on
#define DATA_RADIO_PLAYING 1 // Radio source is CDC
#define DATA_DISK_NUM	   2
#define DATA_TRACK_NUM	   3
#define DATA_TRACK_MINUTE  4
#define DATA_TRACK_SECOND  5
#define DATA_IGNITION	   6  // BSI Ignition is enabled and not Economy Mode
#define DATA_POWERSAVE	   7  // BSI is in power-save mode, ignition will disable soon
#define DATA_BT_PAIRING	   8  // Bluetooth is in pairing mode
#define DATA_BT_CONNECTED  9  // Bluetooth device is connected
#define DATA_BT_PLAYING	   10 // Audio playing from BT
#define DATA_AUDIO_PLAYING 11 // Audio playing from Line-In

extern uint8_t data[12];
