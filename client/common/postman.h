#ifndef POSTMAN_H
#define POSTMAN_H

#include <stdint.h>

#include "letter.h"
#include "letterFactory.h"
#include "player.h"

typedef struct {
    uint32_t POST_BOX;
    uint8_t POST_BOX_LENGTH;

    uint32_t BOTTLE;
} LetterMemory;

LetterMemory LETTER_MEMORY_EUR_USA = {
    .POST_BOX = 0x1463C,
    .POST_BOX_LENGTH = 10,

    .BOTTLE = 0x10c3c,
};

LetterFactory factory;

static inline LetterStruct* selectRegion(LetterMemory* region) {
    if(region == &LETTER_MEMORY_EUR_USA) {
        return &LETTER_EUR_USA;
    }
    consolef("We only support EU/USA for now.\n");
    dsExit(1);
    return 0;
}

static inline int loadLetters(char* save, int addr, int size,  Letter* letters, LetterStruct* region) {
    int i = 0;
    bool done = false;
    while (done == false && i < size) {
        int offset = addr + i * region->LETTER_SIZE;
        Letter current(save, offset, region);
        if(current.Exists() == false) {
            done = true;
        } else {
            memcpy(&letters[i], &current, sizeof(Letter));
            i += 1;
        }
    }
    return i;
}

static inline int gatherLetter(char* save, Letter* letters, LetterMemory* region) {
    return loadLetters(save, region->POST_BOX, region->POST_BOX_LENGTH, letters, selectRegion(region));
}

// Please be sure to have room in your mailbox 
static inline int deliverLetters(char* save, Letter* letters, int length, LetterMemory* region, const char* lang) {
    bool done = false;
    int i = 0;
    LetterStruct* letterRegion = selectRegion(region);
    int delivered = 0;
    while(done == false && i < length) {
        int senderId = letters[i].GetSenderPlayerId();
        Player* player = Player::GetPlayerWithId(senderId);
        if(player == NULL) {
            std::string playername = letters[i].GetSenderPlayerName();
            consolef("No %s player found.", playername.c_str());
        } else {
            uint32_t offset = player->GetFreeMailboxSlot();
            consolef("storing in %06x\n", offset);
            if(offset == 0x00) {
                consolef("Not enough room in mailbox.\n");
                done = true;
            } else { 
                Letter ans = factory.Answer(letters[i], save, offset, letterRegion, lang);
                if(ans.Exists()) {
                    delivered += 1;
                }
            }
        }
        i += 1;
    }
    return delivered;
}

static inline bool gatherBottle(char* save, Letter* bottle, LetterMemory* region) {
    int count = loadLetters(save, region->BOTTLE, 1, bottle, selectRegion(region));
    return count > 0;
}

static inline void sendBottle(char* save, Letter* bottle, LetterMemory* region) {

}
 


#endif