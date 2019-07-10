//
// Created by Kuba on 2019-07-09.
//

/**
 * sent periodically every
 */
class PacketInTemplate {
public:
    const static uint16_t ID = 0x131;
    const static uint8_t LENGTH = 5;
    uint8_t data[LENGTH] = {};

    explicit PacketInTemplate(can_frame msg) {
        for (int i = 0; i < msg.can_dlc; i++) {
            data[i] = msg.data[i];
        }
    }

};