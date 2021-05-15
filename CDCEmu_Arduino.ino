#include <Arduino.h>
#include <math.h>

uint8_t bitReadCount(uint8_t value, uint8_t bit, uint8_t count)  {
    return (((value) >> (bit)) ^ (uint8_t)pow(2, count));
}

#include <SPI.h>
#include <mcp2515.h>
#include <avr/io.h>
#include "PacketInCDChangerCommand.h"
#include "PacketInRadioFrequency.h"
#include "PacketInRadioStatus.h"
#include "PacketOutCurrentDisk.h"
#include "PacketOutCurrentDiskNumber.h"
#include "PacketOutCurrentDiskTrackCount.h"
#include "PacketOutCurrentTrack.h"
#include "PacketOutStatus.h"

MCP2515 mcp2515(10);

struct can_frame can_msg;

void setup() {
    Serial.begin(115200);
    SPIClass::begin();

    mcp2515.reset();
    mcp2515.setBitrate(CAN_125KBPS, MCP_8MHZ);
    mcp2515.setNormalMode();

    //Serial.println("Initialized");
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
    /*if (id == PacketInRadioFrequency::ID) {
        PacketInRadioFrequency packet = PacketInRadioFrequency(msg);
        if (packet.radioFrequency() != radioFrequency) {
            radioFrequency = packet.radioFrequency();
            Serial.print("Radio frequency ");
            Serial.print(radioFrequency / 100);
            Serial.println(" MHz");
        }
    }*/
    if (id == PacketInCDChangerCommand::ID) {
        PacketInCDChangerCommand packet = PacketInCDChangerCommand(msg);
        if (packet.radioEnabled() != radioEnabled) {
            radioEnabled = !radioEnabled;
            Serial.print("Radio enabled");
            Serial.println(radioEnabled);
        }
        if (packet.goToTrack() != 0 && packet.goToTrack() != trackNumber) {
            trackNumber = packet.goToTrack();
            Serial.print("Go to track ");
            Serial.println(trackNumber);
        }
        if (packet.goToDisk() != 0 && packet.goToDisk() != diskNumber) {
            diskNumber = packet.goToDisk();
            Serial.print("Go to disk ");
            Serial.println(diskNumber);
        }
        if (packet.trackPrevious()) {
            Serial.println("Go to previous track");
            trackNumber--;
        }
        if (packet.trackNext()) {
            Serial.println("Go to next track");
            trackNumber++;
        }
        if (packet.trackBackToStart()) {
            Serial.println("Back to start");
        }
        if (packet.trackPlaying() != trackPlaying) {
            trackPlaying = !trackPlaying;
            Serial.print("Track playing ");
            Serial.println(trackPlaying);
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