#include "pcf8574.h"

uint8_t pcf_input = 0x00;
uint8_t pcf_output = 0xff;

void pcf_transfer(uint8_t read) {
	i2c_start_wait((CONFIG_PCF_ADDR << 1) | read);
	if (read)
		pcf_input = i2c_readNak();
	else
		i2c_write(pcf_output);
	i2c_stop();
}

bool pcf_read(uint8_t pin) {
	pcf_transfer(1);
	return (pcf_input & (1 << pin)) > 0;
}

bool pcf_read_state(uint8_t pin) {
	return (pcf_output & (1 << pin)) > 0;
}

void pcf_write(uint8_t pin, uint8_t value) {
	pcf_write_mask(1 << pin, (value > 0) << pin);
}

void pcf_write_mask(uint8_t pins, uint8_t values) {
	pcf_output &= ~pins;		 // clear specified pins
	pcf_output |= values & pins; // set new values
	pcf_transfer(0);
}

void pcf_toggle(uint8_t pin) {
	pcf_output ^= (1 << pin);
	pcf_transfer(0);
}
