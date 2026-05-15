#ifndef NET_H
#define NET_H

#include "./utils.h"
#include "./json.h"

#include <stdint.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#if defined(ARM9) 

#include <nds.h>

#elif defined(__3DS)

#include <3ds.h>

#endif

#define INVALID_RESPONSE 1
#define INVALID_STATUS 2
#define INVALID_BODY 3

class Net {
    public:
        virtual std::string call(const char* language, const char* senderId, const char* receiverName, const char* townName, uint16_t attachementId, int score, std::string &intro, std::string &body, std::string &end) = 0;
        virtual ~Net() {};
};

void initNet(const char* server, int port);

Net* getNet();

void closeNet();

static inline int buildBody(char* dest, const char* language, const char* senderId, const char* receiverName, const char* townName, uint16_t attachementId, int score, std::string &intro, std::string &body, std::string &end) {

    intro = jsonEscape(intro);
    body = jsonEscape(body);
    end = jsonEscape(end);

    return sprintf(dest, 
        "{\n"
        "  \"language\": \"%s\",\n"
        "  \"villagerId\": \"%s\",\n"
        "  \"playerName\": \"%s\",\n"
        "  \"townName\": \"%s\",\n"
        "  \"attachmentId\": %u,\n"
        "  \"score\": %d,\n"
        "  \"intro\": \"%s\",\n"
        "  \"body\": \"%s\",\n"
        "  \"end\": \"%s\"\n"
        "}", 
        language, senderId, receiverName, townName, attachementId, score, intro.c_str(), body.c_str(), end.c_str()
    );

}


static inline int buildRequest(char* dest, const char* addr, int port, const char* json) {
    return sprintf(dest,
        "POST /gen HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "Content-Length: %d\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "%s",
        addr, strlen(json), json
    );
}

static inline int emit(int soc, const char* addr, int port, const char* language, const char* senderId, const char* receiverName, const char* townName, uint16_t attachementId, int score, std::string &intro, std::string &body, std::string &end) {
    char* raw = (char*)malloc(sizeof(char) * 1000); // big size to be sure to store everything
    buildBody(raw, language, senderId, receiverName, townName, attachementId, score, intro, body, end);
    char* json = (char*)malloc(sizeof(char) * strlen(raw) + 1); // +1 to keep the \0
    memcpy(json, raw, sizeof(char) * strlen(raw) + 1);
    buildRequest(raw, addr, port, json);
    free(json);
    char* request = (char*)malloc(sizeof(char) * strlen(raw) + 1); // +1 to keep the \0
    memcpy(request, raw, sizeof(char) * strlen(raw) + 1);
    free(raw);

    // consolef("Sending:\n");
    // consolef(request);
    // consolef("\n");

    int bytesSent;

    size_t totalSent = 0;
    size_t requestLength = strlen(request);
    while (totalSent < requestLength) {
        bytesSent = send(soc, request + totalSent, requestLength - totalSent, 0);
        if (bytesSent < 0) {
            free(request);
            return -1;
        }
        totalSent += bytesSent;
    }

    // consolef("Sent %d bytes\n", totalSent);
    free(request);
    return 0;
}

static inline int receive(int soc, std::string &answer) {
    char buffer[1024];
    int bytesRead;
    std::string response;

    // Read the response from the server
    while ((bytesRead = recv(soc, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        response += buffer;
        // consolef("%s\n", response.c_str());
    }
    if (bytesRead < 0 && errno == EWOULDBLOCK) {
        consolef("Timeout reached while waiting for server response.\n");
        return -1;
    }

    if (bytesRead < 0) {
        return -1;
    }

    // Parse the HTTP response
    size_t headerEnd = response.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        errno = INVALID_RESPONSE;
        return -1;
    }

    std::string headers = response.substr(0, headerEnd);
    std::string body = response.substr(headerEnd + 4);

    // Check the HTTP status code
    size_t statusStart = headers.find(" ") + 1;
    size_t statusEnd = headers.find(" ", statusStart);
    if (statusStart == std::string::npos || statusEnd == std::string::npos) {
        errno = INVALID_STATUS;
        return -1;
    }

    answer += body;

    int statusCode = std::stoi(headers.substr(statusStart, statusEnd - statusStart));
    if (statusCode != 200) {
        errno = statusCode;
        return -1;
    }

    return 0;
}



#endif 