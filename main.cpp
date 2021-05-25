#include <avr/io.h>
#include <util/delay.h>

#include <pcf8574.h>
#include <uart.h>
#include <mcp2515.h>
#include <timers.h>

struct can_message msg;

#define TIMER_CURRENT_DISK      0
#define TIMER_TRACK_COUNT       1
#define TIMER_TRACK_NUM         2
#define TIMER_STATUS            3

#define TRACK_MAX       100

bool radio_enabled = 0;
bool radio_playing = 0;
uint8_t disk_num = 1;
uint8_t track_num = 1;

void radio_enable() {
    radio_enabled = true;
    timer_reset(TIMER_CURRENT_DISK);
    prints("radio_enable\n");
}

void radio_disable() {
    radio_enabled = false;
    prints("radio_disable\n");
}

void radio_play() {
    radio_playing = true;
    prints("radio_play\n");
}

void radio_pause() {
    radio_playing = false;
    prints("radio_pause\n");
}

void track_previous(uint8_t skip_to) {
    if (track_num > 1)
        track_num--;
    if (skip_to)
        track_num = skip_to;
    prints("track_previous\n");
}

void track_next(uint8_t skip_to) {
    if (track_num < TRACK_MAX)
        track_num++;
    if (skip_to)
        track_num = skip_to;
    prints("track_next\n");
}

void cdc_command_parse(const uint8_t cmd[8]) {
    // radio enabled (byte 0, bit 7)
    if (((cmd[0] & (1<<7)) >> 7) != radio_enabled) {
        if (radio_enabled)
            radio_disable();
        else
            radio_enable();
    }

    // single track back (byte 0, bit 6)
    if (cmd[0] & (1<<6))
        track_previous(0);

    // single track forward (byte 0, bit 5)
    if (cmd[0] & (1<<5))
        track_next(0);

    // back to start (byte 0, bit 3)
    if (cmd[0] & (1<<3))
        track_previous(0);

    // playing enabled (byte 0, bit 1)
    if (((cmd[0] & (1<<1)) >> 1) != radio_playing) {
        if (radio_playing)
            radio_pause();
        else
            radio_play();
    }

    // go to track (byte 4)
    if (cmd[4] && cmd[4] < track_num)
        track_previous(cmd[4]);
    if (cmd[4] && cmd[4] > track_num)
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

        if (radio_enabled && timer_check(TIMER_CURRENT_DISK, T_MS(100))) {
            msg.can_id = 0x162;
            msg.can_dlc = 7;
            msg.data[0] = 0b10100000;
            msg.data[1] = 0b00000010 | (radio_playing & 1);
            msg.data[2] = 0x06;
            msg.data[3] = disk_num;
            msg.data[4] = 0x00;
            msg.data[5] = 0x06;
            msg.data[6] = 0x00;
            mcp_send(&msg);
        }

        if (radio_playing && timer_check(TIMER_TRACK_COUNT, T_MS(500))) {
            msg.can_id = 0x1A2;
            msg.can_dlc = 5;
            msg.data[0] = TRACK_MAX;
            msg.data[1] = 0x58;
            msg.data[2] = 0x00;
            msg.data[3] = 0x00;
            msg.data[4] = 0x00;
            mcp_send(&msg);
        }

        if (radio_playing && timer_check(TIMER_TRACK_NUM, T_MS(500))) {
            msg.can_id = 0x1E2;
            msg.can_dlc = 7;
            msg.data[0] = track_num;
            msg.data[1] = 0x05;
            msg.data[2] = 0x00;
            msg.data[3] = 1; // track minute
            msg.data[4] = 1; // track second
            msg.data[5] = 0x00;
            msg.data[6] = 0x00;
            mcp_send(&msg);
        }

        if (radio_playing && timer_check(TIMER_STATUS, T_MS(500))) {
            msg.can_id = 0x1A0;
            msg.can_dlc = 2;
            msg.data[0] = 0b10010010;
            msg.data[1] = 0b00000000;
            mcp_send(&msg);
        }

        if (purx_dataready()) {
            char c = pu_read();
            printc(c);
        }
        _delay_ms(1);
    }
    return 0;
}
