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

#define SERIAL_BAUD 115200

// DFPlayer Mini
uint16_t trackCountInFolder = 0;
uint16_t currentTrack = 0;
uint16_t currentMp3PlayerTrack = 0;
static const uint8_t INIT_VOLUME = 10;
static const uint8_t MAX_VOLUME_LIMIT = 20;

NfcTag myCard;

bool knownCard = false;

int voiceMenu(int numberOfOptions, int startMessage, int messageOffset,
              bool preview = false, int previewFromFolder = 0);
void resetCard(void);
void setupCard(void);
void nextTrack(uint16_t);

class Callback : public ICallback {
public:
  void notify(uint8_t track) {
    nextTrack(track);
  }
};
ICallback *callbackObj = new Callback();

// Busy Pin
#define BUSY_PIN 4
// Software serial
#define MP3_PLAYER_RX_PIN 2
#define MP3_PLAYER_TX_PIN 3

Mp3Player * mp3Player;

// Leider kann das Modul keine Queue abspielen.
static uint16_t _lastTrackFinished;

void nextTrack(uint16_t track) {
  // Log.notice("Play next track." CR);
  if (track == _lastTrackFinished) return;
   _lastTrackFinished = track;
   
   if (knownCard == false)
    // Wenn eine neue Karte angelernt wird soll das Ende eines Tracks nicht
    // verarbeitet werden
    return;

  switch(myCard.mode) {

    case NFC_CARD_MODE::AUDIO_BOOK:
      Serial.println(F("Hörspielmodus ist aktiv -> keinen neuen Track spielen"));
      mp3Player->sleep(); // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
      break;
  
    case NFC_CARD_MODE::ALBUM:
      Log.notice("Album mode is active: next track" CR);
      mp3Player->playNextSongInFolder();
      break;

    case NFC_CARD_MODE::PARTY:
      uint16_t oldTrack = currentTrack;
      currentTrack = random(1, trackCountInFolder + 1);
      if (currentTrack == oldTrack)
        currentTrack = currentTrack == trackCountInFolder ? 1 : currentTrack+1;
      Serial.print(F("Party Modus ist aktiv -> zufälligen Track spielen: "));
      Serial.println(currentTrack);
      mp3Player->playFolderTrack(myCard.folder, currentTrack);
      break;

    case NFC_CARD_MODE::SINGLE_TRACK: 
      Serial.println(F("Einzel Modus aktiv -> Strom sparen"));
      mp3Player->sleep();      // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
      break;
  
    case NFC_CARD_MODE::AUDIO_BOOK_WITH_BOOKMARK:
      if (currentTrack != trackCountInFolder) {
        currentTrack = currentTrack + 1;
        Serial.print(F("Hörbuch Modus ist aktiv -> nächster Track und "
                      "Fortschritt speichern"));
        Serial.println(currentTrack);
        mp3Player->playFolderTrack(myCard.folder, currentTrack);
        // Fortschritt im EEPROM abspeichern
        EEPROM.write(myCard.folder, currentTrack);
      } else {
        mp3Player->sleep();  // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
        // Fortschritt zurück setzen
        EEPROM.write(myCard.folder, 1);
      }
      break;
  }
}

static void previousTrack() {
  // Log.notice("Play previous track." CR);

  switch(myCard.mode) {
    case NFC_CARD_MODE::AUDIO_BOOK:
      Log.notice(F("Audio Book: Start same track from beginning." CR));
      mp3Player->startSameTrackFromBeginning();
      break;
  
    case NFC_CARD_MODE::ALBUM:
      Log.notice(F("Album mode: Start same track from beginning." CR));
      mp3Player->playPreviousSongInFolder();
      break;
  
    case NFC_CARD_MODE::PARTY:
      Log.notice(F("Party mode: Start same track from beginning." CR));
      mp3Player->startSameTrackFromBeginning();
      break;
  
    case NFC_CARD_MODE::SINGLE_TRACK:
      Log.notice(F("Single Track: Start same track from beginning." CR));
      mp3Player->startSameTrackFromBeginning();
      break;

    case NFC_CARD_MODE::AUDIO_BOOK_WITH_BOOKMARK:
      Log.notice(F("Audo Book with Bookmark: Play previous track." CR));
      mp3Player->playPreviousSongInFolder();
      // Save bookmark into eeprom
      EEPROM.write(myCard.folder, mp3Player->getCurrentTrack());
      break;
  }
}

// RFID Reader
#define RST_PIN 9                 // Configurable, see typical pin layout above
#define SS_PIN 10                 // Configurable, see typical pin layout above
RfidReader *rfidReader; // = RfidReader(SS_PIN, RST_PIN);

// Button Pins
#define BUTTON_PAUSE_PIN A0
#define BUTTON_UP_PIN A1
#define BUTTON_DOWN_PIN A2

#define LONG_PRESS 500

// Buttonss
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
    asm volatile ("  jmp 0"); 
  }
}

/**
 * Press all buttons for 1sec at start time to delete complete EEPROM
 */
void resetEEPROMOption() {
  if (digitalRead(BUTTON_PAUSE_PIN) == LOW && digitalRead(BUTTON_UP_PIN) == LOW && digitalRead(BUTTON_DOWN_PIN) == LOW) {
    Serial.println(F("Reset -> clean EEPROM"));
    for (uint16_t i = 0; i < EEPROM.length(); i++) {
      EEPROM.write(i, 0);
    }
  }
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
  
  // DFPlayer Mini initialisieren
  Log.trace(F("Init mp3 player." CR));
  mp3Player = new Mp3Player(MP3_PLAYER_RX_PIN, MP3_PLAYER_TX_PIN, BUSY_PIN, callbackObj);
  mp3Player->setVolume(INIT_VOLUME);
  mp3Player->setMaxVolumeLimit(MAX_VOLUME_LIMIT);
  mp3Player->playSystemSounds(SystemSound::START_SOUND);
  mp3Player->waitUntilTrackIsFinished();

  Log.trace(F("Initialization completed." CR CR));
}

void loop() {
  // Log.notice("loop start" CR);

  do {
    mp3Player->loop();

    pauseButton.read();
    upButton.read();
    downButton.read();

    restartOption();

    if (pauseButton.pressedFor(LONG_PRESS)) {
      waitForButtonToBeReleased(pauseButton);

      if (mp3Player->isPlaying()) {
        Log.notice(F("Advertise current track." CR));
        mp3Player->playAdvertisement(currentTrack);
      } else {
        knownCard = false;
        resetCard();
        rfidReader->halt();
      }
    }
    else if (pauseButton.wasReleased()) {
      if (mp3Player->isPlaying()) {
        Log.notice(F("Pause current track." CR));
        mp3Player->pause();
      } 
      else {
        Log.notice(F("Continue current track." CR));
        mp3Player->start();
      }
    }

    if (upButton.pressedFor(LONG_PRESS * buttonDelayFactor)) {
      Log.notice(F("Volume Up %d" CR), buttonDelayFactor);
      mp3Player->increaseVolume();
      buttonDelayFactor++;
    } else if (upButton.wasReleased()) {
      Log.notice(F("Next button was pressed." CR));
      nextTrack(random(65536));
    }

    if (downButton.pressedFor(LONG_PRESS * buttonDelayFactor)) {
      Log.notice(F("Volume Down %d" CR), buttonDelayFactor);
      mp3Player->decreaseVolume();
      buttonDelayFactor++;
    } else if (downButton.wasReleased()) {
      Log.notice(F("Previous button was pressed." CR));
      previousTrack();
    }
    
    resetButtonDelayForLongPress();

  } while (!rfidReader->isNewCardPresent());

  // RFID card was placed
  // Log.notice("New card was detected. " CR);

  if (!rfidReader->readCardSerial()) {
    return;
  }

  if (rfidReader->readCard(&myCard)) {
      
      if (myCard.cookie == 322417479 && myCard.folder != 0 && myCard.mode != 0) {

          knownCard = true;
          _lastTrackFinished = 0;
          // Log.notice("%d File(s) are in Folder %d" CR, trackCountInFolder, myCard.folder);

          // Hörspielmodus: eine zufällige Datei aus dem Ordner
          switch(myCard.mode) {
            case NFC_CARD_MODE::AUDIO_BOOK:
              Log.notice(F("Audio Book Mode is active. Random track will be played."CR));
              currentTrack = random(1, trackCountInFolder + 1);
              break;
          
          // Album Modus: kompletten Ordner spielen
          case NFC_CARD_MODE::ALBUM:
            Log.notice(F("Album Mode is active. All tracks in folder %d will be played." CR), myCard.folder);
            currentTrack = 1;
            break;
          
          // Party Modus: Ordner in zufälliger Reihenfolge
          case NFC_CARD_MODE::PARTY:
            Log.notice(F("Party Mode is active. Tracks from folder %d will be played randomly." CR), myCard.folder);
            currentTrack = random(1, trackCountInFolder + 1);
            break;

          // Einzel Modus: eine Datei aus dem Ordner abspielen
          case NFC_CARD_MODE::SINGLE_TRACK:
            Log.notice(F("Single Track Mode is active. Track %d in folder %d will be played." CR), myCard.special, myCard.folder);
            currentTrack = myCard.special;
            break;

          // Hörbuch Modus: kompletten Ordner spielen und Fortschritt merken
          case NFC_CARD_MODE::AUDIO_BOOK_WITH_BOOKMARK:
            Log.notice(F("Audio Book Mode with bookmarks is active. All tracks in the folder %d will be played one after the other and all completed tracks will be bookmarked." CR));
            currentTrack = EEPROM.read(myCard.folder);          
            break;
          }

          // Play track
          if (!mp3Player->playFolderTrack(myCard.folder, currentTrack)) {
            // in case of misconfigured tag. If there is e.g. a track configured which doesn't exist on sd card anymore.
            mp3Player->playSystemSounds(SystemSound::ERROR);
          }
      }

      // Neue Karte konfigurieren
      else {
        knownCard = false;
        setupCard();
      }

    rfidReader->halt();
  }
}

void playPreview(int messageOffset, bool preview, int previewFromFolder, int returnValue){
  mp3Player->playSystemSounds(messageOffset + returnValue);
  delay(1000);
  if (preview) {
    do {
      delay(10);
    } while (mp3Player->isPlaying());
    if (previewFromFolder == 0)
      mp3Player->playFolderTrack(returnValue, 1);
    else
      mp3Player->playFolderTrack(previewFromFolder, returnValue);
  }
}


int voiceMenu(int numberOfOptions, int startMessage, int messageOffset,
              bool preview = false, int previewFromFolder = 0) {
  int returnValue = 0;
  if (startMessage != 0) mp3Player->playSystemSounds(startMessage);

  do {
    pauseButton.read();
    upButton.read();
    downButton.read();
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

void resetCard() {
  Log.notice(F("Reset Nfc Card: " CR));
  mp3Player->playSystemSounds(SystemSound::RESET_TAG);

  do {
    pauseButton.read();
    upButton.read();
    downButton.read();

    if (upButton.wasReleased() || downButton.wasReleased()) {
      Log.notice(F("Reset Card was cancelled." CR));
      mp3Player->playSystemSounds(SystemSound::RESET_ABORTED);
      return;
    }
  } while (!rfidReader->isNewCardPresent());

  if (!rfidReader->readCardSerial()) {
    mp3Player->playSystemSounds(SystemSound::ERROR);
    return;
  }

  Log.notice(F("NFC Card was recognized!" CR));
  setupCard();
}

void setupCard() {
  mp3Player->pause();
  Log.notice(F("Configure new card:" CR));

  // Ask for mapping folder
  myCard.folder = voiceMenu(99, SystemSound::NEW_TAG, 0, true);

  // Ask for play mode
  myCard.mode = voiceMenu(6, SystemSound::TAG_LINKED, SystemSound::TAG_LINKED);

  // Create bookmark 
  EEPROM.write(myCard.folder,1);

  // Ask for single track in case of single track mode
  if (myCard.mode == NFC_CARD_MODE::SINGLE_TRACK) {
    myCard.special = voiceMenu(mp3Player->getFolderTrackCount(myCard.folder), 
                                SystemSound::SELECT_FILE, 0,
                                true, myCard.folder);
  }

  // Admin Mode
  if (myCard.mode == NFC_CARD_MODE::ADMIN) {
    myCard.special = voiceMenu(3, SystemSound::SELECT_FILE, SystemSound::SELECT_FILE);
  }

  // Configuration completed
  mp3Player->pause();
  rfidReader->writeCard(myCard);
}
