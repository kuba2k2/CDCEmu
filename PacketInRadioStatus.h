//
// Created by Kuba on 2019-07-09.
//

#define SOURCE_TUNER 1
#define SOURCE_CD 2
#define SOURCE_CD_CHANGER 3
#define SOURCE_AUX1 4
#define SOURCE_AUX2 5
#define SOURCE_USB 6
#define SOURCE_BLUETOOTH 7

/**
 * sent periodically every 100ms
 */
class PacketInRadioStatus {
public:
    const static uint16_t ID = 0x165;
    const static uint8_t LENGTH = 4;
    uint8_t data[LENGTH] = {};

    explicit PacketInRadioStatus(can_frame msg) {
        for (int i = 0; i < msg.can_dlc; i++) {
            data[i] = msg.data[i];
        }
    }

    boolean radioEnabled() {
        return bitRead(data[0], 7);
    }

    boolean silence() {
        return bitRead(data[0], 5);
    }

    uint8_t source() {
        return bitReadCount(data[2], 4, 3);
    }
};