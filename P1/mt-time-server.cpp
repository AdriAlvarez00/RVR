#include <iostream>
#include <string.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <time.h>

#include <thread>

class TimeServerThread {
private:
  int sd;
  int tid = -1;

public:
  TimeServerThread(int sd) : sd(sd) {}
  void proccessMsg() {
    char buffer[80];

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    struct sockaddr client;
    socklen_t clientLen = sizeof(struct sockaddr);

    while (true) {
      int bRecv = recvfrom(sd, (void *)buffer, 79, 0, &client, &clientLen);
      getnameinfo(&client, clientLen, host, NI_MAXHOST, service, NI_MAXSERV,
                  NI_NUMERICHOST | NI_NUMERICSERV);
      std::cout << bRecv << " bytes de " << host << ":" << service
                << " recibidos en el thread " << std::this_thread::get_id()
                << std::endl;
      if (bRecv == -1) {
        std::cout << "[recvfrom] fallo al recibir" << std::endl;
        return;
      };

      char retBuffer[80];
      int retBuffLen = 0;

      // simulamos un procesamiento costoso
      sleep(rand() % 5);
      switch (buffer[0]) {
      case 't': {
        time_t epoch = time(NULL);
        retBuffLen = strftime(retBuffer, sizeof(retBuffer), "%I:%M:%S %p",
                              localtime(&epoch));
        break;
      }
      case 'd': {
        time_t epoch = time(NULL);
        retBuffLen =
            strftime(retBuffer, sizeof(retBuffer), "%D", localtime(&epoch));
        break;
      }
      default:
        std::cout << "Comando no soportado " << buffer[0] << std::endl;
        break;
      }

      if (retBuffLen > 0) {
        int bSent = sendto(sd, retBuffer, retBuffLen, 0, &client, clientLen);
        if (bSent != retBuffLen) {
          std::cout << "[sendto] no pudo enviar todos los bytes" << std::endl;
        }
      }
    }
  }
};

#define MAX_THREADS 20

void serverThread(int sd) {}
// ./time-server <dir> <puerto>
int main(int argc, char **argv) {

  addrinfo hints;
  addrinfo *result;
  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_flags = AI_PASSIVE;    // devuelve 0.0.0.0
  hints.ai_socktype = SOCK_DGRAM; // UDP
  hints.ai_family = AF_INET;      // IPv4

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
    std::cerr << "[bind] failed\n";
    return -1;
  }

  freeaddrinfo(result);

  std::thread pool[MAX_THREADS];
  for (int i = 0; i < MAX_THREADS; i++) {
    TimeServerThread *th = new TimeServerThread(sd);

    std::thread([&th]() {
      th->proccessMsg();
      delete th;
    }).detach();
  }

  char in;
    while (true) {
      std::cin >> in;
      if (in == 'q')
        break;
    }

//   try {
//     for (int i = 0; i < MAX_THREADS; i++) {
// 		if(!pool[i].joinable()){
// 			std::cout << "thread "<<i<<" no es joinable\n";
// 		}
//       pool[i].join();
//     }
//   } catch (const std::system_error &e) {
//     std::cout << e.what() << '\n';
//     std::cout << e.code() << '\n';
//   } catch (const std::exception &e) {
//     std::cout << e.what() << '\n';
//   }

  close(sd);

  return 0;
}
