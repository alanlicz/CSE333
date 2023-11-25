/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent.  Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "SocketUtil.h"

#include <unistd.h>
#include <string.h>

#include <iostream>

using std::cerr;
using std::endl;

bool LookupName(char* name,
                unsigned short port,
                struct sockaddr_storage* ret_addr,
                size_t* ret_addrlen) {
  struct addrinfo hints, *results;
  int retval;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // Do the lookup by invoking getaddrinfo().
  if ((retval = getaddrinfo(name, nullptr, &hints, &results)) != 0) {
    cerr << "getaddrinfo failed: ";
    cerr << gai_strerror(retval) << endl;
    return false;
  }

  // Set the port in the first result.
  if (results->ai_family == AF_INET) {
    struct sockaddr_in* v4addr;
    v4addr = reinterpret_cast<struct sockaddr_in*>(results->ai_addr);
    v4addr->sin_port = htons(port);
  } else if (results->ai_family == AF_INET6) {
    struct sockaddr_in6* v6addr;
    v6addr = reinterpret_cast<struct sockaddr_in6*>(results->ai_addr);
    v6addr->sin6_port = htons(port);
  } else {
    cerr << "getaddrinfo failed to provide an IPv4 or IPv6 address" << endl;
    freeaddrinfo(results);
    return false;
  }

  // Return the first result.
  memcpy(ret_addr, results->ai_addr, results->ai_addrlen);
  *ret_addrlen = results->ai_addrlen;

  // Clean up.
  freeaddrinfo(results);
  return true;
}

bool Connect(const struct sockaddr_storage& addr,
             const size_t& addrlen,
             int* ret_fd) {
  // Create the socket.
  int socket_fd = socket(addr.ss_family, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    cerr << "socket() failed: " << strerror(errno) << endl;
    return false;
  }

  // Connect the socket to the remote host.
  int res = connect(socket_fd,
                    reinterpret_cast<const sockaddr*>(&addr),
                    addrlen);
  if (res == -1) {
    cerr << "connect() failed: " << strerror(errno) << endl;
    close(socket_fd);
    return false;
  }

  *ret_fd = socket_fd;
  return true;
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