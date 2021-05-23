#include <avr/io.h>
#include <util/delay.h>

#include <pcf8574.h>
#include <uart.h>
#include <mcp2515.h>

struct can_message msg;

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
    //spi_end();

    //i2c_init();
    while (1) {
        if (purx_dataready()) {
            char c = pu_read();
            msg.can_id = 0x131;
            msg.can_dlc = 1;
            msg.data[0] = c;
            mcp_send(&msg);
            printc(c);
        }
        if (mcp_receive(&msg)) {
            prints("msg ");
            printc(msg.data[0]);
            printc('\n');
        }
        _delay_ms(5);
        //pcf_write(0, pcf_read(1));
        //_delay_ms(1000);
    }
    return 0;
}
