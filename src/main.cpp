#include <Arduino.h>
#include <EEPROM.h>

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

// local references
#include "NfcData.h"
#include "SystemSound.h"
#include "RfidReader.h"
#include "Mp3Player.h"
#include "ButtonManager.h"
#include "Controller.h"
#include "PowerBankKeepAliveManager.h"

// serial interface for log messages
#define SERIAL_BAUD 115200

// Busy Pin for mp3 player
#define BUSY_PIN 4
// Software serial for mp3 player
#define MP3_PLAYER_RX_PIN 2
#define MP3_PLAYER_TX_PIN 3

// Button Pins
#define BUTTON_PLAY_PIN A0
#define BUTTON_UP_PIN A1
#define BUTTON_DOWN_PIN A2

#define LONG_PRESS 500

// RFID Reader
#define RST_PIN 9
#define SS_PIN 10

// Mp3Player
#define INIT_VOLUME 10
#define MAX_VOLUME_LIMIT 20

//Power Bank Keep Alive
#define PB_KEEP_ALIVE_PIN 8
#define PB_KEEP_ALIVE_INTERVAL_MS 4000
#define PB_KEEP_ALIVE_PULS_LENGTH_MS 150

// timeout for menu.
// After timeout the program is going to go back into main loop
#define MENU_TIMEOUT_MS 60000 //60 sec

uint8_t voiceMenu(uint16_t numberOfOptions, SystemSound::ID startMessage, uint8_t messageOffset,
              bool preview = false, uint8_t previewFromFolder = 0);
bool resetCard(void);
NfcTag setupCard(NfcTag oldCard);
void resetEEPROMOption(void);
bool checkMenuTimeout(void);

NfcTag nfcCard;
Controller *controller;
Mp3Player * mp3Player;
RfidReader *rfidReader;
ButtonManager *buttonManager;
PowerBankKeepAliveManager *pbkaMgr;
unsigned long menuTimeStart = 0;


void setup() {

  Serial.begin(SERIAL_BAUD); 

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.notice(F(CR "**********************************" CR));
  Log.notice(F("Software - Philipp Grimm" CR));
  Log.notice(F("**********************************" CR CR));

  Log.trace(F("Start initializing hardware." CR));

  // Init random generator
  Log.trace(F("Init random generator." CR));
  randomSeed(analogRead(A0));

  // Button with PullUp
  Log.trace(F("Init pullup buttons." CR));
  buttonManager = new ButtonManager(LONG_PRESS, BUTTON_DOWN_PIN, BUTTON_PLAY_PIN, BUTTON_UP_PIN);

  // NFC Reader
  Log.trace(F("Init NFC reader: "));
  rfidReader = new RfidReader(SS_PIN, RST_PIN);
  rfidReader->init(true);

  resetEEPROMOption();

  Log.notice(F("Init PowerBankKeepAliveManager" CR));
  pbkaMgr = new PowerBankKeepAliveManager(PB_KEEP_ALIVE_PIN, PB_KEEP_ALIVE_INTERVAL_MS, PB_KEEP_ALIVE_PULS_LENGTH_MS);
  
  Log.notice(F("Init Controller" CR));
  controller = new Controller();

  // DFPlayer Mini initialisieren
  Log.notice(F("Init mp3 player." CR));
  mp3Player = new Mp3Player(MP3_PLAYER_RX_PIN, MP3_PLAYER_TX_PIN, BUSY_PIN, controller);
  Log.notice(F("Disable function sleep for mp3Player." CR));
  mp3Player->enableSleep(false);
  mp3Player->setVolume(INIT_VOLUME);
  mp3Player->setMaxVolumeLimit(MAX_VOLUME_LIMIT);

  controller->initMp3Player(mp3Player);
  
  delay(500);
  Log.notice(F("Start welcome sound." CR));
  mp3Player->playSystemSounds(SystemSound::START_SOUND);
  mp3Player->waitUntilTrackIsFinished();

  Log.trace(F("Initialization completed." CR CR));
}


void loop() {
  Log.notice(F("In loop." CR));
  do {
    pbkaMgr->loop();
    mp3Player->loop();
    buttonManager->readAllButtons();

    buttonManager->restartOption();

    if (buttonManager->isPlayButtonPressedForLong()) {
      buttonManager->waitForPlayButtonToBeReleased();

      if (mp3Player->isPlaying()) {
        Log.notice(F("Advertise current track." CR));
        mp3Player->playAdvertisement(mp3Player->getCurrentTrack());
      } else {
        // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
        if (resetCard()) {
          nfcCard = setupCard(nfcCard);
          controller->playTrack(nfcCard);
        }
        rfidReader->halt();
      }
    }
    else if (buttonManager->wasPlayButtonReleased()) {
      if (mp3Player->isPlaying()) {
        Log.notice(F("Pause current track." CR));
        mp3Player->pause();
        // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
      }
      else {
        Log.notice(F("Continue current track." CR));
        // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
        mp3Player->start();
      }
    }

    if (buttonManager->isUpButtonPressedForLong()) {
      Log.notice(F("Volume Up" CR));
      mp3Player->increaseVolume();
      // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
    } else if (buttonManager->wasUpButtonReleased()) {
      Log.notice(F("Next button was pressed." CR));
      // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
      controller->nextTrack(random(65536));
    }

    if (buttonManager->isDownButtonPressedForLong()) {
      Log.notice(F("Volume Down" CR));
      mp3Player->decreaseVolume();
      // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
    } else if (buttonManager->wasDownButtonReleased()) {
      Log.notice(F("Previous button was pressed." CR));
      // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
      // mp3Player->playAdvertisement(1);
      controller->previousTrack();
    }
    
    buttonManager->resetButtonDelayForLongPress();

  } while (!rfidReader->isNewCardPresent());

  // RFID card was placed
  Log.notice(F("New card has been detected. " CR));

  if (!rfidReader->readCardSerial()) {
    Log.notice(F("Cannot read card!" CR));
    return;
  }

  if (rfidReader->readCard(&nfcCard)) {
      
    if (nfcCard.cookie == 322417479 && nfcCard.folder != 0 && nfcCard.mode != 0) {
        controller->playTrack(nfcCard);
    }
    else {
      // Configure new nfc card
      nfcCard = setupCard(nfcCard);
      controller->playTrack(nfcCard);
    }

    rfidReader->halt();
  }
}

/**
 * Press all buttons for 1sec at start time to delete complete EEPROM
 */
void resetEEPROMOption() {
  if (digitalRead(BUTTON_PLAY_PIN) == LOW && digitalRead(BUTTON_UP_PIN) == LOW && digitalRead(BUTTON_DOWN_PIN) == LOW) {
    Log.notice(F("Reset -> clean EEPROM"));
    for (uint16_t i = 0; i < EEPROM.length(); i++) {
      EEPROM.write(i, 0);
    }
  }
}

bool checkMenuTimeout() {
  if (millis() - menuTimeStart > MENU_TIMEOUT_MS) {
    Log.notice(F("Exit menu because of timeout: %dms" CR), MENU_TIMEOUT_MS);
    return true;
  }
  return false;
}

void playPreview(SystemSound::ID messageOffset, bool preview, uint8_t previewFromFolder, uint8_t returnValue){
  mp3Player->playSystemSounds((SystemSound::ID) (messageOffset + returnValue));
  pbkaMgr->loop();
  delay(1000);
  if (preview) {
    do {
      delay(10);
    } while (mp3Player->isPlaying());

    if (previewFromFolder == 0) mp3Player->playFolderTrack(returnValue, 1);
    else mp3Player->playFolderTrack(previewFromFolder, returnValue);
  }
}

uint8_t voiceMenu(uint16_t numberOfOptions, SystemSound::ID startMessage, SystemSound::ID messageOffset,
              bool preview = false, uint8_t previewFromFolder = 0) {
  uint8_t returnValue = 0;
  if (startMessage != SystemSound::UNKNOWN) mp3Player->playSystemSounds(startMessage);

  menuTimeStart = millis();
  do {
    pbkaMgr->loop();
    mp3Player->loop();
    buttonManager->readAllButtons();
    buttonManager->restartOption();
    if (checkMenuTimeout()) return 0;

    if (buttonManager->isUpButtonPressedForLong()) {
      Log.notice(F("UpButton pressed for long." CR));
      returnValue = min(returnValue + 10, numberOfOptions);
      playPreview(messageOffset, preview, previewFromFolder, returnValue);

    } else if (buttonManager->wasUpButtonReleased()) {
      Log.notice(F("UpButton was released." CR));
      returnValue = min(returnValue + 1, numberOfOptions);
      playPreview(messageOffset, preview, previewFromFolder, returnValue);
    }
    
    if (buttonManager->isDownButtonPressedForLong()) {
      Log.notice(F("DownButton pressed for long." CR));
      returnValue = max(returnValue - 10, 1);
      playPreview(messageOffset, preview, previewFromFolder, returnValue);

    } else if (buttonManager->wasDownButtonReleased()) {
      Log.notice(F("DownButton was released." CR));
      returnValue = max(returnValue - 1, 1);
      playPreview(messageOffset, preview, previewFromFolder, returnValue);
    }

    buttonManager->resetButtonDelayForLongPress();

  } while ( !(returnValue != 0 && buttonManager->wasPlayButtonReleased()) );

  return returnValue;
}

bool resetCard() {
  Log.notice(F("Reset Nfc Card: " CR));
  mp3Player->playSystemSounds(SystemSound::RESET_TAG);

  menuTimeStart = millis();
  do {
    pbkaMgr->loop();
    buttonManager->readAllButtons();
    buttonManager->restartOption();
    if (checkMenuTimeout()) return false;

    if (buttonManager->wasUpButtonReleased() || buttonManager->wasDownButtonReleased()) {
      Log.notice(F("Reset Card was cancelled." CR));
      mp3Player->playSystemSounds(SystemSound::RESET_ABORTED);
      return false;
    }
  } while (!rfidReader->isNewCardPresent());

  if (!rfidReader->readCardSerial()) {
    mp3Player->playSystemSounds(SystemSound::ERROR);
    return false;
  }
  Log.notice(F("NFC Card was recognized!" CR));

  Log.notice(F("Reset card and write to card empty data." CR));
  NfcTag nfcCard;
  nfcCard.cookie = 0;
  nfcCard.folder = 0;
  nfcCard.mode = 0;
  nfcCard.special = 0;
  nfcCard.version = 0;

  if (rfidReader->writeCard(nfcCard)) {
    Log.notice(F("Nfc card was successfully reset." CR));
    return true;
  } 
  else {
    mp3Player->playSystemSounds(SystemSound::ERROR);
    return false;
  }
}

NfcTag setupCard(NfcTag oldCard) {
  NfcTag ncfCard = oldCard;
  mp3Player->pause();
  Log.notice(F("Configure new card:" CR));

  // Ask for mapping folder
  Log.notice(F("Start menu to define folder." CR));
  ncfCard.folder = voiceMenu(99, SystemSound::NEW_TAG, (SystemSound::ID)0, true);
  if (ncfCard.folder == 0) return oldCard;

  // Ask for play mode
  Log.notice(F("Start menu to define play mode." CR));
  ncfCard.mode = voiceMenu(6, SystemSound::TAG_LINKED, SystemSound::TAG_LINKED);
  if (ncfCard.mode == 0) return oldCard;

  // Create bookmark 
  Log.notice(F("Create bookmark for new folder" CR));
  EEPROM.write(ncfCard.folder,1);

  // Ask for single track in case of single track mode
  if (ncfCard.mode == NFC_CARD_MODE::SINGLE_TRACK) {
    Log.notice(F("Start menu to define single track." CR));
    ncfCard.special = voiceMenu(mp3Player->getFolderTrackCount(ncfCard.folder), 
                                SystemSound::SELECT_FILE, (SystemSound::ID)0,
                                true, ncfCard.folder);
  }

  // Admin Mode
  if (ncfCard.mode == NFC_CARD_MODE::ADMIN) {
    Log.notice(F("Start menu to define file for admin mode." CR));
    ncfCard.special = voiceMenu(3, SystemSound::SELECT_FILE, SystemSound::SELECT_FILE);
  }

  // Configuration completed ... write data to card
  mp3Player->pause();
  if (rfidReader->writeCard(ncfCard)) {
    Log.notice(F("Wrote data to nfc tag successfully."));
    return ncfCard;
  } 
  else {
    mp3Player->playSystemSounds(SystemSound::ERROR);
    return oldCard;
  }

}
