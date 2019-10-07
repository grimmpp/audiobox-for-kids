#ifndef Mp3Player_H
#define Mp3Player_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

#include <DFMiniMp3.h>
// https://github.com/Makuna/DFMiniMp3/blob/master/src/DFMiniMp3.h


class Mp3Player {
    public: 

        Mp3Player(uint8_t receivePin, uint8_t transmitPin) : softwareSerial(receivePin, transmitPin) {
            
        }

    private:
        SoftwareSerial softwareSerial;
};

#endif