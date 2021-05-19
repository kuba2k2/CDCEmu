#include "config.h"

#ifdef ATTINY_CORE
    #include <pins_arduino.h>
    #define USE_SOFTWARE_SERIAL 0
    #define DISABLE_UART 1
#endif

#include <Arduino.h>
#include <math.h>

uint8_t bitReadCount(uint8_t value, uint8_t bit, uint8_t count)  {
    return (((value) >> (bit)) ^ (uint8_t)pow(2, count));
}

#include "mcp2515.h"
#include "PacketInCDChangerCommand.h"
#include "PacketInRadioFrequency.h"
#include "PacketInRadioStatus.h"
#include "PacketOutCurrentDisk.h"
#include "PacketOutCurrentDiskNumber.h"
#include "PacketOutCurrentDiskTrackCount.h"
#include "PacketOutCurrentTrack.h"
#include "PacketOutStatus.h"

#if DEBUG
    #include "uart.h"
#else
    #define print
#endif

MCP2515 mcp2515(MCP_CS_PIN);

struct can_frame can_msg;

void setup() {
    print("reset(): %d\n", mcp2515.reset());
    print("setBitrate(): %d\n", mcp2515.setBitrate(CAN_125KBPS));
    print("setNormalMode(): %d\n", mcp2515.setNormalMode());
    // println("Initialized");
}

boolean radioEnabled = false;
boolean trackPlaying = false;
uint8_t diskNumber = 1;
uint8_t trackNumber = 1;
uint8_t trackMinute = 13;
uint8_t trackSecond = 37;
uint8_t icon = STATUS_ICON_PLAY1;
boolean introMode = false;
boolean repeatMode = false;
boolean randomMode = false;
uint8_t trackCount = 69;

int radioFrequency = 0;

void processPacket(struct can_frame msg) {
    uint8_t id = msg.can_id;
    if (id == PacketInRadioFrequency::ID) {
        PacketInRadioFrequency packet = PacketInRadioFrequency(msg);
        if (packet.radioFrequency() != radioFrequency) {
            radioFrequency = packet.radioFrequency();
            print("Radio frequency %.2f MHz\n", radioFrequency / 100.0);
        }
    }
    if (id == PacketInCDChangerCommand::ID) {
        PacketInCDChangerCommand packet = PacketInCDChangerCommand(msg);
        if (packet.radioEnabled() != radioEnabled) {
            radioEnabled = !radioEnabled;
            print("Radio enabled %d\n", radioEnabled);
        }
        if (packet.goToTrack() != 0 && packet.goToTrack() != trackNumber) {
            trackNumber = packet.goToTrack();
            print("Go to track %d\n", trackNumber);
        }
        if (packet.goToDisk() != 0 && packet.goToDisk() != diskNumber) {
            diskNumber = packet.goToDisk();
            print("Go to disk %d\n", diskNumber);
        }
        if (packet.trackPrevious()) {
            print("Go to previous track\n");
            trackNumber--;
        }
        if (packet.trackNext()) {
            print("Go to next track\n");
            trackNumber++;
        }
        if (packet.trackBackToStart()) {
            print("Back to start\n");
        }
        if (packet.trackPlaying() != trackPlaying) {
            trackPlaying = !trackPlaying;
            print("Track playing %d\n", trackPlaying);
        }
    }
    //else if (id == )
}

long counter100 = 0;
long counter500 = 0;
long counter1000 = 0;

void loop() {

    if (mcp2515.readMessage(&can_msg) == MCP2515::ERROR_OK) {
        processPacket(can_msg);
    }

    if (radioEnabled || true) {

        if (millis() - counter100 > 100) {
            counter100 = millis();
            PacketOutCurrentDisk packet = PacketOutCurrentDisk(); /* required to add cd changer to sources */
            packet.setDiskNumber(diskNumber);
            packet.setDiskChanged(false);
            packet.setRadioEnabled(radioEnabled);
            packet.setStatusIcon(icon);
            packet.send(mcp2515);
        }

        /*if (millis() - counter500 > 500) {
            counter500 = millis();
            PacketOutCurrentDiskTrackCount packet1 = PacketOutCurrentDiskTrackCount();
            packet1.setTrackCount(trackCount);
            packet1.send(mcp2515);

            PacketOutCurrentTrack packet2 = PacketOutCurrentTrack();
            packet2.setTrackMinute(trackMinute);
            packet2.setTrackNumber(trackNumber);
            packet2.setTrackSecond(trackSecond);
            packet2.send(mcp2515);

            PacketOutStatus packet3 = PacketOutStatus();
            packet3.setIntroMode(introMode);
            packet3.setRandomMode(randomMode);
            packet3.setRepeatMode(repeatMode);
            packet3.send(mcp2515);
        }

        if (millis() - counter1000 > 1000) {
            counter1000 = millis();
            PacketOutCurrentDiskNumber packet1 = PacketOutCurrentDiskNumber();
            packet1.setDiskNumber(diskNumber);
            packet1.send(mcp2515);
        }*/
    }
}
