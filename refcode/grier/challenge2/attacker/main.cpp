#include <iostream>

extern "C"
{
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
}
#include <vector>
#include <strategy1.h>


int main(int argc, char * argv [])
{
    int sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    //int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 1)
    {
        std::cerr << "socket call failed " << errno << std::endl;
        return 1;
    }

    struct sockaddr_in servaddr = { 0 };
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(10000);

//    if (bind(sock, (struct sockaddr *)(&servaddr), sizeof(servaddr)) < 0)
//    {
//        int errNo(errno);
//        std::cout << "bind " << errNo << " " << strerror(errNo) << std::endl;
//    }

    Strategy1 strategy(sock, servaddr);
    strategy.attack();
}
