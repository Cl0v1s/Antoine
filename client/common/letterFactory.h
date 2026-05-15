#ifndef LETTERFACTORY_H
#define LETTERFACTORY_H

#include <string>
#include <stdint.h>


#include "./letter.h"
#include "utils.h"
#include "./net.h"
#include "score.h"

typedef struct {
    std::string intro;
    std::string body;
    std::string end;
    uint16_t attachmentId;
    uint16_t paperId;
} Content;

class LetterFactory {
    private:
        /**
         * Get a random paper for the answer
         */
        unsigned char GetPaper() {
            return 0x37;
        }

        /**
         * Generate answer content from player's letter
         */
        int GenerateContent(Content &content, Letter &letter, const char* lang) {
            char villagerId[10];
            sprintf(villagerId, "%04x", letter.GetReceiverPlayerId() & 0xFF); // somehow it's actually on 8bits

            std::string intro = letter.GetIntroPart();
            std::string body = letter.GetBodyPart();
            std::string end = letter.GetEndPart();

            int score = calculateScore(std::string(lang), body);

            std::string reply = getNet()->call(lang, villagerId, letter.GetSenderPlayerName().c_str(), letter.GetReceiverTownName().c_str(), letter.GetAttachementId(), score, intro, body, end);
            if(reply.length() == 0) {
                return -1;
            }

            content.intro = jsonStringValue(reply, "intro");
            content.body = jsonStringValue(reply, "body");
            content.end = jsonStringValue(reply, "end");
            content.attachmentId = jsonUint16Value(reply, "attachmentId");
            content.paperId = jsonUint16Value(reply, "paperId");

            if (content.intro.empty() || content.body.empty() || content.end.empty()) {
                return -1;
            }

            return 0;
        }

    public:
        /**
         * Generate answer letter from player's letter
         */
        Letter Answer(Letter &letter, char* save, int offset, LetterStruct* region, const char* lang) {
            Letter answer(save, offset, region);
            Content content;
            if(this->GenerateContent(content, letter, lang) == -1) {
                consolef("Unable to generate reply\n");
                return answer;
            }
            answer.SetIntroPart(content.intro);
            answer.SetBodyPart(content.body);
            answer.SetEndPart(content.end);
            answer.setPaperId(content.paperId);

            // back to the sender from the receiver
            answer.SetReceiverPlayerId(letter.GetSenderPlayerId());
            answer.SetReceiverPlayerName(letter.GetSenderPlayerName());
            answer.SetReceiverTownId(letter.GetSenderTownId());
            answer.SetReceiverTownName(letter.GetSenderTownName());

            answer.SetSenderPlayerId(letter.GetReceiverPlayerId());
            answer.SetSenderPlayerName(letter.GetReceiverPlayerName());
            answer.SetSenderTownId(letter.GetReceiverTownId());
            answer.SetSenderTownName(letter.GetReceiverTownName());


            answer.SetAttachementId(content.attachmentId);
            answer.SetFlags(FLAG_UNREAD);
            // answer.SetIntroFlag(INSERT_NAME_INTRO);
            answer.SetNameFlag(INSERT_NAME_INVENTORY);

            return answer;
        }
};

#endif 