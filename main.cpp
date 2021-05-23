#include <avr/io.h>
#include <util/delay.h>

#include <pcf8574.h>
#include <uart.h>
#include <mcp2515.h>

int main() {

    spi_begin();
    mcp_init();
    prints("mcp_reset: ");
    printc(mcp_reset() ? '1' : '0');
    prints(" mcp_set_bitrate: ");
    printc(mcp_set_bitrate() ? '1' : '0');
    prints(" mcp_mode_normal: ");
    printc(mcp_mode_normal() ? '1' : '0');
    printc('\n');
    spi_end();

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
