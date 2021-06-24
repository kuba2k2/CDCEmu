#ifndef DATA_H
#define DATA_H

#include <stdint.h>

#define DATA_RADIO_ENABLED      0
#define DATA_RADIO_PLAYING      1
#define DATA_DISK_NUM           2
#define DATA_TRACK_NUM          3
#define DATA_TRACK_MINUTE       4
#define DATA_TRACK_SECOND       5
#define DATA_IGNITION           6
#define DATA_POWERSAVE          7

extern uint8_t data[8];

#endif
