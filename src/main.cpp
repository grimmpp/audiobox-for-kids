#include <Arduino.h>
#include <EEPROM.h>

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

#include <JC_Button.h>

// local references
#include "NfcData.h"
#include "SystemSound.h"
#include "RfidReader.h"
#include "Mp3Player.h"
#include "Controller.h"

// serial interface for log messages
#define SERIAL_BAUD 115200

// Busy Pin for mp3 player
#define BUSY_PIN 4
// Software serial for mp3 player
#define MP3_PLAYER_RX_PIN 2
#define MP3_PLAYER_TX_PIN 3

// Button Pins
#define BUTTON_PAUSE_PIN A0
#define BUTTON_UP_PIN A1
#define BUTTON_DOWN_PIN A2

#define LONG_PRESS 500

// RFID Reader
#define RST_PIN 9
#define SS_PIN 10

// Mp3Player
#define INIT_VOLUME 10
#define MAX_VOLUME_LIMIT 20

NfcTag myCard;

uint8_t voiceMenu(uint16_t numberOfOptions, SystemSound::ID startMessage, uint8_t messageOffset,
              bool preview = false, uint8_t previewFromFolder = 0);
bool resetCard(void);
NfcTag setupCard(NfcTag oldCard);

Controller *controller;
Mp3Player * mp3Player;

RfidReader *rfidReader; // = RfidReader(SS_PIN, RST_PIN);

// Buttons
Button pauseButton(BUTTON_PAUSE_PIN);
Button upButton(BUTTON_UP_PIN);
Button downButton(BUTTON_DOWN_PIN);
// if button is press it will increasing or decreasing the volume in steps by a delay of LONG_PRESS
uint8_t buttonDelayFactor = 1;

uint8_t numberOfCards = 0;

void waitForButtonToBeReleased(Button b) {
  do { b.read(); }
  while (!b.wasReleased());
}

/**
 * Reset button delay for long push
 */
void resetButtonDelayForLongPress() {
    if (!downButton.isPressed() && !upButton.isPressed() && !pauseButton.isPressed()){
      buttonDelayFactor = 1;
    }
}

void restartOption() {
  if (upButton.pressedFor(LONG_PRESS) && downButton.pressedFor(LONG_PRESS)) {
    Log.notice(F("Restart Arduino!"));
    asm volatile ("  jmp 0"); 
  }
}

/**
 * Press all buttons for 1sec at start time to delete complete EEPROM
 */
void resetEEPROMOption() {
  if (digitalRead(BUTTON_PAUSE_PIN) == LOW && digitalRead(BUTTON_UP_PIN) == LOW && digitalRead(BUTTON_DOWN_PIN) == LOW) {
    Log.notice(F("Reset -> clean EEPROM"));
    for (uint16_t i = 0; i < EEPROM.length(); i++) {
      EEPROM.write(i, 0);
    }
  }
}

void readButtons() {
  pauseButton.read();
  upButton.read();
  downButton.read();
}

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
  pinMode(BUTTON_PAUSE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);

  // NFC Reader
  Log.trace(F("Init NFC reader: "));
  rfidReader = new RfidReader(SS_PIN, RST_PIN);
  rfidReader->init(true);

  resetEEPROMOption();
  
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
  
  Log.notice(F("Start welcome sound." CR));
  mp3Player->playSystemSounds(SystemSound::START_SOUND);
  mp3Player->waitUntilTrackIsFinished();

  Log.trace(F("Initialization completed." CR CR));
}

void loop() {
  Log.notice(F("In loop." CR));
  do {
    mp3Player->loop();

    readButtons();

    restartOption();

    if (pauseButton.pressedFor(LONG_PRESS)) {
      waitForButtonToBeReleased(pauseButton);

      if (mp3Player->isPlaying()) {
        Log.notice(F("Advertise current track." CR));
        mp3Player->playAdvertisement(mp3Player->getCurrentTrack());
      } else {
        // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
        if (resetCard()) myCard = setupCard(myCard);
        rfidReader->halt();
      }
    }
    else if (pauseButton.wasReleased()) {
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

    if (upButton.pressedFor(LONG_PRESS * buttonDelayFactor)) {
      Log.notice(F("Volume Up %d" CR), buttonDelayFactor);
      mp3Player->increaseVolume();
      buttonDelayFactor++;
      // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
    } else if (upButton.wasReleased()) {
      Log.notice(F("Next button was pressed." CR));
      // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
      controller->nextTrack(random(65536));
    }

    if (downButton.pressedFor(LONG_PRESS * buttonDelayFactor)) {
      Log.notice(F("Volume Down %d" CR), buttonDelayFactor);
      mp3Player->decreaseVolume();
      buttonDelayFactor++;
      // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
    } else if (downButton.wasReleased()) {
      Log.notice(F("Previous button was pressed." CR));
      // mp3Player->playAdvertisement(SystemSound::BUTTON_SOUND);
      // mp3Player->playAdvertisement(1);
      controller->previousTrack();
    }
    
    resetButtonDelayForLongPress();

  } while (!rfidReader->isNewCardPresent());

  // RFID card was placed
  Log.notice(F("New card has been detected. " CR));

  if (!rfidReader->readCardSerial()) {
    return;
  }

  if (rfidReader->readCard(&myCard)) {
      
    if (myCard.cookie == 322417479 && myCard.folder != 0 && myCard.mode != 0) {

        controller->playTrack(myCard);
    }
    else {
      // Configure new nfc card
      setupCard(myCard);
    }

    rfidReader->halt();
  }
}

void playPreview(SystemSound::ID messageOffset, bool preview, uint8_t previewFromFolder, uint8_t returnValue){
  mp3Player->playSystemSounds((SystemSound::ID) (messageOffset + returnValue));
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

  do {
    readButtons();
    mp3Player->loop();

    restartOption();

    if (upButton.pressedFor(LONG_PRESS*buttonDelayFactor)) {
      Log.notice(F("UpButton pressed for long." CR));
      buttonDelayFactor++;
      returnValue = min(returnValue + 10, numberOfOptions);
      playPreview(messageOffset, preview, previewFromFolder, returnValue);

    } else if (upButton.wasReleased()) {
      Log.notice(F("UpButton was released." CR));
      returnValue = min(returnValue + 1, numberOfOptions);
      playPreview(messageOffset, preview, previewFromFolder, returnValue);
    }
    
    if (downButton.pressedFor(LONG_PRESS*buttonDelayFactor)) {
      Log.notice(F("DownButton pressed for long." CR));
      buttonDelayFactor++;
      returnValue = max(returnValue - 10, 1);
      playPreview(messageOffset, preview, previewFromFolder, returnValue);

    } else if (downButton.wasReleased()) {
      Log.notice(F("DownButton was released." CR));
      returnValue = max(returnValue - 1, 1);
      playPreview(messageOffset, preview, previewFromFolder, returnValue);
    }

    resetButtonDelayForLongPress();

  } while ( !(returnValue != 0 && pauseButton.wasPressed()) );

  return returnValue;
}

bool resetCard() {
  Log.notice(F("Reset Nfc Card: " CR));
  mp3Player->playSystemSounds(SystemSound::RESET_TAG);

  do {
    readButtons();

    if (upButton.wasReleased() || downButton.wasReleased()) {
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

  // Ask for play mode
  Log.notice(F("Start menu to define play mode." CR));
  ncfCard.mode = voiceMenu(6, SystemSound::TAG_LINKED, SystemSound::TAG_LINKED);

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
