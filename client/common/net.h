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
        virtual std::string call(const char* request) = 0;
        virtual ~Net() {};
};

void initNet(const char* server, int port);

Net* getNet();

void closeNet();

static inline std::string buildRequest(const char* addr, const char* url, const char* json) {
    char* dest = (char*)malloc(1000*sizeof(char)); // big to be sure to handle all data
    sprintf(dest,
        "POST %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "Content-Length: %d\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "%s",
        url,
        addr, 
        strlen(json), 
        json
    );
    std::string result = std::string(dest);
    free(dest);
    return result;
}

static inline int emit(int soc, const char* addr, int port, const char* request) {
    consolef("Sending:\n");
    consolef(request);
    consolef("\n");

    int bytesSent;
    size_t totalSent = 0;
    size_t requestLength = strlen(request);
    while (totalSent < requestLength) {
        bytesSent = send(soc, request + totalSent, requestLength - totalSent, 0);
        if (bytesSent < 0) {
            return -1;
        }
        totalSent += bytesSent;
    }
    return 0;
}

static inline int receive(int soc, std::string &result) {
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

    result += body;

    int statusCode = std::stoi(headers.substr(statusStart, statusEnd - statusStart));
    if (statusCode != 200) {
        errno = statusCode;
        return -1;
    }

    return 0;
}



#endif 