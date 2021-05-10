#include <iostream>
#include <iomanip>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#include <time.h>

// ./echo-server <dir> <puerto>
int main(int argc, char** argv)
{
	
        addrinfo hints;
        addrinfo* result;
        memset(&hints,0,sizeof(struct addrinfo));
        hints.ai_flags = AI_PASSIVE; //devuelve 0.0.0.0
        hints.ai_socktype = SOCK_STREAM;//TCP
        hints.ai_family = AF_INET;//IPv4

	int rc = getaddrinfo(argv[1], argv[2] ,&hints,&result);

	        int sd = socket(result->ai_family,result->ai_socktype,0);

        if(sd == -1){
                std::cerr << "[socket] failed to create, errno " << sd;
        }

	int rcon = connect(sd,result->ai_addr, result->ai_addrlen);
	if(rcon != 0){
		std::cerr << "[connect] conection failed errno: " << errno << std::endl;
	}

	//TODO averiguar por que convierte los espacios en saltos de linea
	while(true){
		char buffer[80];
		memset(buffer,0,80);
		std::cin >> std::setw(79) >> buffer;
		int length = strlen(buffer);
		buffer[length] = '\0';
		if(length==1 && buffer[0]=='Q'){
			break;
		}
		int bSent = send(sd,buffer,length,0);
		if(bSent != length)
			std::cout << "Se perdieron bytes al enviar\n";
		int bRecv = recv(sd,buffer,length,0);
		if(bRecv != bSent)
			std::cout << "Se perdieron bytes en la recepcion\n";

		std::cout << buffer << std::endl;
	}

	close(sd);
}
