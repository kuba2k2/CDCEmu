//
// Created by Kuba on 2019-07-09.
//

/**
 * sent periodically every 500ms
 */
class PacketInRadioFrequency {
public:
    const static uint16_t ID = 0x225;
    const static uint8_t LENGTH = 5;
    uint8_t data[LENGTH] = {};

    explicit PacketInRadioFrequency(can_frame msg) {
        for (int i = 0; i < msg.can_dlc; i++) {
            data[i] = msg.data[i];
        }
    }

    boolean stationListOpen() {
        return bitRead(data[0], 7);
    }

    /**
     * @return false - down, true - up
     */
    boolean listTuningDirection() {
        return bitRead(data[0], 5);
    }

    boolean autoScanActive() {
        return bitRead(data[0], 3);
    }

    boolean rdsSearchActive() {
        return bitRead(data[0], 2);
    }

    boolean frequencyTuningActive() {
        return bitRead(data[0], 1);
    }

    boolean ptySearchActive() {
        return bitRead(data[0], 0);
    }

    /**
     * @return 0 - not in memory, 1...6 - memory slot number
     */
    uint8_t bandMemoryPosition() {
        return bitReadCount(data[1], 4, 3);
    }

    /**
     * 0 - ...
     * 1 - FM1
     * 2 - FM2
     * 4 - FM AST
     * 5 - AM
     * @return
     */
    uint8_t radioBandType() {
        return bitReadCount(data[2], 4, 3);
    }

    /**
     * 972 -> 98,6 MHz
     * X * 0.05 + 50 = F[MHz]
     * (F[MHz] - 50) * 0.05 = X
     * @return F[MHz] * 100
     */
    int radioFrequency() {
        uint8_t hi = data[3] & 0b00001111;
        uint8_t lo = data[4];
        int freq = (hi << 8) | lo;
        freq *= 5;
        freq += 5000;
        return freq;
    }

};