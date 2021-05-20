#include <avr/io.h>
#include <util/delay.h>

#include <pcf8574.h>
#include <uart.h>

int main() {
    print("Hello world %d\n", 1234);
    prints("5678");
    printc('c');
    i2c_init();
    while (1) {
        if (purx_dataready()) {
            printc(pu_read());
        }
        pcf_write(0, pcf_read(1));
        _delay_ms(1000);
    }
    return 0;
}
