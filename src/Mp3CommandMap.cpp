#include <Arduino.h>
#include "Mp3CommandMap.h"

Mp3CommandMapEntry Mp3CommandMap::map[] = {
                {Mp3CommandMap::COMMAND_ID_NEW_TAG, "New Tag"},
                {Mp3CommandMap::COMMAND_ID_TAG_LINKED, "Tag linked"},
                {Mp3CommandMap::COMMAND_ID_MODE_RANDOM_EPISODE, "Mode: Random EPISODE"},
                {Mp3CommandMap::COMMAND_ID_MODE_ALBUM, "MODE: Album"},
                {Mp3CommandMap::COMMAND_ID_MODE_PARTY, "Mode: Party"},
                {Mp3CommandMap::COMMAND_ID_MODE_SINGLE_TRACK, "Mode: Single Track"},
                {Mp3CommandMap::COMMAND_ID_MODE_AUDIO_BOOK, "Mode: Audio Book"},
                {Mp3CommandMap::COMMAND_ID_ADMIN, "Admin"},
                {Mp3CommandMap::COMMAND_ID_SELECT_FILE, "Select File"},
                {Mp3CommandMap::COMMAND_ID_SAY_NUMBER_ASK, "Ask for telling numbers"},
                {Mp3CommandMap::COMMAND_ID_SAY_NUMBER_NO, "Say NO for telling numbers"},
                {Mp3CommandMap::COMMAND_ID_SAY_NUMBER_YES, "Say YES for telling numbers"},
                {Mp3CommandMap::COMMAND_ID_TAG_OK, "Tag OK"},
                {Mp3CommandMap::COMMAND_ID_ERROR, "Error Message"},
                {Mp3CommandMap::COMMAND_ID_START_SOUND, "Start Sound"},
                {Mp3CommandMap::COMMAND_ID_RESET_TAG, "Reset Tag"},
                {Mp3CommandMap::COMMAND_ID_RESET_TAG_OK, "Reset Tag OK"},
                {Mp3CommandMap::COMMAND_ID_RESET_ABORTED, "Reset Tag Aborted"},
                {Mp3CommandMap::COMMAND_ID_RESET_OK, "Reset OK"},
        };

char* Mp3CommandMap::getCommandByID(uint16_t id) {
    char result[3];

    for(int i = 0; i < sizeof(map) / sizeof(Mp3CommandMapEntry); i++) {
        if (id == Mp3CommandMap::map[i].commandId) {
            //Serial.print("-> ");
            //Serial.println(Mp3CommandMap::map[i].name);
            return Mp3CommandMap::map[i].name;
        }
    }
    
    Serial.println("--> "+String(id));
    String(id).toCharArray(result, 3);
    return *result;
}