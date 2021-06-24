#include "radio.h"

#include <uart.h>
#include "data.h"
#include "timers.h"
#include "can.h"

void radio_ignition(uint8_t mode, bool economy) {
    data[DATA_IGNITION] = !economy && (mode & IGNITION_ON /* ON or ON_FIRST */);
    data[DATA_POWERSAVE] = mode == IGNITION_POWERSAVE;
    if (!data[DATA_IGNITION]) {
        radio_enabled(false);
        radio_playing(false);
    }
}

void radio_enabled(bool radio_enabled) {
    if (radio_enabled == data[DATA_RADIO_ENABLED])
        return;
    data[DATA_RADIO_ENABLED] = radio_enabled;
    timer_reset(TIMER_PACKET_100MS);

    if (radio_enabled)
        uart_puts_P("radio enabled\n");
    else
        uart_puts_P("radio disabled\n");
}

void radio_playing(bool radio_playing) {
    if (radio_playing == data[DATA_RADIO_PLAYING])
        return;
    data[DATA_RADIO_PLAYING] = radio_playing;
    timer_reset(TIMER_PACKET_100MS);

    if (radio_playing)
        uart_puts_P("radio playing\n");
    else
        uart_puts_P("radio paused\n");
}

void radio_tick() {
    if (!timer_check(TIMER_TRACK_TIME, T_MS(1000)))
        return;
    if (!data[DATA_RADIO_PLAYING])
        return;
    if (++data[DATA_TRACK_SECOND] > 59) {
        data[DATA_TRACK_SECOND] = 0;
        data[DATA_TRACK_MINUTE]++;
    }
}

void radio_next(uint8_t num) {
    if (data[DATA_TRACK_NUM] == TRACK_MAX)
        return;
    if (num)
        data[DATA_TRACK_NUM] = num;
    else
        data[DATA_TRACK_NUM]++;
    radio_to_start();

    uart_puts_P("radio_next\n");
}

void radio_previous(uint8_t num) {
    if (data[DATA_TRACK_NUM] == 1)
        return;
    if (num)
        data[DATA_TRACK_NUM] = num;
    else
        data[DATA_TRACK_NUM]--;
    radio_to_start();

    uart_puts_P("radio_previous\n");
}

void radio_to_start() {
    data[DATA_TRACK_MINUTE] = 0;
    data[DATA_TRACK_SECOND] = 0;
    timer_reset(TIMER_PACKET_500MS);
}

void radio_disk(uint8_t num) {
    if (num == data[DATA_DISK_NUM])
        return;
    data[DATA_DISK_NUM] = num;
    data[DATA_TRACK_NUM] = 1;
    radio_to_start();
}

void radio_track(uint8_t num) {
    if (num > data[DATA_TRACK_NUM])
        radio_next(num);
    else if (num < data[DATA_TRACK_NUM])
        radio_previous(num);
}
