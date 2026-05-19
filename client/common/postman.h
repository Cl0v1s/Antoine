#ifndef POSTMAN_H
#define POSTMAN_H

#include <stdint.h>

#include "letter.h"
#include "letterFactory.h"
#include "player.h"
#include "config.h"

typedef struct
{
    uint32_t POST_BOX;
    uint8_t POST_BOX_LENGTH;

    uint32_t BOTTLE;
} LetterMemory;

LetterMemory LETTER_MEMORY_EUR_USA = {
    .POST_BOX = 0x1463C,
    .POST_BOX_LENGTH = 10,

    .BOTTLE = 0x10c3c,
};

static inline LetterStruct *selectRegion(LetterMemory *region)
{
    if (region == &LETTER_MEMORY_EUR_USA)
    {
        return &LETTER_EUR_USA;
    }
    consolef("We only support EU/USA for now.\n");
    dsExit(1);
    return 0;
}

static inline int loadLetters(char *save, int addr, int size, Letter *letters, LetterStruct *region)
{
    int i = 0;
    bool done = false;
    while (done == false && i < size)
    {
        int offset = addr + i * region->LETTER_SIZE;
        Letter current(save, offset, region);
        if (current.Exists() == false)
        {
            done = true;
        }
        else
        {
            memcpy(&letters[i], &current, sizeof(Letter));
            i += 1;
        }
    }
    return i;
}

static inline int gatherLetter(char *save, Letter *letters, LetterMemory *region)
{
    return loadLetters(save, region->POST_BOX, region->POST_BOX_LENGTH, letters, selectRegion(region));
}

// Please be sure to have room in your mailbox
static inline int deliverLetters(Config *config, char *save, Letter *letters, int length, LetterMemory *region)
{
    LetterFactory factory(config);
    bool done = false;
    int i = 0;
    LetterStruct *letterRegion = selectRegion(region);
    int delivered = 0;
    while (done == false && i < length)
    {
        int senderId = letters[i].GetSenderPlayerId();
        Player *player = Player::GetPlayerWithId(senderId);
        if (player == NULL)
        {
            std::string playername = letters[i].GetSenderPlayerName();
            consolef("No %s player found.", playername.c_str());
        }
        else
        {
            uint32_t offset = player->GetFreeMailboxSlot();
            // consolef("storing in %06x\n", offset);
            if (offset == 0x00)
            {
                std::string name = player->GetName();
                consolef("Not enough room in mailbox of %s.\n", name.c_str());
                done = true;
            }
            else
            {
                Letter ans = factory.Answer(letters[i], save, offset, letterRegion);
                if (ans.Exists())
                {
                    delivered += 1;
                }
            }
        }
        i += 1;
    }
    return delivered;
}

static inline bool gatherBottle(char *save, Letter *bottle, LetterMemory *region)
{
    int count = loadLetters(save, region->BOTTLE, 1, bottle, selectRegion(region));
    return count > 0;
}

static inline void sendBottle(Config *config, char *save, Letter *bottle, LetterMemory *region)
{
    if (bottle == NULL)
        return;
    std::string senderName = jsonEscape(bottle->GetSenderPlayerName());
    std::string senderTownName = jsonEscape(bottle->GetSenderTownName());
    std::string body = jsonEscape(bottle->GetBodyPart());
    std::string end = jsonEscape(bottle->GetEndPart());
    char *raw = (char *)malloc(1000 * sizeof(char)); // big to be sure to be able to store all data
    sprintf(raw,
            "{\n"
            "  \"playerName\": \"%s\",\n"
            "  \"townName\": \"%s\",\n"
            "  \"body\": \"%s\",\n"
            "  \"end\": \"%s\"\n"
            "}",
            senderName.c_str(), senderTownName.c_str(), body.c_str(), end.c_str());
    std::string json(raw);
    free(raw);
    std::string request = buildRequest(config->server.c_str(), "/throw", json.c_str());
    std::string response = getNet()->call(request.c_str());
    if (response.length() == 0)
    {
        consolef("Unable to throw bottle...\n");
    }
}

static inline void receiveBottle(Config *config, char *save, LetterMemory *region)
{
    for (int i = 0; i < PLAYER_COUNT; i++)
    {
        consolef("Player %04x\n", PLAYERS[i].GetPlayerId());
        if (PLAYERS[i].Exists() == false)
            continue;
        std::string name = PLAYERS[i].GetName();
        std::string townName = PLAYERS[i].GetTownName();
        char *raw = (char *)malloc(1000 * sizeof(char)); // big to be sure to be able to store all data
        sprintf(raw,
                "{\n"
                "  \"playerName\": \"%s\",\n"
                "  \"townName\": \"%s\"\n"
                "}",
                name.c_str(), townName.c_str());
        std::string json(raw);
        free(raw);
        std::string request = buildRequest(config->server.c_str(), "/find", json.c_str());
        std::string response = getNet()->call(request.c_str());
        if(response.length() == 0) continue;
        std::string intro = jsonStringValue(response, "intro");
        std::string body = jsonStringValue(response, "body");
        std::string end = jsonStringValue(response, "end");
        uint32_t offset = PLAYERS[i].GetFreeMailboxSlot();
        if (offset == 0x00)
        {
            consolef("Not enough room in mailbox of %s.\n", name.c_str());
            continue;
        }
        LetterStruct *letterRegion = selectRegion(region);
        Letter bottle(save, offset, letterRegion);
        bottle.SetIntroPart(intro);
        bottle.SetBodyPart(body);
        bottle.SetEndPart(end);
        // back to the sender from the receiver
        bottle.SetReceiverPlayerId(PLAYERS[i].GetPlayerId());
        bottle.SetReceiverPlayerName(PLAYERS[i].GetName());
        bottle.SetReceiverTownId(PLAYERS[i].GetTownId());
        bottle.SetReceiverTownName(PLAYERS[i].GetTownName());
        bottle.SetFlags(FLAG_RECEIVED_BOTTLE);
    }
}

#endif