#ifndef PCF8574_H
#define PCF8574_H

extern "C" {
    #include <i2cmaster.h>
}

#ifndef PCF_ADDR
    #define PCF_ADDR 0x20
#endif

uint8_t pcf_read(uint8_t pin);
void pcf_write(uint8_t pin, uint8_t value);
void pcf_toggle(uint8_t pin);

#endif
