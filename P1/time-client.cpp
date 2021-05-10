#include <iostream>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#include <time.h>
// ./time-client <dir> <puerto> <comando>
int main(int argc, char** argv){

	if(argc != 4){
		std::cout << "usage time-client dir port command" << std::endl;
	return -1;
	}
	else if(strlen(argv[3]) != 1){
		std::cout << "Wrond command format\ntime: t\tdate: d\t close server: q: " << std::endl;
	return -1;
	}
	addrinfo hints;
	addrinfo* result;
	memset(&hints,0,sizeof(struct addrinfo));

	hints.ai_socktype = SOCK_DGRAM; //UDP
	hints.ai_family = AF_INET;//IPv4

	int rc = getaddrinfo(argv[1],argv[2],&hints,&result);

	if(rc!=0){
		std::cerr << gai_strerror(rc) << std::endl;
		return -1;
	}

	int sd = socket(result->ai_family,result->ai_socktype,0);

	if(sd == -1){
		std::cerr << "[socket] failed to create. Errno " << sd << std::endl;
	}



	int rsend = sendto(sd,argv[3],strlen(argv[3]),0,result->ai_addr,result->ai_addrlen);

	if(rsend == -1){
		std::cerr << "[sendto] error sending msg" << std::endl;
		return -1;
	}
	else if( rsend < 1){
		std::cerr << "[sendto] not all bytes were sent" << std::endl;
	}

	freeaddrinfo(result);

	char buffer[80];
	memset(buffer,0,80);
	int rbytes = recvfrom(sd,(void*)buffer,80,0,nullptr,nullptr);
	if(rbytes == -1){
		std::cerr << "[rbytes] error receiving" <<std::endl;
	}

	std::cout << buffer << std::endl;
}
