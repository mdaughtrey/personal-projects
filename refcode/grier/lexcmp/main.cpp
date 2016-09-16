#include <iostream>

//extern "C"
//{
//#include <string.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/ip.h>
//#include <netinet/in.h>
//}
//#include <Parser.h>
#include <vector>

bool lexcomp(const char * ref, int refLength, std::string cmp)
{
    std::string strref(ref, ref + refLength);
    bool result(lexicographical_compare(strref.begin(), strref.end(), cmp.begin(), cmp.end()));
    std::cout << "Comparing " << strref << " to " << cmp << " result " << result << std::endl;
    return result;
}

int main(int argc, char * argv [])
{
    lexcomp("password", 8, "OOOOOOOO");
    lexcomp("OOOOOOOO", 8, "password");

    return 0;
}
