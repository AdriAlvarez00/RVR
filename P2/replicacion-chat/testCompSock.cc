#include "Socket.h"
#include "Socket.cc"

void cmpSocks(const Socket& a, const Socket&b){
	std::cout << "a: " << a << " b: " << b;
	if(a==b)
		std::cout << " IGUALES";
	else
		std::cout << " DISTINTOS";
	std::cout << std::endl;
}
int main(){
	Socket a("localhost","9999");
	Socket b("localhost","1234");
	Socket a2("localhost","9999");
	
	cmpSocks(a,b);
	cmpSocks(a,a2);
}
