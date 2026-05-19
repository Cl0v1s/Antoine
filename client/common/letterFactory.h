#ifndef LETTERFACTORY_H
#define LETTERFACTORY_H

#include <string>
#include <stdint.h>


#include "./letter.h"
#include "utils.h"
#include "./net.h"
#include "config.h"
typedef struct {
    std::string intro;
    std::string body;
    std::string end;
    uint16_t attachmentId;
    uint16_t paperId;
    int score;
} Content;

class LetterFactory {
    private:
        Config* config;

        static std::string buildBody(const char* language, const char* senderId, const char* receiverName, const char* townName, uint16_t attachementId, std::string &intro, std::string &body, std::string &end) {
            char* dest = (char*)malloc(1000 * sizeof(char)); // big to be sure to be able to store all data
            intro = jsonEscape(intro);
            body = jsonEscape(body);
            end = jsonEscape(end);
            sprintf(dest,
                "{\n"
                "  \"language\": \"%s\",\n"
                "  \"villagerId\": \"%s\",\n"
                "  \"playerName\": \"%s\",\n"
                "  \"townName\": \"%s\",\n"
                "  \"attachmentId\": %u,\n"
                "  \"intro\": \"%s\",\n"
                "  \"body\": \"%s\",\n"
                "  \"end\": \"%s\"\n"
                "}",
                language, senderId, receiverName, townName, attachementId, intro.c_str(), body.c_str(), end.c_str()
            );
            std::string result = std::string(dest);
            free(dest);
            return result;
        }

        /**
         * Generate answer content from player's letter
         */
        int GenerateContent(Content &content, Letter &letter, const char* lang) {
            char villagerId[10];
            sprintf(villagerId, "%04x", letter.GetReceiverPlayerId() >> 8); // somehow it's actually on 8bits

            std::string intro = letter.GetIntroPart();
            std::string body = letter.GetBodyPart();
            std::string end = letter.GetEndPart();

            std::string json = buildBody(this->config->lang.c_str(), villagerId, letter.GetSenderPlayerName().c_str(), letter.GetReceiverTownName().c_str(), letter.GetAttachementId(), intro, body, end);
            std::string request = buildRequest(this->config->server.c_str(), "/gen", json.c_str());

            std::string reply = getNet()->call(request.c_str());
            if(reply.length() == 0) {
                return -1;
            }

            content.intro = jsonStringValue(reply, "intro");
            content.body = jsonStringValue(reply, "body");
            content.end = jsonStringValue(reply, "end");
            content.attachmentId = jsonUint16Value(reply, "attachmentId");
            content.paperId = jsonUint16Value(reply, "paperId");
            content.score = jsonIntValue(reply, "score");

            if (content.intro.empty() || content.body.empty() || content.end.empty()) {
                return -1;
            }

            return 0;
        }

    public:
        LetterFactory(Config* config) {
            this->config = config;
        }
        /**
         * Generate answer letter from player's letter
         */
        Letter Answer(Letter &letter, char* save, int offset, LetterStruct* region) {
            Letter answer(save, offset, region);
            Content content;
            if(this->GenerateContent(content, letter, this->config->lang.c_str()) == -1) {
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
            answer.SetNameFlag(INSERT_NAME_INVENTORY);

            return answer;
        }
};

#endif 