#ifndef PROCESS_H
#define PROCESS_H

#include "utils.h"
#include "files.h"
#include "config.h"
#include "net.h"
#include "letter.h"
#include "letterFactory.h"
#include "postman.h"
#include "bootstrap.h"
#include "villager.h"
#include "player.h"

#define SAVE_LENGTH 0x3FFFF

static inline void mainProcess(int key) {
    initConsole();
    initFiles();
    // We need to put that data on stack, it doesnt work on rom
    char* saveData = (char*)malloc(sizeof(char) * SAVE_LENGTH);
    Config config = loadConfig("./ac.txt");
    printConfig(config);
    initNet(config.server.c_str(), config.port);

    if(!readFile(config.save.c_str(), saveData, SAVE_LENGTH)) {
        consolef("Unable to load save file\n");
        dsExit(1);
    }

    std::string backup = config.save + ".bak";
    if(!writeFile(backup.c_str(), saveData, SAVE_LENGTH)) {
        consolef("Unable to write backup file\n");
        dsExit(1);
    }

    // Player::LoadPlayers(saveData, &PLAYER_EUR_USA);
    // for(int i = 0; i < PLAYER_COUNT; i++) {
    //     PLAYERS[i].print();
    // }

    Villager::LoadVillagers(saveData, &VILLAGER_EUR_USA);
    for(int i = 0; i < 1; i++) {
        VILLAGERS[i].print();
    }

    LetterMemory* region = &LETTER_MEMORY_EUR_USA;
    Letter* letters = (Letter*)malloc(region->POST_BOX_LENGTH * sizeof(Letter));
    int letterLength = gatherLetter(saveData, letters, region);
    consolef("Loaded %d letters\n", letterLength);
    // for(int i = 0; i < letterLength; i++) {
    //     print(letters[i]);
    // }
    int delivered = deliverLetters(saveData, letters, letterLength, region, config.lang.c_str());
    // int delivered = 0;
    consolef("%d letters got a reply !\n", delivered);
    checksum(saveData);
    if(!writeFile(config.save.c_str(), saveData, SAVE_LENGTH)) {
        consolef("Unable to write save file\n");
        dsExit(1);
    }
    consolef("Save edited !\n");
    free(saveData);
    if(config.launcher) {
        consolef("Press start to launch the game\n");
        waitForKey(key);
        dsStartWW(config);
        return;
    } 
    dsExit(0);
}


#endif 