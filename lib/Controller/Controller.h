#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <EEPROM.h>

// local references
#include "Mp3Player.h"
#include "NfcData.h"

class Controller{
public:
    Controller(Mp3Player *player) : mp3Player(player) { }

    void nextTrack(uint16_t track, NFC_CARD_MODE::ID playMode) {

        if (mp3Player->isCurrentTrackSystemSound()) {
            Log.notice(F("Last track was a system sound." CR));
            return;
        }

        switch(playMode) {
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

    void previousTrack(NFC_CARD_MODE::ID playMode) {
        Log.notice(F("Play previous track." CR));

        switch(playMode) {
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

        switch(nfcCard.mode) {
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
};

#endif