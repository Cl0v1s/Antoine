#ifndef VILLAGER_H
#define VILLAGER_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "villager_memory.h"


#define VILLAGER_COUNT 8

typedef struct {
    uint16_t VILLAGER_START;
    uint16_t VILLAGER_SIZE;

    uint16_t MEMORY;

    uint16_t PATTERN;
    uint16_t LETTER; // This may be usefull in the future, probably a letter from the player stored there to show it later
    uint16_t FURNITURE;
    uint16_t PERSONALITY;
    uint16_t VILLAGER_ID;
    uint16_t SHIRT;
    uint16_t WALLPAPER;
    uint16_t CARPET;
    uint16_t UMBRELLA;
} VillagerStruct;

VillagerStruct VILLAGER_EUR_USA = {
    .VILLAGER_START = 0x8A3C,
    .VILLAGER_SIZE = 0x700,

    .MEMORY = 0x00,

    .PATTERN = 0x340,
	.LETTER = 0x568,
	.FURNITURE = 0x6AC, // 10.
	.PERSONALITY = 0x6CA, 
	.VILLAGER_ID = 0x6CB, 
	.SHIRT = 0x6EC, 
	.WALLPAPER = 0x6EE, 
	.CARPET = 0x6EF, 
	.UMBRELLA = 0x6F4 
};

VillagerStruct VILLAGER_JPN = {
    .VILLAGER_START = 0x744C,
    .VILLAGER_SIZE = 0x5C0,
    .MEMORY = 0x00,
    .PATTERN = 0x280,
    .LETTER = 0x4A0,
	.FURNITURE = 0x578, // 10.
	.PERSONALITY = 0x594, 
	.VILLAGER_ID = 0x595, 
	.SHIRT = 0x5AE, 
	.WALLPAPER = 0x5B0, 
	.CARPET = 0x5B1, 
	.UMBRELLA = 0x544 
};

VillagerStruct VILLAGER_KOR = {
    .VILLAGER_START = 0x9284,
    .VILLAGER_SIZE = 0x7EC,
    .MEMORY = 0x00,
    .PATTERN = 0x400,
    .LETTER = 0x634,
	.FURNITURE = 0x78C, // 10.
	.PERSONALITY = 0x7AE, 
	.VILLAGER_ID = 0x7AF, 
	.SHIRT = 0x7D2, 
	.WALLPAPER = 0x7D4, 
	.CARPET = 0x7D5, 
	.UMBRELLA = 0x7DA 
};


class Villager;
extern Villager* VILLAGERS;

class Villager {
    private:
        char* saveData;
        int startOffset;
        VillagerStruct* regionalData;
        VillagerMemory* memories;
        

    public:
        static void LoadVillagers(char* save, VillagerStruct* region) {
            for(int i = 0; i < VILLAGER_COUNT; i++) {
                int index = region->VILLAGER_START + i * region->VILLAGER_SIZE;
                Villager current(save, index, region);
                memcpy(&VILLAGERS[i], &current, sizeof(Villager));
            }
        }

        static Villager* GetVillagerWithId(uint16_t villagerId) {
            Villager* id = NULL;
            int i = 0;
            do {
                if(VILLAGERS[i].GetVillagerId() == villagerId) {
                    id = &VILLAGERS[i];
                    i = VILLAGER_COUNT;
                }
                i++;
            } while (i < VILLAGER_COUNT);
            return id;
        }

        Villager(char* save, int offset, VillagerStruct* region) {
            saveData = save;
            startOffset = offset;
            regionalData = region;

            this->memories = (VillagerMemory*)malloc(VILLAGER_MEMORY_COUNT*sizeof(VillagerMemory));
            for (int i = 0; i < VILLAGER_MEMORY_COUNT; i++) {
                VillagerMemory current(this->saveData, this->startOffset + region->MEMORY + i * MEMORY_EUR_USA.MEMORY_SIZE, &MEMORY_EUR_USA);
                memcpy(&this->memories[i], &current, sizeof(VillagerMemory));
            }
        }

        VillagerMemory* GetMemory(uint16_t playerId) {
            VillagerMemory* memory = NULL;
            int i = 0;
            do {
                if(this->memories[i].GetPlayerId() == playerId) {
                    memory = &this->memories[i];
                    i = VILLAGER_MEMORY_COUNT;
                }
                i++;
            } while (i < VILLAGER_MEMORY_COUNT);
            return memory;
        }

        uint16_t GetPattern() {
            return (saveData[startOffset + regionalData->PATTERN + 1] << 8) + (saveData[startOffset + regionalData->PATTERN] & 0xFF);
        }
        void SetPattern(uint16_t value) {
            saveData[startOffset + regionalData->PATTERN + 1] = value >> 8;
            saveData[startOffset + regionalData->PATTERN] = value & 0xFF;
        }

        uint8_t GetPersonality() {
            return saveData[startOffset + regionalData->PERSONALITY];
        }
        void SetPersonality(uint8_t value) {
            saveData[startOffset + regionalData->PERSONALITY] = value;
        }

        uint16_t GetVillagerId() {
            return (saveData[startOffset + regionalData->VILLAGER_ID + 1] << 8) + (saveData[startOffset + regionalData->VILLAGER_ID] & 0xFF);
        }
        void SetVillagerId(uint16_t value) {
            saveData[startOffset + regionalData->VILLAGER_ID + 1] = value >> 8;
            saveData[startOffset + regionalData->VILLAGER_ID] = value & 0xFF;
        }

        // Each furniture slot is 2 bytes (item ID, little-endian). 10 slots total.
        uint16_t GetFurniture(int index) {
            int offset = startOffset + regionalData->FURNITURE + index * 2;
            return (saveData[offset + 1] << 8) + (saveData[offset] & 0xFF);
        }
        void SetFurniture(int index, uint16_t value) {
            int offset = startOffset + regionalData->FURNITURE + index * 2;
            saveData[offset + 1] = value >> 8;
            saveData[offset] = value & 0xFF;
        }

        uint16_t GetShirt() {
            return (saveData[startOffset + regionalData->SHIRT + 1] << 8) + (saveData[startOffset + regionalData->SHIRT] & 0xFF);
        }
        void SetShirt(uint16_t value) {
            saveData[startOffset + regionalData->SHIRT + 1] = value >> 8;
            saveData[startOffset + regionalData->SHIRT] = value & 0xFF;
        }

        uint8_t GetWallpaper() {
            return saveData[startOffset + regionalData->WALLPAPER];
        }
        void SetWallpaper(uint8_t value) {
            saveData[startOffset + regionalData->WALLPAPER] = value;
        }

        uint8_t GetCarpet() {
            return saveData[startOffset + regionalData->CARPET];
        }
        void SetCarpet(uint8_t value) {
            saveData[startOffset + regionalData->CARPET] = value;
        }

        uint16_t GetUmbrella() {
            return (saveData[startOffset + regionalData->UMBRELLA + 1] << 8) + (saveData[startOffset + regionalData->UMBRELLA] & 0xFF);
        }
        void SetUmbrella(uint16_t value) {
            saveData[startOffset + regionalData->UMBRELLA + 1] = value >> 8;
            saveData[startOffset + regionalData->UMBRELLA] = value & 0xFF;
        }

        void print() {
            consolef("Villager: id=0x%04X | \n");
            for(int i = 0; i < VILLAGER_MEMORY_COUNT; i++) {
                this->memories[i].print();
            }
            consolef("---\n");
        }
};

Villager* VILLAGERS = (Villager*)malloc(VILLAGER_COUNT * sizeof(Villager));


#endif 