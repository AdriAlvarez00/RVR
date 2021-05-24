#include "Serializable.h"

#include <bits/stdint-intn.h>
#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
	alloc_data(SERIAL_SIZE); //name size
	char* aux = _data;
	//serializamos la posicion
	memcpy(aux,&x,sizeof(int16_t));
	aux+=sizeof(int16_t);
	memcpy(aux,&y,sizeof(int16_t));
	aux+=sizeof(int16_t);
       //serializamos el nombre
       memcpy(aux,name,MAX_NAME);
    }

    int from_bin(char * data)
    {
        
	char* aux = data;
	memcpy(&x,aux,sizeof(int16_t));
	aux+=sizeof(int16_t);
	memcpy(&y,aux,sizeof(int16_t));
	aux+=sizeof(int16_t);
       memcpy(name,aux,MAX_NAME);
        return 0;
    }

    int to_file(char* filename){
	to_bin();
    	int fd = open("./player.data",O_CREAT|O_WRONLY|O_TRUNC,0666);
	write(fd,_data,_size);
    	close(fd);
    	return 0;
    }

    int from_file(char *filename){
	char* serFile =(char*)malloc(SERIAL_SIZE);
	int fd = open("./player.data",O_RDONLY,0666);
    	read(fd,serFile,SERIAL_SIZE);
    	close(fd);
	from_bin(serFile);
	return 0;
    }

    void show_info(){
	    std::cout << "Player " << name <<
		    " at x: " << x << " y: " << y << std::endl;
    } 


private:
    static const size_t MAX_NAME = 20;
    static const size_t SERIAL_SIZE = sizeof(char)*MAX_NAME+sizeof(int16_t)*2;

    char name[MAX_NAME];

    int16_t x;
    int16_t y;

};

int main(int argc, char **argv)
{
	if(argc != 2){
		std::cerr << "ex3 g para generar un jugador aletorio\n"<<
			"ex3 r para recuperar el jugador de player.data" << std::endl;}
	else if(*argv[1] == 'g'){
		srand(time(NULL));
		Jugador player("PLAYER#3",rand()%1000,rand()%1000);
		player.to_file("./player.data");
		std::cout << "Guardadado\n";
		player.show_info();
	}
	else if(*argv[1] == 'r'){
    		Jugador recPlayer("", 0, 0);
		recPlayer.from_file("./player.data");
		std::cout << "Recuperado\n";
		recPlayer.show_info();
	}
	else{
		std::cerr << "ex3 g para generar un jugador aletorio\n"<<
			"ex3 r para recuperar el jugador de player.data" << std::endl;}

}

