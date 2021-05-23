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
    //spi_end();

    //i2c_init();
    while (1) {
        if (purx_dataready()) {
            struct can_message msg;
            msg.can_id = 0x131;
            msg.can_dlc = 4;
            msg.data[0] = 1;
            msg.data[1] = 2;
            msg.data[2] = 3;
            msg.data[3] = 4;
            mcp_send(&msg);
            printc(pu_read());
        }
        //pcf_write(0, pcf_read(1));
        //_delay_ms(1000);
    }
    return 0;
}
