//
// Created by Kuba on 2019-07-09.
//

/**
 * sent periodically every 1000ms
 */
class PacketOutCurrentDiskNumber {
public:
    const static uint16_t ID = 0x0E2;
    const static uint8_t LENGTH = 8;
    uint8_t data[LENGTH] = {};

    explicit PacketOutCurrentDiskNumber() {
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0b00000000;
        data[5] = 0b00000011;
        data[6] = 0b00110011;
        data[7] = 0b00110011;
    }

    // unknown value that is known to sometimes change
    void setSometimes() {
        data[4] = 0b00110011;
        data[5] = 0b00110011;
    }

    void setDiskNumber(uint8_t diskNumber) {
        data[3] = diskNumber << 4;
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