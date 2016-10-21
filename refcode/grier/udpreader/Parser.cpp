#include "Parser.h"
#include <openssl/md5.h>
#include <memory>

#include <iostream>
extern "C"
{
#include <string.h>
}

using namespace std;

namespace
{
}

Parser::Parser(const std::string & password )
: m_password(password)
{
    cout << "Parser Contructor" << endl;
}

Parser::~Parser()
{
    cout << "Parser Destructor" << endl;
}

bool
Parser::checkMd5Sum(const PODLMessage & message)
{
    unsigned char result[MD5_DIGEST_LENGTH];
    std::vector<unsigned char> data((unsigned char *)&message,
        (unsigned char *)&message + sizeof(message.header) + message.header.length);
    MD5(data.data(), data.size(), result);
    if (0 == memcmp(result, message.md5, MD5_DIGEST_LENGTH))
    {
        return true;
    }
    return false;
}

void
Parser::buildMessage(char * data, int length, PODLMessage * message)
{
    memset(message, sizeof(message), 0);
    memcpy(&message->header, data, sizeof(message->header));
    memcpy(message->data, &data[sizeof(message->header)], message->header.length);
    memcpy(message->md5, &data[sizeof(message->header) + message->header.length], 16);
}

unsigned char
Parser::handleData(char * data, int length, PODLResponse & response)
{
    unsigned char code(255);
    cout << "handling " << length << " bytes of data" << endl;

    PODLMessage msg;
    while (1)
    {
        // Raw packet length check
        if (length > sizeof(PODLMessage))
        {
            cerr << "Raw packet length of " << length << " bytes is too long, max "
                << sizeof(PODLMessage) << endl;
            code = 2;
            break;
        }

        buildMessage(data, length, &msg);

        if (strncmp("PODL", msg.header.podl, 4))
        {
            cerr << "Not a PODL header" << endl;
            code = 2;
            break;
        }
     
        if (false == checkMd5Sum(msg))
        {
            cerr << "MD5 checksum fails" << endl;
            code = 2;
            break;
        }

        if (msg.header.length != m_password.length())
        {
            cerr << "Password length mismatch" << endl;
            code = 1;
            break;
        }

        if (strncmp(msg.data, m_password.data(), m_password.length()))
        {
            cerr << "Password mismatch" << endl;
            code = 1;
            break;
        }
        code = 0;
        break;
    }
    if (255 == code)
    {
        cerr << "Code not set. Bug!" << endl;
        exit(1);
    }
    cout << "code is " << (int)code << endl;
    buildResponse(&msg, response, code);
    
    return 0;
}

void
Parser::buildResponse(PODLMessage * msg,
     PODLResponse & response,
     unsigned char code)
{
    std::unique_ptr<unsigned char> rdata(
        new unsigned char[sizeof(msg->header) + msg->header.length + 1 + 15]);
    msg->header.length = 1;
    memcpy(rdata.get(), msg, sizeof(msg->header));
    int offset(sizeof(msg->header));
    rdata.get()[offset++] = code;
    std::vector<unsigned char> checksum;
    memcpy(&rdata.get()[offset], checksum.data(), checksum.size());
    offset += checksum.size();
}

std::vector<unsigned char>
Parser::buildMd5Sum(const PODLResponse response)
{
    std::vector<unsigned char> checksum;
    return checksum;
}

