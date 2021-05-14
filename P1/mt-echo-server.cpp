#include <errno.h>
#include <iostream>
#include <string.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <time.h>

#include <thread>
class EchoServerThread {
private:
  int sd;
  char buffer[80];
  int brecv;

public:
  EchoServerThread(int sd) : sd(sd) {}
  void processMsg() {
    bool running = true;
    while (running) {
      memset(buffer, 0, 80);
      brecv=0;
      
      brecv = recv(sd, (void *)buffer, 79, 0);
      if (brecv <= 0) {
        std::cout << "Conexion terminada" << std::endl;
        running = false;
        break;
      }
      buffer[brecv] = '\0';

      int bytessent = send(sd, buffer, brecv, 0);

      if (bytessent != brecv) {
        std::cout << "Se han perdido bytes" << std::endl;
      }
    }
    close(sd);
  }
};
// ./echo-server <dir> <puerto>
int main(int argc, char **argv) {

  addrinfo hints;
  addrinfo *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_flags = AI_PASSIVE;     // devuelve 0.0.0.0
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_family = AF_INET;       // IPv4

  int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

  if (rc != 0) {
    std::cerr << gai_strerror(rc) << "\n";
    return -1;
  }

  int sd = socket(result->ai_family, result->ai_socktype, 0);

  if (sd == -1) {
    std::cerr << "[socket] failed to create, errno " << sd;
  }

  int rcb = bind(sd, result->ai_addr, result->ai_addrlen);

  if (rcb != 0) {
    std::cerr << "[bind] failed errno: " << errno;
    return -1;
  }

  freeaddrinfo(result);
  int rlisten = listen(sd, 16);
  if (rlisten == -1) {
    std::cerr << "[listen] failed, errno " << errno << std::endl;
  }

  struct sockaddr client;
  socklen_t clientLen = sizeof(struct sockaddr);
  char host[NI_MAXHOST];
  char service[NI_MAXSERV];
  while (true) {

    int client_sd = accept(sd, &client, &clientLen);

    int rname = getnameinfo(&client, clientLen, host, NI_MAXHOST, service,
                            NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    if (rname != 0) {
      std::cerr << gai_strerror(rname) << "\n";
      return 1;
    }
    std::cout << "Conexion desde " << host << ":" << service << std::endl;

    EchoServerThread *th = new EchoServerThread(client_sd);

    std::thread([&th]() {
      th->processMsg();
      delete th;
    }).detach();

  }
}
