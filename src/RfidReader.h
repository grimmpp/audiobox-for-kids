#ifndef Rfid_Reader_H
#define Rfid_Reader_H

#include <Arduino.h>
#include <SPI.h>

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

#include <MFRC522.h>

// local references
#include "NfcData.h"

class RfidReader {
  public:

    RfidReader(byte chipSelectPin, byte resetPowerDownPin) {
      mfrc522 = MFRC522(chipSelectPin, resetPowerDownPin);
    }

    void init(bool dumpVersion = false) {
      SPI.begin();
      mfrc522.PCD_Init();
      if (dumpVersion) {
        mfrc522.PCD_DumpVersionToSerial();
        for (byte i = 0; i < 6; i++) {
          key.keyByte[i] = 0xFF;
        }
      }
    }

    /**
     * 
     */
    bool readCard(NfcTag *nfcTag) {
      bool returnValue = true;
      // Show some details of the PICC (that is: the tag/card)
      Log.notice("Read NFC Card:" CR);
      Log.notice("Card UID: %b" CR, mfrc522.uid.uidByte);
      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);

      Log.notice(CR "PICC type: %s" CR, mfrc522.PICC_GetTypeName(mfrc522.PICC_GetType(mfrc522.uid.sak)));
      // MFRC522::PICC_Type piccType = ;

      byte buffer[18];
      byte size = sizeof(buffer);

      // Authenticate using key A
      Serial.println(F("Authenticating using key A..."));
      MFRC522::StatusCode status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(
          MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        returnValue = false;
        return returnValue;
      }

      // Show the whole sector as it currently is
      Serial.println(F("Current data in sector:"));
      mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
      Serial.println();

      // Read data from the block
      Serial.print(F("Reading data from block "));
      Serial.print(blockAddr);
      Serial.println(F(" ..."));
      status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        returnValue = false;
      }
      Serial.print(F("Data in block "));
      Serial.print(blockAddr);
      Serial.println(F(":"));
      dump_byte_array(buffer, 16);
      Serial.println();
      Serial.println();

      uint32_t tempCookie;
      tempCookie = (uint32_t)buffer[0] << 24;
      tempCookie += (uint32_t)buffer[1] << 16;
      tempCookie += (uint32_t)buffer[2] << 8;
      tempCookie += (uint32_t)buffer[3];

      nfcTag->cookie = tempCookie;
      nfcTag->version = buffer[4];
      nfcTag->folder = buffer[5];
      nfcTag->mode = buffer[6];
      nfcTag->special = buffer[7];

      return returnValue;
    }

    bool writeCard(NfcTag nfcTag) {
      byte buffer[16] = {0x13, 0x37, 0xb3, 0x47, // 0x1337 0xb347 magic cookie to
                                                // identify our nfc tags
                        0x01,                   // version 1
                        nfcTag.folder,          // the folder picked by the user
                        nfcTag.mode,            // the playback mode picked by the user
                        nfcTag.special,         // track or function for admin cards
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

      //TODO: Needed?
      // MFRC522::PICC_Type mifareType = mfrc522.PICC_GetType(mfrc522.uid.sak);

      // Authenticate using key B
      // Log.notice("Authenticating again using key B..." CR);
      MFRC522::StatusCode status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(
          MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
        Log.error("NFC Card PCD_Authenticate() failed: %s" CR, mfrc522.GetStatusCodeName(status));
        // mp3Player.playMp3FolderTrack(Mp3CommandMap::COMMAND_ID_ERROR);
        return false;
      }

      // Write data to the block
      Log.notice("Writing data into block: %d " CR, blockAddr);
      Log.notice(" ..." CR);
      // byte size = sizeof(buffer);
      dump_byte_array(buffer, 16); 
      Log.notice("" CR);

      status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, buffer, 16);
      return status == MFRC522::STATUS_OK;
      // if (status != MFRC522::STATUS_OK) {
      //   Log.notice("MIFARE_Write() failed: %s" CR, mfrc522.GetStatusCodeName(status));
      //   mp3Player.playMp3FolderTrack(Mp3CommandMap::COMMAND_ID_ERROR);
      // }
      // else {
        // mp3Player.playMp3FolderTrack(Mp3CommandMap::COMMAND_ID_TAG_OK);
      // }
      // Serial.println();
      // delay(100);
    }

    bool isNewCardPresent() {
      return mfrc522.PICC_IsNewCardPresent();
    }

    bool readCardSerial() {
      return mfrc522.PICC_ReadCardSerial();
    }

    void halt() {
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }

  private:
    MFRC522 mfrc522;
    MFRC522::MIFARE_Key key;
    static const byte sector = 1;
    static const byte blockAddr = 4;
    static const byte trailerBlock = 7;

    /**
     * Helper routine to dump a byte array as hex values to Serial.
     */
    void dump_byte_array(byte *buffer, byte bufferSize) {
      for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
      }
    }
};

#endif