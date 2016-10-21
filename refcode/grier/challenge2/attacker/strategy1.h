#pragma once

#include <vector>
#include <string>

extern "C"
{
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
}
#pragma pack(push, 1)
    typedef struct 
    {
        struct
        {
            char podl[4];
            unsigned int id;
            unsigned char length;
        } header;
        char data[255];
        char md5[16];
    } PODLMessage;
#pragma pack(pop)

typedef std::vector<unsigned char> Vec;
typedef Vec::iterator VecIter;

class Strategy1
{
public:
    Strategy1(int sock, struct sockaddr_in & servaddr);
    bool attack();

protected:
    int m_socket;
    struct sockaddr_in m_servaddr;

    void buildMessage(PODLMessage * msg, Vec & payload);
    void buildMd5Sum(PODLMessage * message);
    bool probe(PODLMessage * msg, Vec * result);
    void dump(std::string caption, Vec & dump);
    void hexDump(Vec & dump);
    int getPasswordLength();
    bool attack2();
};

