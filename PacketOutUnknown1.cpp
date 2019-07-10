//
// Created by Kuba on 2019-07-09.
//

/**
 * sent periodically every 1000ms
 */
class PacketOutUnknown1 {
public:
    const static uint16_t ID = 0x531;
    const static uint8_t LENGTH = 8;
    uint8_t data[LENGTH] = {};

    explicit PacketOutUnknown1() {
        data[0] = 0x09;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
    }

    void send(MCP2515 mcp2515) {
        struct can_frame msg;
        msg.can_id = ID;
        msg.can_dlc = LENGTH;
        for (int i = 0; i < LENGTH; i++) {
            msg.data[i] = data[i];
        }
        mcp2515.sendMessage(&msg);
    }
};