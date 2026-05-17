#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include <string.h>
#include <string>

#include "translator.h"
#include "utils.h"

#define PLAYER_COUNT 4
#define POCKET_SIZE  15

typedef struct {
    uint16_t PLAYER_START;
    uint16_t PLAYER_SIZE;

    uint16_t FACE_HAIRSTYLE; // uint8_t
    uint16_t TAN_HAIRCOLOR;  // uint8_t
    uint16_t GENDER;         // uint8_t
    uint16_t PLAYER_ID;      // uint16_t little-endian
    uint16_t TOWN_ID;        // uint16_t little-endian
    uint16_t TOWN_NAME;
    uint8_t  TOWN_SIZE;
    uint16_t NAME;
    uint8_t  NAME_SIZE;
    uint16_t WALLET_AMOUNT;  // uint32_t little-endian
    uint16_t BANK_AMOUNT;    // uint32_t little-endian
    uint16_t POCKET;         // POCKET_SIZE × uint16_t little-endian
    uint32_t DRESSER;        // absolute address (not relative to player start)
    uint16_t PATTERN;        // TODO: pattern class
    uint16_t PATTERN_SIZE;
    uint16_t LETTER;         // LETTER_SIZE bytes
    uint16_t LETTER_SIZE;
    uint16_t ACORNS;         // uint8_t
    int16_t  BED;            // uint8_t, -1 = not available
} PlayerStruct;

PlayerStruct PLAYER_EUR_USA = {
    .PLAYER_START   = 0xC,
    .PLAYER_SIZE    = 0x228C,
    .FACE_HAIRSTYLE = 0x223C,
    .TAN_HAIRCOLOR  = 0x223D,
    .GENDER         = 0x228A,
    .PLAYER_ID      = 0x2280,
    .TOWN_ID        = 0x2276,
    .TOWN_NAME      = 0x2278,
    .TOWN_SIZE      = 8,
    .NAME           = 0x2282,
    .NAME_SIZE      = 8,
    .WALLET_AMOUNT  = 0x1B40,
    .BANK_AMOUNT    = 0x21E4,
    .POCKET         = 0x1B22,
    .DRESSER        = 0x15430,
    .PATTERN        = 0,
    .PATTERN_SIZE   = 0x228,
    .LETTER         = 0x1148,
    .LETTER_SIZE    = 0xF4,
    .ACORNS         = 0x2225,
    .BED            = -1,
};

PlayerStruct PLAYER_JPN = {
    .PLAYER_START   = 0xC,
    .PLAYER_SIZE    = 0x1D10,
    .FACE_HAIRSTYLE = 0x1CC6,
    .TAN_HAIRCOLOR  = 0x1CC7,
    .GENDER         = 0x1CFB,
    .PLAYER_ID      = 0x1D04,
    .TOWN_ID        = 0x1CFC,
    .TOWN_NAME      = 0x1CFE,
    .TOWN_SIZE      = 6,
    .NAME           = 0x1D06,
    .NAME_SIZE      = 6,
    .WALLET_AMOUNT  = 0x16C4,
    .BANK_AMOUNT    = 0x1C70,
    .POCKET         = 0x16A6,
    .DRESSER        = 0x11764,
    .PATTERN        = 0,
    .PATTERN_SIZE   = 0x220,
    .LETTER         = 0x1108,
    .LETTER_SIZE    = 0x8C,
    .ACORNS         = 0x1CB1,
    .BED            = 0x1C9E,
};

PlayerStruct PLAYER_KOR = {
    .PLAYER_START   = 0x14,
    .PLAYER_SIZE    = 0x249C,
    .FACE_HAIRSTYLE = 0x243C,
    .TAN_HAIRCOLOR  = 0x243D,
    .GENDER         = 0x249A,
    .PLAYER_ID      = 0x248C,
    .TOWN_ID        = 0x247E,
    .TOWN_NAME      = 0x2480,
    .TOWN_SIZE      = 6,
    .NAME           = 0x248E,
    .NAME_SIZE      = 6,
    .WALLET_AMOUNT  = 0x1C10,
    .BANK_AMOUNT    = 0x23E0,
    .POCKET         = 0x1BF2,
    .DRESSER        = 0x16800,
    .PATTERN        = 0,
    .PATTERN_SIZE   = 0x234,
    .LETTER         = 0x11A8,
    .LETTER_SIZE    = 0x100,
    .ACORNS         = 0x2421,
    .BED            = -1,
};


class Player;
extern Player* PLAYERS;

class Player {
    private:
        char* saveData;
        int startOffset;
        PlayerStruct* regionalData;

    public:
        static void LoadPlayers(char* save, PlayerStruct* region) {
            for (int i = 0; i < PLAYER_COUNT; i++) {
                int index = region->PLAYER_START + i * region->PLAYER_SIZE;
                Player current(save, index, region);
                memcpy(&PLAYERS[i], &current, sizeof(Player));
            }
        }

        static Player* GetPlayerWithId(uint16_t playerId) {
            Player* id = NULL;
            int i = 0;
            do {
                if (PLAYERS[i].GetPlayerId() == playerId) {
                    id = &PLAYERS[i];
                    i = PLAYER_COUNT;
                }
                i++;
            } while (i < PLAYER_COUNT);
            return id;
        }

        Player(char* save, int offset, PlayerStruct* region) : saveData(save), startOffset(offset), regionalData(region) {}

        uint8_t GetFaceHairstyle() {
            return saveData[startOffset + regionalData->FACE_HAIRSTYLE];
        }
        void SetFaceHairstyle(uint8_t value) {
            saveData[startOffset + regionalData->FACE_HAIRSTYLE] = value;
        }

        uint8_t GetTanHaircolor() {
            return saveData[startOffset + regionalData->TAN_HAIRCOLOR];
        }
        void SetTanHaircolor(uint8_t value) {
            saveData[startOffset + regionalData->TAN_HAIRCOLOR] = value;
        }

        uint8_t GetGender() {
            return saveData[startOffset + regionalData->GENDER];
        }
        void SetGender(uint8_t value) {
            saveData[startOffset + regionalData->GENDER] = value;
        }

        uint16_t GetPlayerId() {
            return (saveData[startOffset + regionalData->PLAYER_ID + 1] << 8) + (saveData[startOffset + regionalData->PLAYER_ID] & 0xFF);
        }
        void SetPlayerId(uint16_t value) {
            saveData[startOffset + regionalData->PLAYER_ID + 1] = value >> 8;
            saveData[startOffset + regionalData->PLAYER_ID]     = value & 0xFF;
        }

        uint16_t GetTownId() {
            return (saveData[startOffset + regionalData->TOWN_ID + 1] << 8) + (saveData[startOffset + regionalData->TOWN_ID] & 0xFF);
        }
        void SetTownId(uint16_t value) {
            saveData[startOffset + regionalData->TOWN_ID + 1] = value >> 8;
            saveData[startOffset + regionalData->TOWN_ID]     = value & 0xFF;
        }

        std::string GetTownName() {
            return decode(std::string(saveData + startOffset + regionalData->TOWN_NAME, regionalData->TOWN_SIZE), regionalData == &PLAYER_JPN, regionalData == &PLAYER_KOR);
        }
        void SetTownName(const std::string& value) {
            std::string encoded = encode(value, regionalData == &PLAYER_JPN, regionalData == &PLAYER_KOR);
            strncpy(saveData + startOffset + regionalData->TOWN_NAME, encoded.c_str(), regionalData->TOWN_SIZE);
        }

        std::string GetName() {
            return decode(std::string(saveData + startOffset + regionalData->NAME, regionalData->NAME_SIZE), regionalData == &PLAYER_JPN, regionalData == &PLAYER_KOR);
        }
        void SetName(const std::string& value) {
            std::string encoded = encode(value, regionalData == &PLAYER_JPN, regionalData == &PLAYER_KOR);
            strncpy(saveData + startOffset + regionalData->NAME, encoded.c_str(), regionalData->NAME_SIZE);
        }

        uint32_t GetWalletAmount() {
            int o = startOffset + regionalData->WALLET_AMOUNT;
            return (saveData[o] & 0xFF) | ((saveData[o+1] & 0xFF) << 8) | ((saveData[o+2] & 0xFF) << 16) | ((saveData[o+3] & 0xFF) << 24);
        }
        void SetWalletAmount(uint32_t value) {
            int o = startOffset + regionalData->WALLET_AMOUNT;
            saveData[o]   = value & 0xFF;
            saveData[o+1] = (value >> 8)  & 0xFF;
            saveData[o+2] = (value >> 16) & 0xFF;
            saveData[o+3] = (value >> 24) & 0xFF;
        }

        uint32_t GetBankAmount() {
            int o = startOffset + regionalData->BANK_AMOUNT;
            return (saveData[o] & 0xFF) | ((saveData[o+1] & 0xFF) << 8) | ((saveData[o+2] & 0xFF) << 16) | ((saveData[o+3] & 0xFF) << 24);
        }
        void SetBankAmount(uint32_t value) {
            int o = startOffset + regionalData->BANK_AMOUNT;
            saveData[o]   = value & 0xFF;
            saveData[o+1] = (value >> 8)  & 0xFF;
            saveData[o+2] = (value >> 16) & 0xFF;
            saveData[o+3] = (value >> 24) & 0xFF;
        }

        // POCKET_SIZE slots of uint16_t little-endian
        uint16_t GetPocket(int index) {
            int o = startOffset + regionalData->POCKET + index * 2;
            return (saveData[o+1] << 8) + (saveData[o] & 0xFF);
        }
        void SetPocket(int index, uint16_t value) {
            int o = startOffset + regionalData->POCKET + index * 2;
            saveData[o+1] = value >> 8;
            saveData[o]   = value & 0xFF;
        }

        // Dresser: absolute address, not relative to player start
        uint16_t GetDresser(int index) {
            int o = regionalData->DRESSER + index * 2;
            return (saveData[o+1] << 8) + (saveData[o] & 0xFF);
        }
        void SetDresser(int index, uint16_t value) {
            int o = regionalData->DRESSER + index * 2;
            saveData[o+1] = value >> 8;
            saveData[o]   = value & 0xFF;
        }

        uint8_t GetAcorns() {
            return saveData[startOffset + regionalData->ACORNS];
        }
        void SetAcorns(uint8_t value) {
            saveData[startOffset + regionalData->ACORNS] = value;
        }

        // Returns -1 if not available for this region
        int GetBed() {
            if (regionalData->BED < 0) return -1;
            return saveData[startOffset + regionalData->BED] & 0xFF;
        }
        void SetBed(uint8_t value) {
            if (regionalData->BED < 0) return;
            saveData[startOffset + regionalData->BED] = value;
        }

        void print() {
            if(this->GetPlayerId() == 0) return;
            consolef("Player: %s (town: %s) | id=0x%04X | wallet: %lu | bank: %lu\n",
                GetName().c_str(),
                GetTownName().c_str(),
                GetPlayerId(),
                (unsigned long)GetWalletAmount(),
                (unsigned long)GetBankAmount());
        }
};

Player* PLAYERS = (Player*)malloc(PLAYER_COUNT * sizeof(Player));

#endif
