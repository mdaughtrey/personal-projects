#include <iostream>
#include <vector>

extern "C"
{
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
}

int main(int argc, char * argv [])
{
    //auto sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    auto sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in servaddr = { 0 };
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(10000);


    bind(sock, (struct sockaddr *)(&servaddr), sizeof(servaddr));
    unsigned char buffer[256] = { 0 };
    int rxCount(recvfrom(sock, (void *)buffer, 256, 0, NULL, NULL));
    std::cout << "rxCount " << rxCount << std::endl;
    std::cout << "buffer " << buffer << std::endl;

#if 0
    while (1)
    {
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET (sock, &fdReads);
        struct timeval timeout { 10, 0 };

        auto result = select(sock + 1, &fdReads, NULL, NULL, &timeout);
        if (result < 0)
        {
            cerr << "select error" << endl;
            close(sock);
            return 1;
        }
        if (0 == result)
        {
            cout << "Timeout" << endl;
            continue;
        }

        if (FD_ISSET(sock, &fdReads))
        {
            char buffer[1512]; // derived from the MTU size

            auto received = recv(sock, buffer, 1512,0);
            cout << "received " << received << endl;
            if (-1 == received)
            {
                if (EWOULDBLOCK == errno || EAGAIN == errno)
                {
                    cout << "Reading complete" << endl;
                }
                continue;
            }
            vector<unsigned char> response;
            parser.handleData(buffer, received, response);
            sleep(1);
            send(sock, response.data(), response.size(), 0);
        }
    }
#endif // 0
}
