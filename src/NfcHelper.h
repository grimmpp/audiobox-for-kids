#ifndef NfcHelper_H
#define NfcHelper_H

#include <Arduino.h>

// this object stores nfc tag data
struct NfcTag {
  uint32_t cookie;
  uint8_t version;
  uint8_t folder;
  uint8_t mode; 
  uint8_t special;
};

enum NFC_CARD_MODE {
  AUDIO_BOOK = 1,
  ALBUM = 2,
  PARTY = 3,
  SINGLE_TRACK = 4,
  AUDIO_BOOK_WITH_BOOKMARK = 5,
  ADMIN = 6
};

#endif