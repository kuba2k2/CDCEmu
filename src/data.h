#pragma once

#include <stdint.h>

enum data_item {
	DATA_RADIO_ENABLED = 0, // If the radio is turned on
	DATA_RADIO_PLAYING,		// Radio source is CDC
	DATA_DISK_NUM,
	DATA_TRACK_NUM,
	DATA_TRACK_MINUTE,
	DATA_TRACK_SECOND,
	DATA_IGNITION,		// BSI Ignition is enabled and not Economy Mode
	DATA_POWERSAVE,		// BSI is in power-save mode, ignition will disable soon
	DATA_ANALOG_POWER,	// Analog PCB is powered on
	DATA_BT_PAIRING,	// Bluetooth is in pairing mode
	DATA_BT_CONNECTED,	// Bluetooth device is connected
	DATA_BT_PLAYING,	// Audio playing from BT
	DATA_AUDIO_PLAYING, // Audio playing from Line-In
};

extern uint8_t data[13];

#define DATA(name) data[DATA_##name]
