// Create a listening socket, accept a connection from a client,
// and write all the data the client sends to stdout.
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include "SocketUtil.h"


int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
    return 1;
  }

  char* port = argv[1];
  int sock_family;
  int sockfd = Listen(port, &sock_family);

  if (sockfd == -1) {
    std::cerr << "Listen error" << std::endl;
    return 1;
  }

  std::cout << "Listening on port " << port << std::endl;

  struct sockaddr_storage client_addr;
  socklen_t client_addr_len = sizeof client_addr;
  int clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len);

  if (clientfd == -1) {
    std::cerr << "Accept error: " << strerror(errno) << std::endl;
    close(sockfd);
    return 1;
  }
  
  char buf[1024];
  ssize_t nread;
  while ((nread = read(clientfd, buf, sizeof buf)) > 0) {
    write(STDOUT_FILENO, buf, nread);
  }

  if (nread == -1) {
    std::cerr << "Read error: " << strerror(errno) << std::endl;
    close(clientfd);
    close(sockfd);
    return 1;
  }

  close(clientfd);
  close(sockfd);
  return 0;
}
