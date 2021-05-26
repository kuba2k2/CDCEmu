#ifndef PACKETS_H
#define PACKETS_H

#include "timers.h"

#define PACKET_DEF(id, dlc, data_bytes, ...)                (dlc<<4) | (id>>8), (id & 0xff), ##__VA_ARGS__, (data_bytes)
#define PACKET_DATA(src_byte, src_mask, dst_byte, dst_rsh)  (src_byte) | (src_mask<<3), (dst_byte) | (dst_rsh<<3)
#define PACKET_META(address, timer, delay, flag)            (address), (timer<<5) | T_MS(delay), (flag)

#endif
