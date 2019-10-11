#include "SystemSound.h"

SystemSound::Mp3SystemSoundMap SystemSound::MP3_SYSTEM_SOUND_MAP[20] = {
        {SystemSound::UNKNOWN, "Unknown Track"},
        {SystemSound::NEW_TAG, "New Tag"},
        {SystemSound::TAG_LINKED, "Tag linked"},
        {SystemSound::MODE_RANDOM_EPISODE, "Mode: Random EPISODE"},
        {SystemSound::MODE_ALBUM, "MODE: Album"},
        {SystemSound::MODE_PARTY, "Mode: Party"},
        {SystemSound::MODE_SINGLE_TRACK, "Mode: Single Track"},
        {SystemSound::MODE_AUDIO_BOOK, "Mode: Audio Book"},
        {SystemSound::ADMIN, "Admin"},
        {SystemSound::SELECT_FILE, "Select File"},
        {SystemSound::SAY_NUMBER_ASK, "Ask for telling numbers"},
        {SystemSound::SAY_NUMBER_NO, "Say NO for telling numbers"},
        {SystemSound::SAY_NUMBER_YES, "Say YES for telling numbers"},
        {SystemSound::TAG_OK, "Tag OK"},
        {SystemSound::ERROR, "Error Message"},
        {SystemSound::START_SOUND, "Start Sound"},
        {SystemSound::RESET_TAG, "Reset Tag"},
        {SystemSound::RESET_TAG_OK, "Reset Tag OK"},
        {SystemSound::RESET_ABORTED, "Reset Tag Aborted"},
        {SystemSound::RESET_OK, "Reset OK"},
    };