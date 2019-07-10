//
// Created by Kuba on 2019-07-09.
//

/**
 * sent periodically every
 */
class PacketOutTemplate {
public:
    const static uint16_t ID = 0x1E2;
    const static uint8_t LENGTH = 7;
    uint8_t data[LENGTH] = {};

    explicit PacketInTemplate() {
        data[0] = 0; // track number
        data[1] = 0b00000101;
        data[2] = 0;
        data[3] = 0; // minute
        data[4] = 0; // second
        data[5] = 0;
        data[6] = 0;
    }

    void send(MCP2515 mcp2515) {
        struct can_frame msg;
        msg.can_id = ID;
        msg.can_dlc = LENGTH;
        for (int i = 0; i < LENGTH; i++) {
            msg.data[i] = data[i];
        }

        /*Serial.print(msg.can_id, HEX); // print ID
        Serial.print('\t');
        Serial.print(msg.can_dlc, HEX); // print DLC
        Serial.print('\t');
        for (int i = 0; i < msg.can_dlc; i++)  { // print the data
            if (msg.data[i] < 0x10) {
                Serial.print('0');
            }
            Serial.print(msg.data[i], HEX);
            Serial.print(' ');
        }*/

        mcp2515.sendMessage(&msg);
    }
};