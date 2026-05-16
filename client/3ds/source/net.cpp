#include "./../../common/net.h"

#include <malloc.h>

#include <3ds.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

Net* net;

class Net3DS: public Net {
    private:
        u32* SOC_buffer;
        sockaddr_in remote;
        char* addr;
        int port;
    public:
        Net3DS(const char* _addr, int _port) {
            SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

            if(SOC_buffer == NULL) {
                consolef("Unable to start wifi (al).\n");
                dsExit(1);
            }
            int ret;
            if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
                consolef("Unable to start wifi (sock ox%08X).\n", ret);
                dsExit(1);
            }
            addr = (char*)malloc(sizeof(char) * strlen(_addr) + 1);
            memcpy(addr, _addr, sizeof(char) * strlen(_addr) + 1);
            port = _port;
            struct hostent * myhost = gethostbyname(_addr);
            if(myhost == NULL) {
                consolef("Unable to resolve %s.\n", _addr);
                dsExit(1);
            }
            remote.sin_family = AF_INET;
            remote.sin_port = htons(_port);
            remote.sin_addr.s_addr= *( (unsigned long *)(myhost->h_addr_list[0]) );
        }

        ~Net3DS() {
            free(addr);
            free(SOC_buffer);
            socExit();
        }

        std::string call(const char* language, const char* senderId, const char* receiverName, const char* townName, uint16_t attachementId, std::string &intro, std::string &body, std::string &end) {
            int soc = socket(AF_INET, SOCK_STREAM, 0);
            int result = connect(soc, (struct sockaddr *)&this->remote, sizeof(this->remote));
            if(result != 0) {
                consolef("%d: unable to connect to %s\n%s\n", errno, addr, strerror(errno) );
                return std::string("");
            }
            result = emit(soc, addr, port, language, senderId, receiverName, townName, attachementId, intro, body, end);
            if(result != 0) {
                consolef("%d: unable to send data\n%s\n", errno, strerror(errno) );
                return std::string("");
            }
            
            std::string anwser("");
            result = receive(soc, anwser);
            if(result != 0) {
                consolef("%d: unable to receive data\n%s\n", errno, anwser.c_str());
                return std::string("");
            }
            close(soc);
            return anwser;
        }
};

void initNet(const char* server, int port) {
    consolef("Starting networking...\n");
    net = new Net3DS(server, port);
}

Net* getNet() {
    return net;
}

void closeNet() {
    delete net;
}
