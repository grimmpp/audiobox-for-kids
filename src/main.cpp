#include <Arduino.h>
#include <EEPROM.h>

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

#include <DFMiniMp3.h>
// https://github.com/Makuna/DFMiniMp3/blob/master/src/DFMiniMp3.h

#include <JC_Button.h>
#include <MFRC522.h>
#include <SPI.h>
#include <SoftwareSerial.h>

// local references
#include "NfcData.h"
#include "Mp3CommandMap.h"
#include "RfidReader.h"
#include "Mp3Player.h"

// DFPlayer Mini
SoftwareSerial mySoftwareSerial(2, 3); // RX, TX
uint16_t numTracksInFolder = 0;
uint16_t currentTrack = 0;
uint16_t currentMp3PlayerTrack = 0;
static const uint8_t INIT_VOL = 15;

NfcTag myCard;

bool knownCard = false;

static void nextTrack(uint16_t track);
int voiceMenu(int numberOfOptions, int startMessage, int messageOffset,
              bool preview = false, int previewFromFolder = 0);
void resetCard(void);
// bool readCard(NfcTag *nfcTag);
void setupCard(void);
// void writeCard(NfcTag nfcTag);
// void dump_byte_array(byte *buffer, byte bufferSize);

class Mp3Notify {
public:
  static void OnError(uint16_t errorCode) {
    // see DfMp3_Error for code meaning
    Log.notice(CR "Com Error %d" CR, errorCode);
  }
  static void OnPlayFinished(uint16_t track) {
    Log.notice("Finished Track: %d" CR, track);
    //Log.notice("Finished Track: S% " CR, Mp3CommandMap::getCommandByID(track));
    delay(100);
    nextTrack(track);
  }

  static void OnCardOnline(uint16_t code) { Log.notice("SD Card is online."); }
  static void OnCardInserted(uint16_t code) { Log.notice("SD card has been inserted."); }
  static void OnCardRemoved(uint16_t code) { Log.notice("SD card has been removed."); }
  static void OnUsbOnline(uint16_t code) { Log.notice("USB is online."); }
  static void OnUsbInserted(uint16_t code) { Log.notice("USB is ready."); }
  static void OnUsbRemoved(uint16_t code) { Log.notice("USB has been removed."); }
};

static DFMiniMp3<SoftwareSerial, Mp3Notify> mp3Player(mySoftwareSerial);

// Leider kann das Modul keine Queue abspielen.
static uint16_t _lastTrackFinished;

static void nextTrack(uint16_t track) {
  if (track == _lastTrackFinished) {
    return;
   }
   _lastTrackFinished = track;
   
   if (knownCard == false)
    // Wenn eine neue Karte angelernt wird soll das Ende eines Tracks nicht
    // verarbeitet werden
    return;

  switch(myCard.mode) {

    case NFC_CARD_MODE::AUDIO_BOOK:
      Serial.println(F("Hörspielmodus ist aktiv -> keinen neuen Track spielen"));
      mp3Player.sleep(); // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
      break;
  
    case NFC_CARD_MODE::ALBUM:
      if (currentTrack != numTracksInFolder) {
        currentTrack = currentTrack + 1;
        mp3Player.playFolderTrack(myCard.folder, currentTrack);
        Serial.print(F("Albummodus ist aktiv -> nächster Track: "));
        Serial.print(currentTrack);
      } else {
        mp3Player.sleep();   // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
      }
      break;

    case NFC_CARD_MODE::PARTY:
      uint16_t oldTrack = currentTrack;
      currentTrack = random(1, numTracksInFolder + 1);
      if (currentTrack == oldTrack)
        currentTrack = currentTrack == numTracksInFolder ? 1 : currentTrack+1;
      Serial.print(F("Party Modus ist aktiv -> zufälligen Track spielen: "));
      Serial.println(currentTrack);
      mp3Player.playFolderTrack(myCard.folder, currentTrack);
      break;

    case NFC_CARD_MODE::SINGLE_TRACK:
      Serial.println(F("Einzel Modus aktiv -> Strom sparen"));
      mp3Player.sleep();      // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
      break;
  
    case NFC_CARD_MODE::AUDIO_BOOK_WITH_BOOKMARK:
      if (currentTrack != numTracksInFolder) {
        currentTrack = currentTrack + 1;
        Serial.print(F("Hörbuch Modus ist aktiv -> nächster Track und "
                      "Fortschritt speichern"));
        Serial.println(currentTrack);
        mp3Player.playFolderTrack(myCard.folder, currentTrack);
        // Fortschritt im EEPROM abspeichern
        EEPROM.write(myCard.folder, currentTrack);
      } else {
        mp3Player.sleep();  // Je nach Modul kommt es nicht mehr zurück aus dem Sleep!
        // Fortschritt zurück setzen
        EEPROM.write(myCard.folder, 1);
      }
      break;
  }
}

static void previousTrack() {

  switch(myCard.mode) {
    case NFC_CARD_MODE::AUDIO_BOOK:
      Serial.println(F("Hörspielmodus ist aktiv -> Track von vorne spielen"));
      mp3Player.playFolderTrack(myCard.folder, currentTrack);
      break;
  
    case NFC_CARD_MODE::ALBUM:
      Serial.println(F("Albummodus ist aktiv -> vorheriger Track"));
      if (currentTrack != 1) {
        currentTrack = currentTrack - 1;
      }
      mp3Player.playFolderTrack(myCard.folder, currentTrack);
      break;
  
    case NFC_CARD_MODE::PARTY:
      Serial.println(F("Party Modus ist aktiv -> Track von vorne spielen"));
      mp3Player.playFolderTrack(myCard.folder, currentTrack);
      break;
  
    case NFC_CARD_MODE::SINGLE_TRACK:
      Serial.println(F("Einzel Modus aktiv -> Track von vorne spielen"));
      mp3Player.playFolderTrack(myCard.folder, currentTrack);
      break;

    case NFC_CARD_MODE::AUDIO_BOOK_WITH_BOOKMARK:
      Serial.println(F("Hörbuch Modus ist aktiv -> vorheriger Track und Fortschritt speichern"));
      if (currentTrack != 1) {
        currentTrack = currentTrack - 1;
      }
      mp3Player.playFolderTrack(myCard.folder, currentTrack);
      // Fortschritt im EEPROM abspeichern
      EEPROM.write(myCard.folder, currentTrack);
      break;
  }
}

// RFID Reader
#define RST_PIN 9                 // Configurable, see typical pin layout above
#define SS_PIN 10                 // Configurable, see typical pin layout above
RfidReader rfidReader = RfidReader(SS_PIN, RST_PIN);

// Button Pins
#define buttonPause A0
#define buttonUp A1
#define buttonDown A2

// Busy Pin
#define busyPin 4

#define LONG_PRESS 1000

// Buttonss
Button pauseButton(buttonPause);
Button upButton(buttonUp);
Button downButton(buttonDown);
bool ignorePauseButton = false;
bool ignoreUpButton = false;
bool ignoreDownButton = false;

uint8_t numberOfCards = 0;

bool isPlaying() { return !digitalRead(busyPin); }

void restartOption() {
  if (upButton.pressedFor(LONG_PRESS) && downButton.pressedFor(LONG_PRESS)) {
    asm volatile ("  jmp 0"); 
  }
}

void setup() {

  Serial.begin(115200); // Es gibt ein paar Debug Ausgaben über die serielle
                        // Schnittstelle
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.notice(CR "**********************************" CR);
  Log.notice("TonUINO Version 2.0 - Thorsten Voß" CR);
  Log.notice("Software - Philipp Grimm" CR);
  Log.notice("**********************************" CR CR);

  Log.trace("Start initializing hardware." CR);

  // Init random generator
  // Log.trace("Init random generator." CR);
  randomSeed(analogRead(A0));

  // Button with PullUp
  // Log.trace("Init pullup buttons." CR);
  pinMode(buttonPause, INPUT_PULLUP);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);

  // Busy Pin
  // Log.trace("Init busy pin from mp3 module." CR);
  pinMode(busyPin, INPUT);

  // NFC Reader
  // Log.trace("Init NFC reader: ");
  // SPI.begin();        // Init SPI bus
  // mfrc522.PCD_Init(); // Init MFRC522
  // mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader
  // for (byte i = 0; i < 6; i++) {
  //   key.keyByte[i] = 0xFF;
  // }
  rfidReader.init();

  // RESET --- ALLE DREI KNÖPFE BEIM STARTEN GEDRÜCKT HALTEN -> alle bekannten
  // Karten werden gelöscht
  if (digitalRead(buttonPause) == LOW && digitalRead(buttonUp) == LOW && digitalRead(buttonDown) == LOW) {
    Serial.println(F("Reset -> EEPROM wird gelöscht"));
    for (int i = 0; i < EEPROM.length(); i++) {
      EEPROM.write(i, 0);
    }
  }

  // DFPlayer Mini initialisieren
  // Log.trace("Init mp3 player." CR);
  mp3Player.begin();
  // Log.notice("Set volume to %d" CR, INIT_VOL);
  mp3Player.setVolume(INIT_VOL);
  // Log.notice("Play start sound: %s " CR, Mp3CommandMap::getCommandByID(Mp3CommandMap::COMMAND_ID_START_SOUND));
  mp3Player.playMp3FolderTrack(Mp3CommandMap::COMMAND_ID_START_SOUND);
  
  Log.trace("Initialization completed." CR CR);
}

void loop() {
  // Log.notice("loop start" CR);

  do {
    mp3Player.loop();
    // Buttons werden nun über JS_Button gehandelt, dadurch kann jede Taste
    // doppelt belegt werden
    pauseButton.read();
    upButton.read();
    downButton.read();

    restartOption();

    if (pauseButton.wasReleased()) {
      if (ignorePauseButton == false) {
        if (isPlaying())
          mp3Player.pause();
        else
          mp3Player.start();
      }
      ignorePauseButton = false;
    } 
    else if (pauseButton.pressedFor(LONG_PRESS) && ignorePauseButton == false) {
      if (isPlaying()) {
        mp3Player.playAdvertisement(currentTrack);
      } else {
        knownCard = false;
        mp3Player.playMp3FolderTrack(Mp3CommandMap::COMMAND_ID_RESET_TAG);
        Log.notice("Reset Nfc Card: "CR);
        resetCard();
        rfidReader.halt();
      }
      ignorePauseButton = true;
    }

    if (upButton.pressedFor(LONG_PRESS)) {
      Serial.println(F("Volume Up"));
      mp3Player.increaseVolume();
      ignoreUpButton = true;
    } else if (upButton.wasReleased()) {
      if (!ignoreUpButton)
        nextTrack(random(65536));
      else
        ignoreUpButton = false;
    }

    if (downButton.pressedFor(LONG_PRESS)) {
      Serial.println(F("Volume Down"));
      mp3Player.decreaseVolume();
      ignoreDownButton = true;
    } else if (downButton.wasReleased()) {
      if (!ignoreDownButton)
        previousTrack();
      else
        ignoreDownButton = false;
    }
  } while (!rfidReader.isNewCardPresent());

  // RFID card was placed
  // Log.notice("New card was detected. " CR);

  if (!rfidReader.readCardSerial()) {
    return;
  }

  if (rfidReader.readCard(&myCard)) {

    uint16_t numTracksInFolder = mp3Player.getFolderTrackCount(myCard.folder);
      
      if (myCard.cookie == 322417479 && myCard.folder != 0 && myCard.mode != 0) {

        if (numTracksInFolder == 0) {
          Log.notice("No tracks found." CR);

        } else {

        // Log.notice("Mp3 player playing %T and current track number is %d" CR, isPlaying(), mp3Player.getCurrentTrack());

        // if (!isPlaying() || currentMp3PlayerTrack != mp3Player.getCurrentTrack()) {
          
          knownCard = true;
          _lastTrackFinished = 0;
          Log.notice("%d File(s) are in Folder %d" CR, numTracksInFolder, myCard.folder);

          // Hörspielmodus: eine zufällige Datei aus dem Ordner
          switch(myCard.mode) {
            case NFC_CARD_MODE::AUDIO_BOOK:
              Log.notice("Audio Book Mode is active. Random track will be played." CR);
              currentTrack = random(1, numTracksInFolder + 1);
              break;
          
          // Album Modus: kompletten Ordner spielen
          case NFC_CARD_MODE::ALBUM:
            Log.notice("Album Mode is active. All tracks in folder %d will be played." CR, myCard.folder);
            currentTrack = 1;
            break;
          
          // Party Modus: Ordner in zufälliger Reihenfolge
          case NFC_CARD_MODE::PARTY:
            Log.notice("Party Mode is active. Tracks from folder %d will be played randomly." CR, myCard.folder);
            currentTrack = random(1, numTracksInFolder + 1);
            break;

          // Einzel Modus: eine Datei aus dem Ordner abspielen
          case NFC_CARD_MODE::SINGLE_TRACK:
            Log.notice("Single Track Mode is active. Track %d in folder %d will be played." CR, myCard.special, myCard.folder);
            currentTrack = myCard.special;
            break;

          // Hörbuch Modus: kompletten Ordner spielen und Fortschritt merken
          case NFC_CARD_MODE::AUDIO_BOOK_WITH_BOOKMARK:
            Log.notice("Audio Book Mode with bookmarks is active. All tracks in the folder %d will be played one after the other and all completed tracks will be bookmarked." CR);
            currentTrack = EEPROM.read(myCard.folder);          
            break;
          }

          // Play track
          mp3Player.playFolderTrack(myCard.folder, currentTrack);

          delay(250);

          currentMp3PlayerTrack = mp3Player.getCurrentTrack();

          Log.notice("Track %d in folder %d (Mp3 Play number %d) is playing." CR, 
            currentTrack, myCard.folder, currentMp3PlayerTrack);
        
        // } else {
        //   Log.notice("Mp3Player is already playing the card. Tracker: %d" CR, currentMp3PlayerTrack);
        // }
        }
      }

      // Neue Karte konfigurieren
      else {
        knownCard = false;
        setupCard();
      }

    rfidReader.halt();
  }
}

void playPreview(int messageOffset, bool preview, int previewFromFolder, int returnValue){
  mp3Player.playMp3FolderTrack(messageOffset + returnValue);
  delay(1000);
  if (preview) {
    do {
      delay(10);
    } while (isPlaying());
    if (previewFromFolder == 0)
      mp3Player.playFolderTrack(returnValue, 1);
    else
      mp3Player.playFolderTrack(previewFromFolder, returnValue);
  }
}


int voiceMenu(int numberOfOptions, int startMessage, int messageOffset,
              bool preview = false, int previewFromFolder = 0) {
  int returnValue = 0;
  if (startMessage != 0) mp3Player.playMp3FolderTrack(startMessage);

  do {
    pauseButton.read();
    upButton.read();
    downButton.read();
    mp3Player.loop();

    restartOption();

    if (pauseButton.wasPressed()) {
      if (returnValue != 0) return returnValue;
      delay(1000);
    }

    if (upButton.pressedFor(LONG_PRESS)) {
      returnValue = min(returnValue + 10, numberOfOptions);
      playPreview(messageOffset, preview, previewFromFolder, returnValue);
      ignoreUpButton = true;

    } else if (upButton.wasReleased()) {
      if (!ignoreUpButton) {
        returnValue = min(returnValue + 1, numberOfOptions);
        playPreview(messageOffset, preview, previewFromFolder, returnValue);
      } else
        ignoreUpButton = false;
    }
    
    if (downButton.pressedFor(LONG_PRESS)) {
      returnValue = max(returnValue - 10, 1);
      playPreview(messageOffset, preview, previewFromFolder, returnValue);
      ignoreDownButton = true;

    } else if (downButton.wasReleased()) {
      if (!ignoreDownButton) {
        returnValue = max(returnValue - 1, 1);
        playPreview(messageOffset, preview, previewFromFolder, returnValue);
      } else
        ignoreDownButton = false;
    }
  } while (true);
}

void resetCard() {
  do {
    pauseButton.read();
    upButton.read();
    downButton.read();

    if (upButton.wasReleased() || downButton.wasReleased()) {
      Log.notice("Reset Card was cancelled.");
      mp3Player.playMp3FolderTrack(Mp3CommandMap::COMMAND_ID_RESET_ABORTED);
      return;
    }
  } while (!rfidReader.isNewCardPresent());

  if (!rfidReader.readCardSerial()) return;

  Log.notice("NFC Card is configured successfully!" CR);
  setupCard();
}

void setupCard() {
  mp3Player.pause();
  Log.notice("Configure new card:" CR);

  // Ask for mapping folder
  myCard.folder = voiceMenu(99, Mp3CommandMap::COMMAND_ID_NEW_TAG, 0, true);

  // Ask for play mode
  myCard.mode = voiceMenu(6, Mp3CommandMap::COMMAND_ID_TAG_LINKED, Mp3CommandMap::COMMAND_ID_TAG_LINKED);

  // Create bookmark 
  EEPROM.write(myCard.folder,1);

  // Ask for single track in case of single track mode
  if (myCard.mode == NFC_CARD_MODE::SINGLE_TRACK) {
    myCard.special = voiceMenu(mp3Player.getFolderTrackCount(myCard.folder), 
                                Mp3CommandMap::COMMAND_ID_SELECT_FILE, 0,
                                true, myCard.folder);
  }

  // Admin Mode
  if (myCard.mode == NFC_CARD_MODE::ADMIN) {
    myCard.special = voiceMenu(3, Mp3CommandMap::COMMAND_ID_SELECT_FILE, Mp3CommandMap::COMMAND_ID_SELECT_FILE);
  }

  // Configuration completed
  mp3Player.pause();
  rfidReader.writeCard(myCard);
}
