#include "can.h"

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <mcp2515.h>
#include <uart.h>
#include "main.h"
#include "data.h"
#include "timers.h"
#include "radio.h"

#define META_SIZE       3
#define META_COUNT      4

struct can_message msg;

struct cdc_command {
    bool ff_fb : 1;
    bool radio_playing : 1;
    bool intro_mode : 1;
    bool to_start : 1, : 1;
    bool next_track : 1;
    bool previous_track : 1;
    bool radio_enabled : 1;

    bool random_mode : 1, : 6;
    bool repeat_mode : 1;
};

/*
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

const PROGMEM uint8_t packets[] = {
    // 0x162, DLC 7, 2 data bytes
    PACKET_DEF(0x162, 7, 2, /* data */ 0b10100000, 0b00000010, 0x06, 0, 0x00, 0x06, 0x00),
    PACKET_DATA(DATA_RADIO_PLAYING, 0x01, 1, 0),
    PACKET_DATA(DATA_DISK_NUM, 0x0F, 3, 0),

    // 0x1A2, DLC 5, 0 data bytes
    PACKET_DEF(0x1A2, 5, 0, /* data */ TRACK_MAX, 0x58, 0x00, 0x00, 0x00),

    // 0x1E2, DLC 7, 3 data bytes
    PACKET_DEF(0x1E2, 7, 3, /* data */ 0, 0x05, 0x00, 0, 0, 0x00, 0x00),
    PACKET_DATA(DATA_TRACK_NUM, 0, 0, 0),
    PACKET_DATA(DATA_TRACK_MINUTE, 0, 3, 0),
    PACKET_DATA(DATA_TRACK_SECOND, 0, 4, 0),

    // 0x1A0, DLC 2, 0 data bytes
    PACKET_DEF(0x1A0, 2, 0, /* data */ 0b10010010, 0b00000000),
};

const PROGMEM uint8_t packets_meta[META_SIZE * META_COUNT] = {
    PACKET_META(/* 0x162 */ 0x00, TIMER_PACKET_100MS, 100, DATA_RADIO_ENABLED),
    PACKET_META(/* 0x1A2 */ 0x0e, TIMER_PACKET_500MS, 500, DATA_RADIO_PLAYING),
    PACKET_META(/* 0x1E2 */ 0x16, TIMER_PACKET_500MS, 500, DATA_RADIO_PLAYING),
    PACKET_META(/* 0x1A0 */ 0x26, TIMER_PACKET_500MS, 500, DATA_RADIO_PLAYING),
};

bool can_init() {
    ensure_spi();
    mcp_init();

    uart_puts_P("mcp_reset: ");
    uart_putc(mcp_reset() ? '1' : '0');
    uart_puts_P(" mcp_set_bitrate: ");
    uart_putc(mcp_set_bitrate() ? '1' : '0');

    mcp_set_filter_mask(0, false, 0x7FF); // enable both filter masks
    mcp_set_filter_mask(1, false, 0x7FF);
    mcp_set_filter(0, false, 0x131); // accept CDC command message

    uart_puts_P(" mcp_mode_normal: ");
    uart_putc(mcp_mode_normal() ? '1' : '0');
    uart_putc('\n');
    return true;
}

void parse_cdc_command(const uint8_t cmd[8]) {
    struct cdc_command* cdc = (cdc_command*) cmd;

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
        if (msg.can_id == 0x131)
            parse_cdc_command(msg.data);
    }
}

void can_send_all() {
    uint8_t timers_checked = 0;
    for (uint8_t i = 0; i < META_COUNT; i++) {
        uint8_t *meta = (uint8_t*)packets_meta + META_SIZE * i;
        uint8_t timer_meta = pgm_read_byte(&meta[1]);
        uint8_t timer = timer_meta >> 5;
        uint8_t delay = timer_meta & 0b11111;

        // check the specified timer
        uint8_t timer_mask = 1<<timer;
        bool timer_checked = timer_check(timer, delay) || (timers_checked & timer_mask);
        if (!timer_checked)
            continue;
        timers_checked |= timer_mask;

        // check the flag data byte
        uint8_t flag = pgm_read_byte(&meta[2]);
        if (flag != 0xff && !data[flag])
            continue;
        // get a pointer to the packet data
        uint8_t addr = pgm_read_byte(&meta[0]);
        uint8_t *packet = (uint8_t*)packets + addr;

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
            uint8_t src = pgm_read_byte(packet + 1 + j*2);
            uint8_t dst = pgm_read_byte(packet + 2 + j*2);
            uint8_t src_byte = src & 0b111;
            uint8_t src_mask = (src>>3) ? (src>>3) : 0xff;
            uint8_t dst_byte = dst & 0b111;
            uint8_t dst_rsh = (dst>>3);
            msg.data[dst_byte] |= (data[src_byte] & src_mask) << dst_rsh;
        }

        ensure_spi();
        // wait until the TX buffer becomes available (one of three)
        while (!mcp_send(&msg)) {
            _delay_ms(1);
        }
    }
}
