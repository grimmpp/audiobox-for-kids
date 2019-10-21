#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <EEPROM.h>

// local references
#include "ICallback.h"
#include "Mp3Player.h"
#include "NfcData.h"

class Controller : public ICallback {
public:
    Controller() {}
    void initMp3Player(Mp3Player *player) { mp3Player = player; }

    void notify(uint8_t track) {
        // Log.notice(F("Start next track in callback." CR));
        nextTrack(track, true);
    }

    void nextTrack(uint16_t track, bool continuousMode = false) {
        if (mp3Player->isCurrentTrackSystemSound()) {
            Log.notice(F("Last track was a system sound." CR));
            return;
        }

        // This is needed because otherwise it would call this function at least two times. 
        // (loop in mp3Player lib doesn't clean up the state fast enough)
        if (continuousMode && mp3Player->isNextTrackHanded()) {
            Log.notice(F("Exit next track because it was already handled."));
            return;
        }
        mp3Player->nextTrackHandlingStarted();

        Log.notice(F("Choose Next Track: finished track: %d (Mp3PlayerTrack: %d, currentTrack: %d, Is system Sound: %b), playMode: %d" CR), 
            track, mp3Player->getCurrentMp3PlayerTrackId(), mp3Player->getCurrentTrack(), mp3Player->isCurrentTrackSystemSound(), mode);

        switch(mode) {
            case NFC_CARD_MODE::UNASSIGNED:
            Log.notice(F("No play mode assigned." CR));
            break;

            case NFC_CARD_MODE::AUDIO_BOOK:
            Log.notice(F("Audio Book: Do nothing." CR));
            mp3Player->sleep();
            break;
        
            case NFC_CARD_MODE::ALBUM:
            Log.notice(F("Album mode: play next track" CR));
            mp3Player->playNextSongInFolder();
            break;
        
            case NFC_CARD_MODE::PARTY: {
            uint16_t randomTrack = random(1, mp3Player->getFolderTrackCount(mp3Player->getCurrentFolder()));
            Log.notice(F("Party mode: Play random track %d" CR), randomTrack);
            mp3Player->playFolderTrack(mp3Player->getCurrentFolder(), randomTrack);
            break; }

            case NFC_CARD_MODE::SINGLE_TRACK:
            Log.notice(F("Single Track: do nothing." CR));
            mp3Player->sleep();
            break;

            case NFC_CARD_MODE::AUDIO_BOOK_WITH_BOOKMARK:
            Log.notice(F("Audio Book with Bookmark: play next track or start from beginning and save current track in EEPROM." CR));
            if (!mp3Player->playNextSongInFolder()) {
                mp3Player->playFolderTrack(mp3Player->getCurrentFolder(), 1);
            }
            EEPROM.write(mp3Player->getCurrentFolder(), mp3Player->getCurrentTrack());
            break;

            case NFC_CARD_MODE::ADMIN:
            Log.notice(F("Admin mode ... not defined." CR));
            break;
        }
    }

    void previousTrack() {
        Log.notice(F("Play previous track." CR));

        switch(mode) {
            case NFC_CARD_MODE::UNASSIGNED:
            Log.notice(F("No play mode assigned." CR));
            mp3Player->sleep();
            break;

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
            Log.notice(F("Single Track: Do nothing." CR));
            // Playing the same track can be done by pressing start/stop button
            mp3Player->sleep();
            break;

            case NFC_CARD_MODE::AUDIO_BOOK_WITH_BOOKMARK:
            Log.notice(F("Audio Book with Bookmark: Play previous track." CR));
            mp3Player->playPreviousSongInFolder();
            // Save bookmark into EEPROM
            EEPROM.write(mp3Player->getCurrentFolder(), mp3Player->getCurrentTrack());
            break;

            case NFC_CARD_MODE::ADMIN:
            Log.notice(F("Admin mode ... not defined." CR));
            break;
        }
    }

    void playTrack(NfcTag nfcCard) {
        uint16_t nextTrack = 0;

        mode = (NFC_CARD_MODE::ID)nfcCard.mode;
        switch(mode) {
            case NFC_CARD_MODE::UNASSIGNED:
            Log.error(F("No play mode assigned." CR));
            break;

            case NFC_CARD_MODE::AUDIO_BOOK:
            Log.notice(F("Audio Book Mode: Random track will be played." CR));
            nextTrack = random(1, mp3Player->getCurrentFolder() + 1);
            break;
        
            case NFC_CARD_MODE::ALBUM:
            Log.notice(F("Album Mode: All tracks in folder %d will be played." CR), nfcCard.folder);
            nextTrack = 1;
            break;
        
            case NFC_CARD_MODE::PARTY:
            Log.notice(F("Party Mode: Tracks from folder %d will be played randomly." CR), nfcCard.folder);
            nextTrack = random(1, mp3Player->getCurrentFolder() + 1);
            break;

            case NFC_CARD_MODE::SINGLE_TRACK:
            Log.notice(F("Single Track: Track %d in folder %d will be played." CR), nfcCard.special, nfcCard.folder);
            nextTrack = nfcCard.special;
            break;

            case NFC_CARD_MODE::AUDIO_BOOK_WITH_BOOKMARK:
            Log.notice(F("Audio Book Mode with bookmarks is active. All tracks in the folder %d will be played one after the other and all completed tracks will be bookmarked." CR));
            nextTrack = EEPROM.read(nfcCard.folder);
            if (mp3Player->isPlaying() && mp3Player->getCurrentFolder() == nfcCard.folder) {
                Log.notice(F("Is already playing - skip restarting."));
                return;
            }    
            break;

            case NFC_CARD_MODE::ADMIN:
            Log.notice(F("Admin mode ... not defined." CR));
            break;
        }

        // Play track
        if (!mp3Player->playFolderTrack(nfcCard.folder, nextTrack)) {
            // in case of misconfigured tag. If there is e.g. a track configured which doesn't exist on sd card anymore.
            mp3Player->playSystemSounds(SystemSound::ERROR);
        }
    }
    

private:
    Mp3Player *mp3Player;
    uint16_t lastTrack = 0;
    bool handled = false;

    NFC_CARD_MODE::ID mode = NFC_CARD_MODE::UNASSIGNED;
};

#endif