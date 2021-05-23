#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

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
	alloc_data(sizeof(int16_t) * 2 //x and y pos
			+ sizeof(char)*MAX_NAME); //name size
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

    void show_info(){
	    std::cout << "Player " << name <<
		    " at x: " << x << " y: " << y << std::endl;
    } 


private:
    static const size_t MAX_NAME = 20;

    char name[MAX_NAME];

    int16_t x;
    int16_t y;

};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    int fd = open("./player.data",O_CREAT|O_WRONLY|O_TRUNC,0666);
    write(fd,one_w.data(),one_w.size());
    close(fd);
    // 3. Leer el fichero
    char* serFile =(char*)malloc(one_w.size());
    fd = open("./player.data",O_RDONLY,0666);
    read(fd,serFile,one_w.size());
    close(fd);
    // 4. "Deserializar" en one_r
    one_r.from_bin(serFile);
    // 5. Mostrar el contenido de one_r
    one_r.show_info();
    return 0;
}

