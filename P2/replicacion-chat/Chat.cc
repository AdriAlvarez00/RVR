#include "Chat.h"
#include "Socket.h"
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
    memcpy(aux,nick.c_str(),NICK_LEN);
    aux+=NICK_LEN;
    memcpy(aux,message.c_str(),MESSAGE_LEN);
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
    nick = aux;
    aux+=NICK_LEN;
    message = aux;
    aux+=MESSAGE_LEN;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::removeClient(Socket* sock){
	auto it = clients.begin();
	while(it != clients.end() && **it != *sock)
		it++;
	if(it == clients.end()){
		std::cout << "sock is not a client\n";
		return;
	}
	clients.erase(it);	
}

void ChatServer::broadcastMessage(ChatMessage& msg,Socket *sender){
	for(auto it = clients.begin();it!=clients.end();++it){
		if(**it != *sender) 
			socket.send(msg,**it);
	}
}
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
	Socket *msgSock = new Socket(socket);
	socket.recv(msg,msgSock);

	if(msg.type == msg.LOGIN){
		std::cout << "LOGIN " << *msgSock << "\n";
		std::unique_ptr<Socket> uptr = std::make_unique<Socket>(*msgSock);
		msgSock=nullptr;
		clients.push_back(std::move(uptr));
	}
	else if(msg.type == msg.LOGOUT){
		std::cout << "LOGOUT " << *msgSock << "\n";
		removeClient(msgSock);
	}
	else if(msg.type == msg.MESSAGE){
		std::cout << "MESSAGGE " << *msgSock << "\n";
		broadcastMessage(msg,msgSock);
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
    std::string msg;

    ChatMessage outMsg = ChatMessage(nick,msg);
    outMsg.type = ChatMessage::LOGOUT;

    socket.send(outMsg,socket);

	
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
	    std::string str;
	    std::getline(std::cin,str);
        // Enviar al servidor usando socket
	ChatMessage msg = ChatMessage(nick,str);
	msg.type = ChatMessage::MESSAGE;
	socket.send(msg,socket);
    }
}

void ChatClient::net_thread()
{
    ChatMessage inMsg;
    Socket* inSock = new Socket(socket);
    while(true)
    {
        //Recibir Mensajes de red
	socket.recv(inMsg,inSock);
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
	std::cout << inMsg.nick << ": " << inMsg.message << std::endl;
    }
}

