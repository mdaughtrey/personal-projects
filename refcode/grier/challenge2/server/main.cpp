#include <iostream>


extern "C"
{
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
}
#include <Parser.h>
#include <vector>

int main(int argc, char * argv [])
{
    if (argc < 2)
    {
        std::cerr << "No password specified" << std::endl;
        return 1;
    }
    std::string password(argv[1]);
    if (password.length() > 255)
    {
        std::cerr << "Password is too long" << std::endl;
        return 1;
    }

    std::cout << "Password is " << password.c_str() << std::endl;
    Parser parser(password);
    auto sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);

    if (sock < 1)
    {
        std::cerr << "socket call failed " << errno << std::endl;
        return 1;
    }

    struct sockaddr_in servaddr = { 0 };
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(10000);

    if (bind(sock, (struct sockaddr *)(&servaddr), sizeof(servaddr)) < 0)
    {
        std::cerr << "bind failed " << errno << std::endl;
        return 1;
    }

    while (1)
    {
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET (sock, &fdReads);
        struct timeval timeout { 10, 0 };

        auto result = select(sock + 1, &fdReads, NULL, NULL, &timeout);
        if (result < 0)
        {
            std::cerr << "select error" << std::endl;
            close(sock);
            return 1;
        }
        if (0 == result)
        {
            std::cout << "Timeout" << std::endl;
            continue;
        }

        if (FD_ISSET(sock, &fdReads))
        {
            char buffer[1512]; // derived from the MTU size

            struct sockaddr_in clientaddr = { 0 };
            clientaddr.sin_family = AF_INET;
            clientaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            clientaddr.sin_port = htons(10000);
            socklen_t csize;

            int received = recvfrom(sock, buffer, 1512, 0, (struct sockaddr *)&clientaddr, &csize);
            std::cout << "received " << received << std::endl;
            if (-1 == received)
            {
                if (EWOULDBLOCK == errno || EAGAIN == errno)
                {
                    std::cout << "Reading complete" << std::endl;
                }
                continue;
            }
            std::vector<unsigned char> response;
            parser.handleData(buffer, received, response);
            std::cout << "Sending response, " << std::dec << (int)response.size() << " bytes...";
            int rc = sendto(sock, response.data(), response.size(), 0,
                 (struct sockaddr *)&clientaddr, sizeof(clientaddr));
            if (rc < 0)
            {
                int errNo(errno);
                std::cout << "send fail " << errNo << " " << strerror(errNo) << std::endl;
            }

            std::cout << "rc " << rc << std::endl;
            std::cout << "sent" << std::endl;
        }
    }
}
