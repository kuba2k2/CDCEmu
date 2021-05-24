#include <avr/io.h>
#include <util/delay.h>

#include <pcf8574.h>
#include <uart.h>
#include <mcp2515.h>
#include <timers.h>

struct can_message msg;

int main() {
    timer_start();

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
        if (timer_check(0, T_MS(100))) {
            msg.can_id = 0x131;
            msg.can_dlc = 1;
            msg.data[0] = 0x5f;
            mcp_send(&msg);
        }
        if (timer_check(1, T_MS(1000))) {
            prints("1s ");
            msg.can_id = 0x165;
            msg.can_dlc = 3;
            msg.data[0] = 'x';
            msg.data[1] = 'y';
            msg.data[2] = 'z';
            mcp_send(&msg);
        }
        if (purx_dataready()) {
            char c = pu_read();
            printc(c);
        }
        if (mcp_receive(&msg)) {
            prints("msg ");
            printc(msg.data[0]);
            printc('\n');
        }
        _delay_ms(1);
    }
    return 0;
}
