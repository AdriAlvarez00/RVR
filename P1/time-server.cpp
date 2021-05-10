#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#include <time.h>
// ./time-server <dir> <puerto>
int main(int argc, char** argv)
{

	addrinfo hints;
	addrinfo* result;
	memset(&hints,0,sizeof(struct addrinfo));

	hints.ai_flags = AI_PASSIVE; //devuelve 0.0.0.0
	hints.ai_socktype = SOCK_DGRAM;//UDP
	hints.ai_family = AF_INET;//IPv4

	int rc = getaddrinfo(argv[1], argv[2] ,&hints,&result);

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

	bool running = true;
	while(running){
		char buffer[80];
		char host[NI_MAXHOST];
		char service[NI_MAXSERV];

		struct sockaddr client;
		socklen_t clientLen = sizeof(struct sockaddr); 

		int bytes = recvfrom(sd,(void*)buffer, 80,0,&client, &clientLen); 
		
		char retBuffer[80];
		memset(retBuffer,0,80);
		int retSize;
		if(bytes == -1){
			std::cerr<<"[recvfrom] error receiving";
		}
	//else if( bytes > 1){
	//	std::cerr<<"[recvfrom] wrong command format\n";
	//}
	else{
	switch(buffer[0]){
		case 'q':
			std::cout << "Saliendo...\n";
			running = false;
			break;
		case 't':{
			time_t epoch = time(NULL);
			retSize = strftime(retBuffer,sizeof(retBuffer),"%I:%M:%S %p",localtime(&epoch));
			break;}
		case 'd':{
			time_t epoch = time(NULL);
			retSize = strftime(retBuffer,sizeof(retBuffer),"%D",localtime(&epoch));
			break;}
		default:
			std::cout << "Comando no soportado " << buffer[0] << std::endl;
			break;
		}
	}
	if(running){
			getnameinfo(&client,clientLen,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICHOST|NI_NUMERICSERV);
			sendto(sd,retBuffer,retSize,0,&client,clientLen);
			std::cout << bytes << " bytes de " << host << ":" << service <<  std::endl;
	}
			
	}

		close(sd);

	return 0;
}
