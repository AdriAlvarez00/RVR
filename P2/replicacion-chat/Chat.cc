#include "Chat.h"
#include <bits/stdint-uintn.h>
#include <cstring>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* aux = _data;
    memcpy(aux, &type,sizeof(uint8_t));
    aux+=sizeof(uint8_t);
    memcpy(aux,nick.data(),NICK_LEN);
    aux+=NICK_LEN;
    memcpy(aux,message.data(),MESSAGE_LEN);
    aux+=MESSAGE_LEN;
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* aux = _data;
    memcpy(&type,aux,sizeof(uint8_t));
    aux+=sizeof(uint8_t);
    nick.resize(NICK_LEN);
    memcpy((void*)nick.data(),aux,NICK_LEN);
    aux+=NICK_LEN;
    message.resize(MESSAGE_LEN);
    memcpy((void*)message.data(),aux,MESSAGE_LEN);
    aux+=MESSAGE_LEN;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
	ChatMessage msg;
	socket.recv(msg);

	if(msg.type == msg.LOGIN){
		std::cout << "LOGIN\n";
	}
	else if(msg.type == msg.LOGOUT){
		std::cout << "LOGOUT\n";
	}
	else if(msg.type == msg.MESSAGE){
		std::cout << "MESSAGGE\n";
	}
	else{
		std::cerr << "Error receiving" << std::endl;
		break;
	}
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

