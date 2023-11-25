/*
 * Copyright ©2023 Chris Thachuk.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "./ServerSocket.h"

#include <arpa/inet.h>   // for inet_ntop()
#include <errno.h>       // for errno, used by strerror()
#include <netdb.h>       // for getaddrinfo()
#include <stdio.h>       // for snprintf()
#include <string.h>      // for memset, strerror()
#include <sys/socket.h>  // for socket(), getaddrinfo(), etc.
#include <sys/types.h>   // for socket(), getaddrinfo(), etc.
#include <unistd.h>      // for close(), fcntl()

#include <iostream>  // for std::cerr, etc.

extern "C" {
#include "libhw1/CSE333.h"
}

namespace hw4 {

ServerSocket::ServerSocket(uint16_t port) {
  port_ = port;
  listen_sock_fd_ = -1;
}

ServerSocket::~ServerSocket() {
  // Close the listening socket if it's not zero.  The rest of this
  // class will make sure to zero out the socket if it is closed
  // elsewhere.
  if (listen_sock_fd_ != -1) close(listen_sock_fd_);
  listen_sock_fd_ = -1;
}

bool ServerSocket::BindAndListen(int ai_family, int* const listen_fd) {
  // Use "getaddrinfo," "socket," "bind," and "listen" to
  // create a listening socket on port port_.  Return the
  // listening socket through the output parameter "listen_fd"
  // and set the ServerSocket data member "listen_sock_fd_"

  // STEP 1:

  // Check if ai_family is reasonable
  if (ai_family != AF_INET && ai_family != AF_INET6 && ai_family != AF_UNSPEC) {
    return false;
  }

  // Populate the "hints" addrinfo structure for getaddrinfo().
  // ("man addrinfo")
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = ai_family;      // IPv6 (also handles IPv4 clients)
  hints.ai_socktype = SOCK_STREAM;  // stream
  hints.ai_flags = AI_PASSIVE;      // use wildcard "in6addr_any" address
  hints.ai_flags |= AI_V4MAPPED;    // use v4-mapped v6 if no v6 found
  hints.ai_protocol = IPPROTO_TCP;  // tcp protocol
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;

  // Cast the port number from int to char*.
  // getaddrinfo() returns a list of address structures
  // via the output parameter "result".
  struct addrinfo* result;
  std::string s = std::to_string(this->port_);
  int res = getaddrinfo(nullptr, s.c_str(), &hints, &result);

  // Did addrinfo() fail?
  if (res != 0) {
    std::cerr << "getaddrinfo() failed: ";
    std::cerr << gai_strerror(res) << std::endl;
    return false;
  }

  // Loop through the returned address structures until we are able
  // to create a socket and bind to one.  The address structures are
  // linked in a list through the "ai_next" field of result.
  int fd = -1;
  for (struct addrinfo* rp = result; rp != nullptr; rp = rp->ai_next) {
    fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (fd == -1) {
      // Creating this socket failed.  So, loop to the next returned
      // result and try again.
      std::cerr << "socket() failed: " << strerror(errno) << std::endl;
      fd = -1;
      continue;
    }

    // Configure the socket; we're setting a socket "option."  In
    // particular, we set "SO_REUSEADDR", which tells the TCP stack
    // so make the port we bind to available again as soon as we
    // exit, rather than waiting for a few tens of seconds to recycle it.
    int optval = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
        -1) {
      std::cerr << "setsockopt error: " << strerror(errno) << std::endl;
      close(fd);
      continue;
    }

    // Try binding the socket to the address and port number returned
    // by getaddrinfo().
    if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0) {
      // Bind worked!
      this->sock_family_ = rp->ai_family;
      break;
    }

    // The bind failed.  Close the socket, then loop back around and
    // try the next address/port returned by getaddrinfo().
    close(fd);
    fd = -1;
  }

  // Free the structure returned by getaddrinfo().
  freeaddrinfo(result);

  // If we failed to bind, return failure.
  if (fd == -1) return false;

  // Success. Tell the OS that we want this to be a listening socket.
  if (listen(fd, SOMAXCONN) != 0) {
    std::cerr << "Failed to mark socket as listening: ";
    std::cerr << strerror(errno) << std::endl;
    close(fd);
    return false;
  }

  // Sets listen_sock_fd_ to be the file descriptor
  *listen_fd = fd;
  this->listen_sock_fd_ = fd;
  return true;
}

bool ServerSocket::Accept(int* const accepted_fd,
                          std::string* const client_addr,
                          uint16_t* const client_port,
                          std::string* const client_dns_name,
                          std::string* const server_addr,
                          std::string* const server_dns_name) const {
  // Accept a new connection on the listening socket listen_sock_fd_.
  // (Block until a new connection arrives.)  Return the newly accepted
  // socket, as well as information about both ends of the new connection,
  // through the various output parameters.

  // STEP 2:
  int client_fd;
  struct sockaddr_storage caddr;

  while (1) {
    socklen_t caddr_len = sizeof(caddr);
    client_fd = accept(listen_sock_fd_,
                       reinterpret_cast<struct sockaddr*>(&caddr), &caddr_len);
    if (client_fd < 0) {
      if ((errno == EINTR) || (errno == EAGAIN) || (errno == EWOULDBLOCK))
        continue;
      std::cerr << "Failure on accept: " << strerror(errno) << std::endl;
      close(listen_sock_fd_);
      return false;
    }
    break;
  }

  *accepted_fd = client_fd;

  if (sock_family_ == AF_INET) {
    // IPv4
    char ip_str[INET_ADDRSTRLEN];
    struct sockaddr_in* client_addr_in = (struct sockaddr_in*)&caddr;
    inet_ntop(AF_INET, &(client_addr_in->sin_addr), ip_str, INET_ADDRSTRLEN);
    *client_addr = (std::string)ip_str;
    *client_port = ntohs(client_addr_in->sin_port);
  } else if (sock_family_ == AF_INET6) {
    // IPv6
    char ip_str[INET6_ADDRSTRLEN];
    struct sockaddr_in6* client_addr_in6 = (struct sockaddr_in6*)&caddr;
    inet_ntop(AF_INET6, &(client_addr_in6->sin6_addr), ip_str,
              INET6_ADDRSTRLEN);
    *client_addr = (std::string)ip_str;
    *client_port = ntohs(client_addr_in6->sin6_port);
  } else {
    std::cerr << "Unknown socket family" << std::endl;
    close(client_fd);
    return false;
  }

  char client_host[NI_MAXHOST];
  struct sockaddr* addr = reinterpret_cast<struct sockaddr*>(&caddr);
  int status =
      getnameinfo(reinterpret_cast<struct sockaddr*>(addr), sizeof(caddr),
                  client_host, NI_MAXHOST, nullptr, 0, 0);
  if (status != 0) {
    std::cerr << "getnameinfo error: " << gai_strerror(status) << std::endl;
    close(client_fd);
    return false;
  }
  *client_dns_name = (std::string)client_host;

  // get the server dns name and ip address
  char server_host[NI_MAXHOST];
  server_host[0] = '\0';
  if (sock_family_ == AF_INET) {
    // server use IPv4 address
    struct sockaddr_in srvr;
    socklen_t srvrlen = sizeof(srvr);
    char addrbuf[INET_ADDRSTRLEN];
    getsockname(client_fd, (struct sockaddr*)&srvr, &srvrlen);
    inet_ntop(AF_INET, &srvr.sin_addr, addrbuf, INET_ADDRSTRLEN);
    getnameinfo((const struct sockaddr*)&srvr, srvrlen, server_host, NI_MAXHOST,
                NULL, 0, 0);

    *server_addr = std::string(addrbuf);
    *server_dns_name = std::string(server_host);
  } else {
    // server uses IPv6 address
    struct sockaddr_in6 srvr;
    socklen_t srvrlen = sizeof(srvr);
    char addrbuf[INET6_ADDRSTRLEN];
    getsockname(client_fd, (struct sockaddr*)&srvr, &srvrlen);
    inet_ntop(AF_INET6, &srvr.sin6_addr, addrbuf, INET6_ADDRSTRLEN);
    getnameinfo((const struct sockaddr*)&srvr, srvrlen, server_host, NI_MAXHOST,
                NULL, 0, 0);

    *server_addr = std::string(addrbuf);
    *server_dns_name = std::string(server_host);
  }

  return true;
}

}  // namespace hw4
