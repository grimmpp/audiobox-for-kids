#ifndef Mp3Player_H
#define Mp3Player_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

#include <DFMiniMp3.h>
// https://github.com/Makuna/DFMiniMp3/blob/master/src/dfMiniMp3->h

#include "Mp3Notify.h"
#include "ICallback.h"

class Mp3Player  {
    public: 

        Mp3Player(uint8_t receivePin, uint8_t transmitPin, uint8_t busyPin, ICallback *callbackObj) {
            
                BUSY_PIN = busyPin;
                softwareSerial = new SoftwareSerial(receivePin, transmitPin);
                dfMiniMp3 = new DFMiniMp3<SoftwareSerial, Mp3Notify>(*softwareSerial);
                pinMode(busyPin, INPUT);

                Mp3Notify::setCallbackObj(callbackObj);

                dfMiniMp3->begin();
                dfMiniMp3->setVolume(_volume);
        }

        void setMaxVolumeLimit(uint8_t limit) {
            _maxVolumeLimit = min(30, max(limit, 0));
        }

        void setVolume(uint8_t volume) {
            _volume = min(_maxVolumeLimit, max(volume, 0));
            dfMiniMp3->setVolume(_volume);
        }

        void increaseVolume() {
            setVolume(_volume+1);
        }

        void decreaseVolume() {
            setVolume(_volume-1);
        }

        void sleep() {
            dfMiniMp3->sleep();
        }

        void loop() {
            dfMiniMp3->loop();
        }

        void pause() {
            dfMiniMp3->pause();
        }

        void start() {
            dfMiniMp3->start();
        }

        void playFolderTrack(uint8_t folder, uint8_t track) {
            dfMiniMp3->playFolderTrack(folder, track);
        }

        void playAdvertisement(uint16_t track) {
            dfMiniMp3->playAdvertisement(track);
        }

        //TODO: check error handling
        uint16_t getFolderTrackCount(uint16_t folder) {
            return dfMiniMp3->getFolderTrackCount(folder);
        }

        //TODO: change system uint16_t track to enum
        void playSystemSounds(uint16_t track) {
            dfMiniMp3->playMp3FolderTrack(track);
        }

        //TODO: Check if it is playing
        uint16_t getCurrentTrack() {
            return dfMiniMp3->getCurrentTrack();
        }

        bool isPlaying() { 
            return !digitalRead(BUSY_PIN); 
        }

    private:
        uint8_t _volume = 15;
        uint8_t _maxVolumeLimit = 30;
        uint8_t BUSY_PIN;
        SoftwareSerial *softwareSerial;
        DFMiniMp3<SoftwareSerial, Mp3Notify> *dfMiniMp3;
};

#endif