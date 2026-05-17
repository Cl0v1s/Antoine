#ifndef VILLAGER_MEMORY_H
#define VILLAGER_MEMORY_H

#include <stdint.h>
#include <string.h>
#include <string>

#include "translator.h"
#include "utils.h"

#define VILLAGER_MEMORY_COUNT 8

typedef struct {
    uint16_t MEMORY_SIZE;

    uint16_t TOWN_ID;
    uint16_t TOWN_NAME;
    uint8_t  TOWN_NAME_SIZE;
    uint16_t PLAYER_ID;
    uint16_t PLAYER_NAME;
    uint8_t  PLAYER_NAME_SIZE;
    uint16_t NICKNAME;
    uint8_t  NICKNAME_SIZE;
    uint16_t FRIENDSHIP;
} VillagerMemoryStruct;

VillagerMemoryStruct MEMORY_EUR_USA = {
    .MEMORY_SIZE       = 0x68,
    .TOWN_ID          = 0x00,
    .TOWN_NAME        = 0x02,
    .TOWN_NAME_SIZE   = 8,
    .PLAYER_ID        = 0x0A,
    .PLAYER_NAME      = 0x0C,
    .PLAYER_NAME_SIZE = 8,
    .NICKNAME         = 0x16,
    .NICKNAME_SIZE    = 8,
    .FRIENDSHIP       = 0x43,
};

VillagerMemoryStruct MEMORY_JPN = {
    .MEMORY_SIZE       = 0x0, // TODO
    .TOWN_ID          = 0x00,
    .TOWN_NAME        = 0x02,
    .TOWN_NAME_SIZE   = 6,
    .PLAYER_ID        = 0x08,
    .PLAYER_NAME      = 0x0A,
    .PLAYER_NAME_SIZE = 6,
    .NICKNAME         = 0x0, // TODO
    .NICKNAME_SIZE    = 6,
    .FRIENDSHIP       = 0x0, // TODO
};

VillagerMemoryStruct MEMORY_KOR = {
    .MEMORY_SIZE       = 0x0, // TODO
    .TOWN_ID          = 0x00,
    .TOWN_NAME        = 0x02,
    .TOWN_NAME_SIZE   = 6,
    .PLAYER_ID        = 0x08,
    .PLAYER_NAME      = 0x0A,
    .PLAYER_NAME_SIZE = 6,
    .NICKNAME         = 0x0, // TODO
    .NICKNAME_SIZE    = 6,
    .FRIENDSHIP       = 0x0, // TODO
};


class VillagerMemory {
    private:
        char* saveData;
        int startOffset;
        VillagerMemoryStruct* regionalData;

    public:
        VillagerMemory(char* save, int offset, VillagerMemoryStruct* region)
            : saveData(save), startOffset(offset), regionalData(region) {}

        uint16_t GetTownId() {
            return (saveData[startOffset + regionalData->TOWN_ID + 1] << 8) | (saveData[startOffset + regionalData->TOWN_ID] & 0xFF);
        }

        std::string GetTownName() {
            return decode(std::string(saveData + startOffset + regionalData->TOWN_NAME, regionalData->TOWN_NAME_SIZE),
                          regionalData == &MEMORY_JPN, regionalData == &MEMORY_KOR);
        }

        uint16_t GetPlayerId() {
            return (saveData[startOffset + regionalData->PLAYER_ID + 1] << 8) | (saveData[startOffset + regionalData->PLAYER_ID] & 0xFF);
        }

        std::string GetPlayerName() {
            return decode(std::string(saveData + startOffset + regionalData->PLAYER_NAME, regionalData->PLAYER_NAME_SIZE),
                          regionalData == &MEMORY_JPN, regionalData == &MEMORY_KOR);
        }

        std::string GetNickname() {
            return decode(std::string(saveData + startOffset + regionalData->NICKNAME, regionalData->NICKNAME_SIZE),
                          regionalData == &MEMORY_JPN, regionalData == &MEMORY_KOR);
        }
        void SetNickname(const std::string& value) {
            std::string encoded = encode(value, regionalData == &MEMORY_JPN, regionalData == &MEMORY_KOR);
            strncpy(saveData + startOffset + regionalData->NICKNAME, encoded.c_str(), regionalData->NICKNAME_SIZE);
        }

        uint8_t GetFriendship() {
            return (uint8_t)saveData[startOffset + regionalData->FRIENDSHIP];
        }
        void SetFriendship(uint8_t value) {
            saveData[startOffset + regionalData->FRIENDSHIP] = (char)value;
        }

        void print() {
            if(this->GetPlayerId() == 0) return;
            consolef("Memory: player=%s (id=0x%04X, town=%s) | nickname=%s | friendship=%u\n",
                GetPlayerName().c_str(),
                (unsigned)GetPlayerId(),
                GetTownName().c_str(),
                GetNickname().c_str(),
                (unsigned)GetFriendship());
        }
};


#endif
