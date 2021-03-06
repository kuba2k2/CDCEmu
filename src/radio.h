#pragma once

#include <stdint.h>

void radio_set_ignition(bool enabled, bool powersave);
void radio_set_enabled(bool radio_enabled);
void radio_set_playing(bool radio_playing);
void radio_tick();
void radio_next(uint8_t num = 0);
void radio_previous(uint8_t num = 0);
void radio_to_start();
void radio_set_disk(uint8_t num);
void radio_set_track(uint8_t num);
