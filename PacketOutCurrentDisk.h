//
// Created by Kuba on 2019-07-09.
//

#define STATUS_ICON_NONE 0
#define STATUS_ICON_STOP 1
#define STATUS_ICON_PAUSE 2
#define STATUS_ICON_PLAY1 3
#define STATUS_ICON_FAST_FORWARD 4
#define STATUS_ICON_FAST_BACKWARD 5
#define STATUS_ICON_NONE2 6
#define STATUS_ICON_PLAY2 7

/**
 * sent periodically every 100ms
 */
class PacketOutCurrentDisk {
public:
    const static uint16_t ID = 0x162;
    const static uint8_t LENGTH = 8;
    uint8_t data[LENGTH] = {};

    /* La trame 162 permet de gérer le chargeur de CD. Elle contient a0-00-05-01-00-00-00-0X où X est
    le numéro du CD sollicité. */
    explicit PacketOutCurrentDisk() {
        data[0] = 0b00100000;
        data[1] = 0b00000000;
        data[2] = 0b00000101;
        data[3] = 0; // disk number
        data[4] = 0;
        data[5] = 0b00000110; // Non-zero value required to add cd changer to sources
        data[6] = 0;
        data[7] = 0;
    }

    void setDiskNumber(uint8_t diskNumber) {
        data[3] = diskNumber;
        data[7] = diskNumber;
    }

    void setRadioEnabled(boolean radioEnabled) {
        data[0] = bitWrite(data[0], 7, radioEnabled);
    }

    void setDiskChanged(boolean diskChanged) {
        data[1] = bitWrite(data[1], 7, diskChanged);
    }

    void setStatusIcon(uint8_t statusIcon) {
        data[1] = (data[1] & 0b10000000) | statusIcon;
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
                Serial.print(' ');
            }
            Serial.print(msg.data[i], HEX);
            Serial.print(' ');
        }
        Serial.println();*/

        mcp2515.sendMessage(&msg);
    }
};