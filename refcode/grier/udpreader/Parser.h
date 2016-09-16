#pragma once

#include <string>
#include <vector>

class Parser
{
    typedef std::vector<unsigned char> PODLResponse;
    typedef PODLResponse::iterator PODLResponseIter;
public:
    Parser(const std::string & password);
    ~Parser();
    unsigned char handleData(char * data, int length, PODLResponse & response);
private:
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
    
    const std::string & m_password;
    void buildMessage(char * data, int length, PODLMessage * message);
    void buildResponse(PODLMessage * msg, PODLResponse & response, unsigned char code);
    bool checkMd5Sum(const PODLMessage & message);
    std::vector<unsigned char> buildMd5Sum(const PODLResponse response);
};
