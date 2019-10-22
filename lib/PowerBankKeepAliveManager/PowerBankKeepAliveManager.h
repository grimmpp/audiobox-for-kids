#ifndef POWER_BANK_KEEP_ALIVE_MANAGER_H
#define POWER_BANK_KEEP_ALIVE_MANAGER_H

#include <Arduino.h>

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

class PowerBankKeepAliveManager {
public:

    PowerBankKeepAliveManager(uint8_t pin, uint16_t durationMS, uint16_t delayMS) : 
        PIN(pin),
        DURATION_MS(durationMS),
        DELAY_MS(delayMS)
    { 
        pinMode(PIN, OUTPUT);
        digitalWrite(PIN, LOW);
    }

    void loop() {
        endOfDuration = millis();

        if (endOfDuration - startOfDuration < DURATION_MS) {
            Log.notice(F("Start triggering Power Bank."));
            digitalWrite(PIN, HIGH);
            delay(DELAY_MS);
            digitalWrite(PIN, LOW);
            Log.notice(F("Stop triggering Power Bank."));
        }
    }

private:
    uint8_t PIN = 8;
    uint16_t DURATION_MS = 5000;
    uint16_t DELAY_MS = 250;
    unsigned int startOfDuration = 0;
    unsigned int endOfDuration = 0;
};

#endif