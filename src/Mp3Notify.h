#ifndef Mp3Notify_H
#define Mp3Notify_H

#include <Arduino.h>

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

#include <DFMiniMp3.h>
// https://github.com/Makuna/DFMiniMp3/blob/master/src/DFMiniMp3.h

#include "ICallback.h"

class Mp3Notify {
    public:
        static void setCallbackObj(ICallback *callback) {
            Mp3Notify::callbackObj = callback;
        }

        static void OnError(uint16_t errorCode) {
            // see DfMp3_Error for code meaning
            Log.notice(CR "Com Error %d" CR, errorCode);
        }

        static void OnPlayFinished(uint16_t track) {
            Log.notice("Finished Track: %d" CR, track);
            //Log.notice("Finished Track: S% " CR, Mp3CommandMap::getCommandByID(track));
            delay(100);
            Mp3Notify::callbackObj->notify(track);
        }

        static void OnCardOnline(uint16_t code) { Log.notice("SD Card is online."); }
        static void OnCardInserted(uint16_t code) { Log.notice("SD card has been inserted."); }
        static void OnCardRemoved(uint16_t code) { Log.notice("SD card has been removed."); }
        static void OnUsbOnline(uint16_t code) { Log.notice("USB is online."); }
        static void OnUsbInserted(uint16_t code) { Log.notice("USB is ready."); }
        static void OnUsbRemoved(uint16_t code) { Log.notice("USB has been removed."); }

    private:
        static ICallback *callbackObj;
};

#endif