#include <iostream>

using namespace std;

extern "C"
{
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
        cerr << "No password specified" << endl;
        return 1;
    }
    std::string password(argv[1]);
    if (password.length() > 255)
    {
        cerr << "Password is too long" << endl;
        return 1;
    }

    cout << "Password is " << password.c_str() << endl;
    Parser parser(password);
    auto sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);

    if (sock < 1)
    {
        cerr << "socket call failed " << errno << endl;
        return 1;
    }

    struct sockaddr_in servaddr = { 0 };
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(10000);

    if (bind(sock, (struct sockaddr *)(&servaddr), sizeof(servaddr)) < 0)
    {
        cerr << "bind failed " << errno << endl;
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
            send(sock, response.data(), response.size(), 0);
        }
    }
}
