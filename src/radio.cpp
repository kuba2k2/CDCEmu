#include "radio.h"

#include "data.h"
#include "events.h"
#include "timers.h"

void radio_set_ignition(bool enabled, bool powersave) { // NOLINT(misc-no-recursion)
	// `enabled` will be false if economy is active
	if (enabled == DATA(IGNITION) && powersave == DATA(POWERSAVE))
		return;
	on_bsi_ignition(enabled || (powersave && DATA(IGNITION)));
	DATA(IGNITION) = enabled;
	DATA(POWERSAVE) = powersave;

	radio_set_playing(false);
	radio_set_enabled(false);
}

void radio_set_enabled(bool radio_enabled) { // NOLINT(misc-no-recursion)
	if (radio_enabled == DATA(RADIO_ENABLED))
		return;
	DATA(RADIO_ENABLED) = radio_enabled;
	timer_reset(TIMER_PACKET_100MS);

	if (radio_enabled) {
		radio_set_ignition(true, false);
	}
	on_radio_enabled(radio_enabled);
}

void radio_set_playing(bool radio_playing) {
	if (radio_playing == DATA(RADIO_PLAYING))
		return;
	DATA(RADIO_PLAYING) = radio_playing;
	timer_reset(TIMER_PACKET_100MS);

	on_radio_playing(radio_playing);
}

void radio_tick() {
	if (!timer_check(TIMER_TRACK_TIME, T_MS(1000)))
		return;
	if (!DATA(RADIO_PLAYING))
		return;
	if (++DATA(TRACK_SECOND) > 59) {
		DATA(TRACK_SECOND) = 0;
		DATA(TRACK_MINUTE)++;
	}
}

void radio_next(uint8_t num) {
	if (DATA(TRACK_NUM) == TRACK_MAX)
		return;
	if (num)
		DATA(TRACK_NUM) = num;
	else
		DATA(TRACK_NUM)++;
	radio_to_start();
	on_req_track_next();
}

void radio_previous(uint8_t num) {
	if (DATA(TRACK_NUM) == 1)
		return;
	if (num)
		DATA(TRACK_NUM) = num;
	else
		DATA(TRACK_NUM)--;
	radio_to_start();
	on_req_track_previous();
}

void radio_to_start() {
	DATA(TRACK_MINUTE) = 0;
	DATA(TRACK_SECOND) = 0;
	timer_reset(TIMER_PACKET_500MS);
}

void radio_set_disk(uint8_t num) {
	if (num == DATA(DISK_NUM))
		return;
	DATA(DISK_NUM) = num;
	DATA(TRACK_NUM) = 1;
	radio_to_start();
}

void radio_set_track(uint8_t num) {
	if (num > DATA(TRACK_NUM))
		radio_next(num);
	else if (num < DATA(TRACK_NUM))
		radio_previous(num);
}
