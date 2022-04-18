#pragma once

extern "C" {
#include <i2cmaster.h>
}

bool pcf_read(uint8_t pin);
bool pcf_read_state(uint8_t pin);
void pcf_write(uint8_t pin, uint8_t value);
void pcf_write_mask(uint8_t pins, uint8_t values);
void pcf_toggle(uint8_t pin);
