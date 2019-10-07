#ifndef Mp3CommandMap_H
#define Mp3CommandMap_H

#include <Arduino.h>

struct Mp3CommandMapEntry{
    uint16_t commandId;
    char* name;
};

class Mp3CommandMap {

    public:
        static const uint16_t COMMAND_ID_NEW_TAG = 300;
        static const uint16_t COMMAND_ID_TAG_LINKED = 310;
        static const uint16_t COMMAND_ID_MODE_RANDOM_EPISODE = 311;
        static const uint16_t COMMAND_ID_MODE_ALBUM = 312;
        static const uint16_t COMMAND_ID_MODE_PARTY = 313;
        static const uint16_t COMMAND_ID_MODE_SINGLE_TRACK = 314;
        static const uint16_t COMMAND_ID_MODE_AUDIO_BOOK = 315;
        static const uint16_t COMMAND_ID_ADMIN = 316;
        static const uint16_t COMMAND_ID_SELECT_FILE = 320;
        static const uint16_t COMMAND_ID_SAY_NUMBER_ASK = 330;
        static const uint16_t COMMAND_ID_SAY_NUMBER_NO = 331;
        static const uint16_t COMMAND_ID_SAY_NUMBER_YES = 332;
        static const uint16_t COMMAND_ID_TAG_OK = 400;
        static const uint16_t COMMAND_ID_ERROR = 401;
        static const uint16_t COMMAND_ID_START_SOUND = 500;
        static const uint16_t COMMAND_ID_RESET_TAG = 800;
        static const uint16_t COMMAND_ID_RESET_TAG_OK = 801;
        static const uint16_t COMMAND_ID_RESET_ABORTED = 802;
        static const uint16_t COMMAND_ID_RESET_OK = 999;

        static char* getCommandByID(uint16_t id);

    private:
        static Mp3CommandMapEntry map[20];
};

#endif