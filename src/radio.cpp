#include "radio.h"

#include <uart.h>

#include "data.h"
#include "timers.h"

void radio_set_ignition(bool enabled, bool powersave) { // NOLINT(misc-no-recursion)
	if (enabled == data[DATA_IGNITION] && powersave == data[DATA_POWERSAVE])
		return;
	data[DATA_IGNITION] = enabled;
	data[DATA_POWERSAVE] = powersave;

	if (enabled) {
		debug_puts_P("ignition enabled\r\n");
	} else {
		radio_set_playing(false);
		radio_set_enabled(false);
		debug_puts_P("ignition disabled\r\n");
	}
}

void radio_set_enabled(bool radio_enabled) { // NOLINT(misc-no-recursion)
	if (radio_enabled == data[DATA_RADIO_ENABLED])
		return;
	data[DATA_RADIO_ENABLED] = radio_enabled;
	timer_reset(TIMER_PACKET_100MS);

	if (radio_enabled) {
		radio_set_ignition(true, false);
		debug_puts_P("radio enabled\r\n");
	} else {
		debug_puts_P("radio disabled\r\n");
	}
}

void radio_set_playing(bool radio_playing) {
	if (radio_playing == data[DATA_RADIO_PLAYING])
		return;
	data[DATA_RADIO_PLAYING] = radio_playing;
	timer_reset(TIMER_PACKET_100MS);

	if (radio_playing)
		debug_puts_P("radio playing\r\n");
	else
		debug_puts_P("radio paused\r\n");
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

	debug_puts_P("radio_next\r\n");
}

void radio_previous(uint8_t num) {
	if (data[DATA_TRACK_NUM] == 1)
		return;
	if (num)
		data[DATA_TRACK_NUM] = num;
	else
		data[DATA_TRACK_NUM]--;
	radio_to_start();

	debug_puts_P("radio_previous\r\n");
}

void radio_to_start() {
	data[DATA_TRACK_MINUTE] = 0;
	data[DATA_TRACK_SECOND] = 0;
	timer_reset(TIMER_PACKET_500MS);
}

void radio_set_disk(uint8_t num) {
	if (num == data[DATA_DISK_NUM])
		return;
	data[DATA_DISK_NUM] = num;
	data[DATA_TRACK_NUM] = 1;
	radio_to_start();
}

void radio_set_track(uint8_t num) {
	if (num > data[DATA_TRACK_NUM])
		radio_next(num);
	else if (num < data[DATA_TRACK_NUM])
		radio_previous(num);
}
