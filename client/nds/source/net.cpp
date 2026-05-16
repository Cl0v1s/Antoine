#include "./../../common/net.h"
#include "./../../common/utils.h"

#include <nds.h>
#include <dswifi9.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

Net* net;

class NetNDS: public Net {
    private:
        sockaddr_in remote;
        char* addr;
        int port;
    public:
        NetNDS(const char* _addr, int _port) {
            if(!Wifi_InitDefault(WFC_CONNECT)) {
                consolef("Unable to start wifi.\n");
                consolef("Please check your internet switch and ensure that you configured\n");
                consolef("a connection (DS connection if running on a 3DS)\n");
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

        ~NetNDS() {
            free(addr);
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

            shutdown(soc, SHUT_RDWR);
            #ifndef ARM9
                close(soc);
            #else 
                closesocket(soc);
            #endif
            return anwser;
        }
};

void initNet(const char* server, int port) {
    consolef("Starting networking...\n");
    net = new NetNDS(server, port);
}

Net* getNet() {
    return net;
}

void closeNet() {
    delete net;
}