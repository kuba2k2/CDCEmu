#include "pcf8574.h"

uint8_t pcf_input = 0x00;
uint8_t pcf_output = 0xff;

void pcf_transfer(uint8_t read) {
    i2c_start_wait((CONFIG_PCF_ADDR<<1) | read);
    if (read)
        pcf_input = i2c_readNak();
    else
        i2c_write(pcf_output);
    i2c_stop();
}

uint8_t pcf_read(uint8_t pin) {
    pcf_transfer(1);
    return (pcf_input & (1 << pin)) > 0;
}

void pcf_write(uint8_t pin, uint8_t value) {
    pcf_output &= ~(1<<pin);
    pcf_output |= ((value > 0)<<pin);
    pcf_transfer(0);
}

void pcf_toggle(uint8_t pin) {
    pcf_output ^= (1<<pin);
    pcf_transfer(0);
}
