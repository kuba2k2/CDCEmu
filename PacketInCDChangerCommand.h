//
// Created by Kuba on 2019-07-09.
//

/**
 * sent periodically every 100ms
 */
class PacketInCDChangerCommand {
public:
    const static uint16_t ID = 0x131;
    const static uint8_t LENGTH = 5;
    uint8_t data[LENGTH] = {};

    explicit PacketInCDChangerCommand(can_frame msg) {
        for (int i = 0; i < msg.can_dlc; i++) {
            data[i] = msg.data[i];
        }
    }

    boolean radioEnabled() {
        return bitRead(data[0], 7);
    }

    boolean trackPrevious() {
        return bitRead(data[0], 6);
    }

    boolean trackNext() {
        return bitRead(data[0], 5);
    }

    boolean trackFastForward() {
        return bitRead(data[0], 4) && !trackPlaying() && trackIntroMode();
    }

    boolean trackFastBackward() {
        return !bitRead(data[0], 4) && !trackPlaying() && trackIntroMode();
    }

    boolean trackBackToStart() {
        return bitRead(data[0], 3);
    }

    boolean trackIntroMode() {
        return bitRead(data[0], 2);
    }

    boolean trackPlaying() {
        return bitRead(data[0], 1);
    }

    boolean trackRepeatMode() {
        return bitRead(data[1], 7);
    }

    boolean trackRandomMode() {
        return bitRead(data[1], 0);
    }

    uint8_t goToDisk() {
        return data[2];
    }

    uint8_t goToTrack() {
        return data[4];
    }
};