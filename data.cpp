#include "data.h"

uint8_t data[12] = {
	false, // DATA_RADIO_ENABLED
	false, // DATA_RADIO_PLAYING
	1,	   // DATA_DISK_NUM
	1,	   // DATA_TRACK_NUM
	0,	   // DATA_TRACK_MINUTE
	0,	   // DATA_TRACK_SECOND
	false, // DATA_IGNITION
	false, // DATA_POWERSAVE
	false, // DATA_BT_PAIRING
	false, // DATA_BT_CONNECTED
	false, // DATA_BT_PLAYING
	false, // DATA_AUDIO_PLAYING
};
