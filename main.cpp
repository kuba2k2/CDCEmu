#include <avr/io.h>
#include <util/delay.h>

#include <pcf8574.h>
#include <uart.h>
#include <mcp2515.h>
#include <timers.h>

#include "packets.h"

struct can_message msg;

#define TIMER_CURRENT_DISK      0
#define TIMER_TRACK_COUNT       1
#define TIMER_TRACK_NUM         2
#define TIMER_STATUS            3

#define TRACK_MAX       100

#define DATA_RADIO_ENABLED      0
#define DATA_RADIO_PLAYING      1
#define DATA_DISK_NUM           2
#define DATA_TRACK_NUM          3

uint8_t data[8] = {false, false, 1, 1, 0x00, 0x00, 0x00, 0x00};

uint8_t packets[] = {
    // 0x162, DLC 7, 2 data bytes
    PACKET_DEF(0x162, 7, 2, /* data */ 0b10100000, 0b00000010, 0x06, 0, 0x00, 0x06, 0x00),
    PACKET_DATA(DATA_RADIO_PLAYING, 0x01, 1, 0),
    PACKET_DATA(DATA_DISK_NUM, 0x0F, 3, 0),

    // 0x1A2, DLC 5, 0 data bytes
    PACKET_DEF(0x1A2, 5, 0, /* data */ TRACK_MAX, 0x58, 0x00, 0x00, 0x00),

    // 0x1E2, DLC 7, 1 data byte
    PACKET_DEF(0x1E2, 7, 1, /* data */ 0, 0x05, 0x00, 1, 1, 0x00, 0x00),
    PACKET_DATA(DATA_TRACK_NUM, 0, 0, 0),

    // 0x1A0, DLC 2, 0 data bytes
    PACKET_DEF(0x1A0, 2, 0, /* data */ 0b10010010, 0b00000000),
};

#define META_SIZE       3
#define META_COUNT      4

uint8_t packets_meta[META_SIZE * META_COUNT] = {
    PACKET_META(/* 0x162 */ 0x00, TIMER_CURRENT_DISK, 100, DATA_RADIO_ENABLED),
    PACKET_META(/* 0x1A2 */ 0x0e, TIMER_TRACK_COUNT, 500, DATA_RADIO_PLAYING),
    PACKET_META(/* 0x1E2 */ 0x16, TIMER_TRACK_NUM, 500, DATA_RADIO_PLAYING),
    PACKET_META(/* 0x1A0 */ 0x22, TIMER_STATUS, 500, DATA_RADIO_PLAYING),
};

void radio_enable() {
    data[DATA_RADIO_ENABLED] = true;
    timer_reset(TIMER_CURRENT_DISK);
    prints("radio_enable\n");
}

void radio_disable() {
    data[DATA_RADIO_ENABLED] = false;
    prints("radio_disable\n");
}

void radio_play() {
    data[DATA_RADIO_PLAYING] = true;
    prints("radio_play\n");
}

void radio_pause() {
    data[DATA_RADIO_PLAYING] = false;
    prints("radio_pause\n");
}

void track_previous(uint8_t skip_to) {
    if (data[DATA_TRACK_NUM] > 1)
        data[DATA_TRACK_NUM]--;
    if (skip_to)
        data[DATA_TRACK_NUM] = skip_to;
    prints("track_previous\n");
}

void track_next(uint8_t skip_to) {
    if (data[DATA_TRACK_NUM] < TRACK_MAX)
        data[DATA_TRACK_NUM]++;
    if (skip_to)
        data[DATA_TRACK_NUM] = skip_to;
    prints("track_next\n");
}

void cdc_command_parse(const uint8_t cmd[8]) {
    // radio enabled (byte 0, bit 7)
    if (cmd[0] & 1<<7) {
        if (!data[DATA_RADIO_ENABLED])
            radio_enable();
    }
    else if (data[DATA_RADIO_ENABLED])
        radio_disable();

    // single track back (byte 0, bit 6)
    if (cmd[0] & 1<<6)
        track_previous(0);

    // single track forward (byte 0, bit 5)
    if (cmd[0] & 1<<5)
        track_next(0);

    // back to start (byte 0, bit 3)
    if (cmd[0] & 1<<3)
        track_previous(0);

    // playing enabled (byte 0, bit 1)
    if (cmd[0] & 1<<1) {
        if (!data[DATA_RADIO_PLAYING])
            radio_play();
    }
    else if (data[DATA_RADIO_PLAYING])
        radio_pause();

    // go to track (byte 4)
    if (cmd[4] && cmd[4] < data[DATA_TRACK_NUM])
        track_previous(cmd[4]);
    if (cmd[4] && cmd[4] > data[DATA_TRACK_NUM])
        track_next(cmd[4]);
}

int main() {
    timer_start();

    spi_begin();
    mcp_init();
    prints("mcp_reset: ");
    printc(mcp_reset() ? '1' : '0');
    prints(" mcp_set_bitrate: ");
    printc(mcp_set_bitrate() ? '1' : '0');

    mcp_set_filter_mask(0, false, 0x7FF); // enable both filter masks
    mcp_set_filter_mask(1, false, 0x7FF);
    mcp_set_filter(0, false, 0x131); // accept CDC command message

    prints(" mcp_mode_normal: ");
    printc(mcp_mode_normal() ? '1' : '0');
    printc('\n');
    //spi_end();

    //i2c_init();
    while (1) {
        if (mcp_receive(&msg)) {
            if (msg.can_id == 0x131)
                cdc_command_parse(msg.data);
        }

        for (uint8_t i = 0; i < META_COUNT; i++) {
            uint8_t *meta = packets_meta + META_SIZE * i;
            uint8_t timer = meta[1] >> 5;
            uint8_t delay = meta[1] & 0b11111;
            // check the specified timer
            if (!timer_check(timer, delay))
                continue;
            // check the flag data byte
            if (meta[2] != 0xff && !data[meta[2]])
                continue;
            // get a pointer to the packet data
            uint8_t *packet = packets + meta[0];

            // set the ID and DLC
            msg.can_id = ((packet[0] << 8) | packet[1]) & 0x7ff;
            msg.can_dlc = packet[0] >> 4;

            // set all default message payload bytes
            for (uint8_t j = 0; j < msg.can_dlc; j++) {
                msg.data[j] = packet[2 + j];
            }
            // get a pointer to the data byte part
            packet += 2 + msg.can_dlc;

            uint8_t db_count = packet[0];
            // set all configured data bytes
            for (uint8_t j = 0; j < db_count; j++) {
                uint8_t src = packet[1 + j*2];
                uint8_t dst = packet[2 + j*2];
                uint8_t src_byte = src & 0b111;
                uint8_t src_mask = (src>>3) ? (src>>3) : 0xff;
                uint8_t dst_byte = dst & 0b111;
                uint8_t dst_rsh = (dst>>3);
                msg.data[dst_byte] |= (data[src_byte] & src_mask) << dst_rsh;
            }

            // wait until the TX buffer becomes available (one of three)
            while (!mcp_send(&msg)) {
                _delay_ms(1);
            }
        }

        if (purx_dataready()) {
            char c = pu_read();
            printc(c);
        }
        _delay_ms(1);
    }
    return 0;
}
