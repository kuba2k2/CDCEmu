#include "can.h"

#include "data.h"
#include "main.h"
#include "radio.h"
#include "timers.h"
#include <avr/pgmspace.h>
#include <mcp2515.h>
#include <uart.h>
#include <util/delay.h>

struct can_message msg;

typedef struct cdc_command {
	bool ff_fb : 1;
	bool radio_playing : 1;
	bool intro_mode : 1;
	bool to_start : 1, : 1;
	bool next_track : 1;
	bool previous_track : 1;
	bool radio_enabled : 1;

	bool random_mode : 1, : 6;
	bool repeat_mode : 1;
} cdc_command_t;

/*
0x036 - BSI ignition
00001110 00000000 E0000000 00LIBBBB 00000MMM 00000000 00000000 10100000
				  │          │││         │
				  │          │││         │ Ignition mode:
				  │          │││         │  000 - OFF
				  │          │││         │  001 - ON
				  │          │││         │  010 - Power saving mode
				  │          │││         └─ 011 - ON (first packet only)
				  │          ││└─ Dashboard brightness
				  │          │└─ Disable dark mode
				  │          └─ Dashboard lighting enabled
				  └─ Economy mode active


0x131 - CD changer command
EBF0SIPX R000000R DDDDDDDD 00000000 TTTTTTTT
│││ ││││ │      │ │                 │
│││ ││││ │      │ └─ Go to disk     └─ Go to track
│││ ││││ │      └─ Random mode
│││ ││││ └─ Repeat mode
│││ │││└─ Fast forward/backward
│││ ││└─ Radio playing
│││ │└─ Intro mode
│││ └─ Go to start
│││
││└─ Next track
│└─ Previous track
└─ Radio enabled


0x162 - CDC status
E0100000 L0000SSS 00000110 0000DDDD 00000000 00000110 00000000
│        │    │                │
│        │    │                └─ Disk num
│        │    └─ Status icon
│        └─ Please wait
└─ Radio enabled


0x1A2 - Current disk tracks count
CCCCCCCC 01011000 00000000 00000000 00000000
│
└─ Track count


0x1E2 - Current track information
TTTTTTTT 00000101 00000000 MMMMMMMM 0SSSSSSS 00000000 00000000
│        │                 │         │       │
│        │                 │         │       └─ 80 while switching
│        │                 │         │  Track second
│        │                 │         └─ 7F while switching
│        │                 │  Track minute
│        │                 └─ FF while switching
│        └─ FF FF while switching
└─ Track num


0x1A0 - CDC status
10I10R10 R0000000
  │  │   │
  │  │   └─ Repeat mode
  │  └─ Random mode
  └─ Intro mode
*/

// size of PACKET_DEF - 3 + DLC
// size of PACKET_DATA - 2
const PROGMEM uint8_t packets[] = {
	// 0x162, DLC 7, 2 data bytes - 0x00:0x0D
	PACKET_DEF(0x162, 7, 2, /* data */ 0b10100000, 0b00000010, 0x06, 0, 0x00, 0x06, 0x00),
	PACKET_DATA(DATA_RADIO_PLAYING, 0x01, 1, 0),
	PACKET_DATA(DATA_DISK_NUM, 0x0F, 3, 0),

	// 0x1A2, DLC 5, 0 data bytes - 0x0E:0x15
	PACKET_DEF(0x1A2, 5, 0, /* data */ TRACK_MAX, 0x58, 0x00, 0x00, 0x00),

	// 0x1E2, DLC 7, 3 data bytes - 0x16:0x25
	PACKET_DEF(0x1E2, 7, 3, /* data */ 0, 0x05, 0x00, 0, 0, 0x00, 0x00),
	PACKET_DATA(DATA_TRACK_NUM, 0, 0, 0),
	PACKET_DATA(DATA_TRACK_MINUTE, 0, 3, 0),
	PACKET_DATA(DATA_TRACK_SECOND, 0, 4, 0),

	// 0x1A0, DLC 2, 0 data bytes - 0x26:0x2A
	PACKET_DEF(0x1A0, 2, 0, /* data */ 0b10010010, 0b00000000),

	// 0x531, DLC 8, 0 data bytes - 0x2B:0x35
	PACKET_DEF(0x531, 8, 0, /* data */ 0x09, 0, 0, 0, 0, 0, 0, 0),

	// 0x0E2, DLC 8, 1 data byte - 0x36:0x42
	PACKET_DEF(0x0E2, 8, 1, /* data */ 0, 0, 0, 0, 0, 0x03, 0x33, 0x33),
	PACKET_DATA(DATA_DISK_NUM, 0, 3, 4),
};

#define META_SIZE  3
#define META_COUNT 6

const PROGMEM uint8_t packets_meta[META_SIZE * META_COUNT] = {
	PACKET_META(/* 0x162 */ 0x00, TIMER_PACKET_100MS, 100, DATA_IGNITION),
	PACKET_META(/* 0x1A2 */ 0x0e, TIMER_PACKET_500MS, 500, DATA_RADIO_ENABLED),
	PACKET_META(/* 0x1E2 */ 0x16, TIMER_PACKET_500MS, 500, DATA_RADIO_ENABLED),
	PACKET_META(/* 0x1A0 */ 0x26, TIMER_PACKET_500MS, 500, DATA_RADIO_ENABLED),
	PACKET_META(/* 0x531 */ 0x2B, TIMER_PACKET_1000MS, 1000, DATA_RADIO_ENABLED),
	PACKET_META(/* 0x0E2 */ 0x36, TIMER_PACKET_1000MS, 1000, DATA_RADIO_ENABLED),
};

bool can_init() {
	ensure_spi();

	debug_puts_P("mcp_reset: ");
	debug_putc(mcp_reset() ? '1' : '0');
	debug_puts_P(" mcp_set_bitrate: ");
	debug_putc(mcp_set_bitrate() ? '1' : '0');

	mcp_set_filter_mask(0, false, 0x7FF); // enable both filter masks
	mcp_set_filter_mask(1, false, 0x7FF);
	mcp_set_filter(0, false, 0x131); // accept CDC command message
	mcp_set_filter(1, false, 0x036);

	debug_puts_P(" mcp_mode_normal: ");
	debug_putc(mcp_mode_normal() ? '1' : '0');
	debug_nl();
	return true;
}

void parse_bsi_ignition(const uint8_t cmd[8]) {
	uint8_t mode = cmd[4] & 0b11;
	bool ignition = mode & IGNITION_ON;
	bool powersave = mode == IGNITION_POWERSAVE;
	bool sleep = mode == IGNITION_OFF;
	bool economy = cmd[2] >> 7;
	radio_ignition(!economy && ignition, powersave);
	if (sleep)
		enter_sleep();
}

void parse_cdc_command(const uint8_t cmd[8]) {
	cdc_command_t *cdc = (cdc_command_t *)cmd;

	radio_enabled(cdc->radio_enabled);
	radio_playing(cdc->radio_playing || cdc->intro_mode);

	// radio_repeat_mode(cdc->repeat_mode);
	// radio_random_mode(cdc->random_mode);
	// radio_intro_mode(cdc->intro_mode && cdc->radio_playing);
	// radio_fast_forward(cdc->intro_mode && !cdc->radio_playing && !cdc->ff_fb);
	// radio_fast_backward(cdc->intro_mode && !cdc->radio_playing && cdc->ff_fb);

	if (cdc->next_track)
		radio_next();
	if (cdc->previous_track)
		radio_previous();
	if (cdc->to_start)
		radio_to_start();

	if (cmd[2])
		radio_disk(cmd[2]);
	if (cmd[4])
		radio_track(cmd[4]);
}

void can_receive_all() {
	ensure_spi();
	if (mcp_receive(&msg)) {
		if (msg.can_id == 0x036)
			parse_bsi_ignition(msg.data);
		else if (msg.can_id == 0x131)
			parse_cdc_command(msg.data);
	}
}

void can_send_all() {
	uint8_t tcntg = timer_read_global();
	uint8_t timers_checked = 0;
	for (uint8_t i = 0; i < META_COUNT; i++) {
		uint8_t *meta = (uint8_t *)packets_meta + META_SIZE * i;
		uint8_t delay = pgm_read_byte(&meta[1]);
		uint8_t timer_meta = pgm_read_byte(&meta[2]);
		uint8_t timer = timer_meta >> 4;
		uint8_t flag = timer_meta & 0xf;

		// check the specified timer
		uint8_t timer_mask = 1 << timer;
		bool timer_checked = timer_check(timer, delay, tcntg) || (timers_checked & timer_mask);
		if (!timer_checked)
			continue;
		timers_checked |= timer_mask;

		// check the flag data byte
		if (flag != 0xf && !data[flag])
			continue;
		// get a pointer to the packet data
		uint8_t addr = pgm_read_byte(&meta[0]);
		uint8_t *packet = (uint8_t *)packets + addr;

		uint8_t id_hi = pgm_read_byte(&packet[0]);
		uint8_t id_lo = pgm_read_byte(&packet[1]);

		// set the ID and DLC
		msg.can_id = ((id_hi << 8) | id_lo) & 0x7ff;
		msg.can_dlc = id_hi >> 4;

		// set all default message payload bytes
		for (uint8_t j = 0; j < msg.can_dlc; j++) {
			msg.data[j] = pgm_read_byte(packet + 2 + j);
		}
		// get a pointer to the data byte part
		packet += 2 + msg.can_dlc;

		uint8_t db_count = pgm_read_byte(&packet[0]);
		// set all configured data bytes
		for (uint8_t j = 0; j < db_count; j++) {
			uint8_t masks = pgm_read_byte(packet + 1 + j * 2);
			uint8_t bytes = pgm_read_byte(packet + 2 + j * 2);
			uint8_t src_byte = bytes >> 4;
			uint8_t src_mask = (masks >> 3) ? (masks >> 3) : 0xff;
			uint8_t dst_byte = bytes & 0b111;
			uint8_t dst_rsh = masks & 0b111;
			msg.data[dst_byte] |= (data[src_byte] & src_mask) << dst_rsh;
		}

		ensure_spi();
		// wait until the TX buffer becomes available (one of three)
		while (!mcp_send(&msg)) {
			_delay_ms(1);
		}
	}
}
