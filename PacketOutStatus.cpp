//
// Created by Kuba on 2019-07-09.
//

/**
 * sent periodically every 500ms
 */
class PacketOutStatus {
public:
    const static uint16_t ID = 0x1A0;
    const static uint8_t LENGTH = 2;
    uint8_t data[LENGTH] = {};

    explicit PacketOutStatus() {
        data[0] = 0b10010010;
        data[1] = 0b00000000;
    }

    void setIntroMode(boolean enabled) {
        data[0] = bitWrite(data[0], 5, enabled);
    }

    void setRandomMode(boolean enabled) {
        data[0] = bitWrite(data[0], 2, enabled);
    }

    void setRepeatMode(boolean enabled) {
        data[1] = bitWrite(data[1], 7, enabled);
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