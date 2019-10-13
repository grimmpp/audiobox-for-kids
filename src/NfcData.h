#ifndef NfcHelper_H
#define NfcHelper_H

#include <Arduino.h>

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

// this object stores nfc tag data
struct NfcTag {
  uint32_t cookie;
  uint8_t version;
  uint8_t folder;
  uint8_t mode; 
  uint8_t special;
};

class NFC_CARD_MODE {
public:
  enum ID : uint8_t {
    UNASSIGNED = 0,
    AUDIO_BOOK = 1,
    ALBUM = 2,
    PARTY = 3,
    SINGLE_TRACK = 4,
    AUDIO_BOOK_WITH_BOOKMARK = 5,
    ADMIN = 6
  };
};

void printNfcCardInfo(NfcTag nfcTag) {
  Log.notice(F("NFC Card: cookie: %d, version: %d, folder: %d, mode: %d, special: %d" CR),
    nfcTag.cookie, nfcTag.version, nfcTag.folder, nfcTag.mode, nfcTag.special);
}

#endif