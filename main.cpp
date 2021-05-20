#include <avr/io.h>
#include <util/delay.h>

#include <pcf8574.h>

int main() {
    i2c_init();
    while (1) {
        pcf_write(0, pcf_read(1));
        _delay_ms(1000);
    }
    return 0;
}
