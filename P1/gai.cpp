#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(int argc, char** argv)
{
    if(argc != 2){
        std::cerr << "Expected use gai www.myweb.domain\n";
    }

    addrinfo hints;
    addrinfo* result;
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;//IPv4 and IPv6
    
    int res = getaddrinfo(argv[1], NULL,&hints,&result);

    if(res!=0){
        std::cerr << gai_strerror(res) << "\n";
        return 1;
    }

    while(result!=nullptr){
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        getnameinfo(result->ai_addr,result->ai_addrlen,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICHOST|NI_NUMERICSERV);

        std::cout << host << "\t" << result->ai_family << "\t" << result->ai_socktype << "\n";
        result = result->ai_next;
    }
 
    freeaddrinfo(result);
    return 0;
}
