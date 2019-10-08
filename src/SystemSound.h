#ifndef Mp3CommandMap_H
#define Mp3CommandMap_H

#include <Arduino.h>


class SystemSound {
public:

    enum ID : uint16_t {
        UNKNOWN = 0,
        NEW_TAG = 300,
        TAG_LINKED = 310,
        MODE_RANDOM_EPISODE = 311,
        MODE_ALBUM = 312,
        MODE_PARTY = 313,
        MODE_SINGLE_TRACK = 314,
        MODE_AUDIO_BOOK = 315,
        ADMIN = 316,
        SELECT_FILE = 320,
        SAY_NUMBER_ASK = 330,
        SAY_NUMBER_NO = 331,
        SAY_NUMBER_YES = 332,
        TAG_OK = 400,
        ERROR = 401,
        START_SOUND = 500,
        RESET_TAG = 800,
        RESET_TAG_OK = 801,
        RESET_ABORTED = 802,
        RESET_OK = 999,
    };

    struct Mp3SystemSoundMap{
        ID systemSoundId;
        char const *name;
    };

    static Mp3SystemSoundMap MP3_SYSTEM_SOUND_MAP[20] = {
        {ID::UNKNOWN, "Unknown Track"},
        {ID::NEW_TAG, "New Tag"},
        {ID::TAG_LINKED, "Tag linked"},
        {ID::MODE_RANDOM_EPISODE, "Mode: Random EPISODE"},
        {ID::MODE_ALBUM, "MODE: Album"},
        {ID::MODE_PARTY, "Mode: Party"},
        {ID::MODE_SINGLE_TRACK, "Mode: Single Track"},
        {ID::MODE_AUDIO_BOOK, "Mode: Audio Book"},
        {ID::ADMIN, "Admin"},
        {ID::SELECT_FILE, "Select File"},
        {ID::SAY_NUMBER_ASK, "Ask for telling numbers"},
        {ID::SAY_NUMBER_NO, "Say NO for telling numbers"},
        {ID::SAY_NUMBER_YES, "Say YES for telling numbers"},
        {ID::TAG_OK, "Tag OK"},
        {ID::ERROR, "Error Message"},
        {ID::START_SOUND, "Start Sound"},
        {ID::RESET_TAG, "Reset Tag"},
        {ID::RESET_TAG_OK, "Reset Tag OK"},
        {ID::RESET_ABORTED, "Reset Tag Aborted"},
        {ID::RESET_OK, "Reset OK"},
    };

    static const char* GetSystemSoundById(uint16_t id) {

        for(uint16_t i = 0; i < sizeof(MP3_SYSTEM_SOUND_MAP) / sizeof(Mp3SystemSoundMap); i++) {
            if (id == MP3_SYSTEM_SOUND_MAP[i].systemSoundId) {
                return MP3_SYSTEM_SOUND_MAP[i].name;
            }
        }
        
        // Return unknown field.
        return MP3_SYSTEM_SOUND_MAP[0].name;
    }
};

#endif