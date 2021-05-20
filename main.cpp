#include <avr/io.h>
#include <util/delay.h>

extern "C" {
    #include <i2cmaster.h>
}

char s = 0;

int main() {
    i2c_init();
    while (1) {
        i2c_start_wait((0x22<<1) | I2C_WRITE);
        i2c_write(s ? 0xff : 0x00);
        s = s ? 0 : 1;
        i2c_stop();
        _delay_ms(1000);
    }
    return 0;
}
