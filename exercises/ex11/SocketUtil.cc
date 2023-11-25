Copyright 2023 Alan Li
#include "SocketUtil.h"
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <iostream>

using std::cerr;
using std::endl;


#include <sys/types.h>
#include <cstring>

int Listen(char* portnum, int* sock_family) {
  struct addrinfo hints, *res, *p;
  int sockfd, status, yes = 1;

  // Set up hints struct for getaddrinfo
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // Get address info for the given port
  if ((status = getaddrinfo(nullptr, portnum, &hints, &res)) != 0) {
    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
    return -1;
  }

  // Loop through all the results and bind to the first we can
  for (p = res; p != nullptr; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      std::cerr << "socket error: " << strerror(errno) << std::endl;
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      std::cerr << "setsockopt error: " << strerror(errno) << std::endl;
      close(sockfd);
      continue;
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      std::cerr << "bind error: " << strerror(errno) << std::endl;
      close(sockfd);
      continue;
    }

    break;
  }

  freeaddrinfo(res);

  if (p == nullptr) {
    std::cerr << "Listen error: failed to bind" << std::endl;
    return -1;
  }

  if (listen(sockfd, SOMAXCONN) == -1) {
    std::cerr << "listen error: " << strerror(errno) << std::endl;
    close(sockfd);
    return -1;
  }

  *sock_family = p->ai_family;
  return sockfd;
}

int WrappedRead(int fd, unsigned char* buf, int readlen) {
  int res;
  while (1) {
    res = read(fd, buf, readlen);
    if (res == -1) {
      if ((errno == EAGAIN) || (errno == EINTR))
        continue;
    }
    break;
  }
  return res;
}

int WrappedWrite(int fd, unsigned char* buf, int writelen) {
  int res, written_so_far = 0;

  while (written_so_far < writelen) {
    res = write(fd, buf + written_so_far, writelen - written_so_far);
    if (res == -1) {
      if ((errno == EAGAIN) || (errno == EINTR))
        continue;
      break;
    }
    if (res == 0)
      break;
    written_so_far += res;
  }
  return written_so_far;
}
