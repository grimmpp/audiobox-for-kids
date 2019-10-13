#ifndef Mp3Player_H
#define Mp3Player_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

#include <DFMiniMp3.h>
// https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299
// https://github.com/Makuna/DFMiniMp3

#include "Mp3Notify.h"
#include "ICallback.h"
#include "SystemSound.h"

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

        uint8_t getVolumen() {
            return _volume;
        }

        void sleep() {
            dfMiniMp3->sleep();
        }

        void loop() {
            dfMiniMp3->loop();

            // if (!this->isPlaying()) resetTrackValues();
        }

        void waitUntilTrackIsFinished() {
            while(this->isPlaying()) delay(10);
        }

        void pause() {
            dfMiniMp3->pause();
        }

        void start() {
            dfMiniMp3->start();
        }

        bool playFolderTrack(uint8_t folder, uint8_t track) {
            currentFolder = folder;
            currentTrack = track;

            trackCountInFolder = dfMiniMp3->getFolderTrackCount(currentFolder);

            if (trackCountInFolder > 0 && trackCountInFolder >= track) {
                currentTrackIsSystemSound = false;
                dfMiniMp3->playFolderTrack(currentFolder, currentTrack);
                delay(250);
                currentMp3PlayerTrackId = dfMiniMp3->getCurrentTrack();
                Log.notice(F("Play track %d (dfPlayerTrackId: %d) of %d track(s) in folder %d." CR), 
                    currentTrack, currentMp3PlayerTrackId, trackCountInFolder, currentFolder);
                return true;

            } 
            // else {                
            //     resetTrackValues();
            //     return false;
            // }
        }

        void playAdvertisement(uint16_t track) {
            Log.notice(F("Play advert: %d" CR), track);
            dfMiniMp3->playAdvertisement(track);
        }

        //TODO: check error handling
        uint16_t getFolderTrackCount(uint16_t folder) {
            return dfMiniMp3->getFolderTrackCount(folder);
        }

        void playSystemSounds(SystemSound::ID systemSoundId) {
            // resetTrackValues();

            currentTrackIsSystemSound = true;
            dfMiniMp3->playMp3FolderTrack(systemSoundId);
            delay(250);
            currentMp3PlayerTrackId = dfMiniMp3->getCurrentTrack();

            Log.notice(F("Play system sound: %s (Id: %d, dfPlayerTrackId: %d)" CR), 
                SystemSound::GetSystemSoundById(systemSoundId), systemSoundId, currentMp3PlayerTrackId);
        }

        uint16_t getCurrentTrack() {
            return currentTrack;
        }

        uint16_t getCurrentFolder() {
            return currentFolder;
        }

        bool startSameTrackFromBeginning() {
            Log.notice(F("Play same track: currentFolder %d, currentTrack %d" CR), currentFolder, currentTrack);
            if (currentFolder != 0 || currentTrack != 0) {
                playFolderTrack(currentFolder, currentTrack);
                return true;
            } else {
                dfMiniMp3->sleep();
                return false;
            }
        }

        bool isPlaying() { 
            return !digitalRead(BUSY_PIN); 
        }

        bool playNextSongInFolder() {
            Log.notice(F("Play next track." CR));
            if (currentTrack > 0) {
                if (currentTrack < trackCountInFolder) {
                    return playFolderTrack(currentFolder, currentTrack+1);
                }
            }
            return false;
        }

        bool playPreviousSongInFolder() {
            Log.notice(F("Play previous track: currentFolder %d, currentTrack %d" CR), currentFolder, currentTrack);
            if (currentTrack > 1) {
                playFolderTrack(currentFolder, currentTrack-1);
            } else if (currentTrack == 1) {
                startSameTrackFromBeginning();
            } else {
                sleep();
                return false;
            }
            return true;
        }

        bool isCurrentTrackSystemSound() {
            return currentTrackIsSystemSound;
        }

    private:
        uint16_t currentFolder = 0;
        uint16_t currentTrack = 0;
        uint16_t trackCountInFolder = 0;
        uint16_t currentMp3PlayerTrackId = 0;
        bool currentTrackIsSystemSound = false;

        uint8_t _volume = 15;
        uint8_t _maxVolumeLimit = 30;
        uint8_t BUSY_PIN;
        SoftwareSerial *softwareSerial;
        DFMiniMp3<SoftwareSerial, Mp3Notify> *dfMiniMp3;

        void resetTrackValues() {
            currentFolder = 0;
            currentTrack = 0;
            trackCountInFolder = 0;
            currentMp3PlayerTrackId = 0;
        }
};

#endif