#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

// ./time-server <dir> <puerto>
int main(int argc, char** argv)
{

    addrinfo hints;
    addrinfo* result;
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_DGRAM;//UDP
    hints.ai_family = AF_INET;

    int rc = getaddrinfo(argv[1], NULL,&hints,&result);

    if(rc!=0){
        std::cerr << gai_strerror(rc) << "\n";
        return -1;
    }

    int sd = socket(result->ai_family,result->ai_socktype,0);

    if(sd == -1){
	    std::cerr << "[socket] failed to create, errno " << sd;
    }

    int rcb = bind(sd, result->ai_addr, result->ai_addrlen);

    if(rcb != 0){
        std::cerr << "[bind] failed\n";
        return -1;
    }

    freeaddrinfo(result);

    while(true){
        char buffer[80];
        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        struct sockaddr client;
        socklen_t clientLen = sizeof(struct sockaddr); 

        int bytes = recvfrom(sd,(void*)buffer, 80,0,&client, &clientLen); 

        if(bytes == -1){
            std::cerr<<"[recvfrom] error receiving";
        }

            getnameinfo(&client,clientLen,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICHOST|NI_NUMERICSERV);

            std::cout << "Host: " << host << "\tPort: " << service << std::endl;
            std::cout << "\tData: " << buffer << std::endl;
        }

        close(sd);

    return 0;
}
