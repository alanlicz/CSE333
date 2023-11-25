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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>

#include "SocketUtil.h"

using std::cerr;
using std::endl;

#define BUFSIZE 256

// Prints usage information about this program and exits with EXIT_FAILURE.
void Usage(char* progname);

// Open a local file, open a socket to a remote process,
// and send the file to other process.

int main(int argc, char** argv) {
  if (argc != 4) {
    cerr << "Wrong number of arguments." << endl;
    Usage(argv[0]);
  }

  // Attempt to open up the file
  int file_fd = open(argv[3], O_RDONLY);
  if (file_fd == -1) {
    cerr << "Unable to open file " << argv[3] << endl;
    Usage(argv[0]);
  }

  unsigned short port = 0;
  if (sscanf(argv[2], "%hu", &port) != 1) {
    cerr << "Invalid port number." << endl;
    close(file_fd);
    Usage(argv[0]);
  }

  // Get an appropriate sockaddr structure.
  struct sockaddr_storage addr;
  size_t addrlen;
  if (!LookupName(argv[1], port, &addr, &addrlen)) {
    cerr << "Unable to lookup name." << endl;
    close(file_fd);
    Usage(argv[0]);
  }

  // Connect to the remote host.
  int socket_fd;
  if (!Connect(addr, addrlen, &socket_fd)) {
    cerr << "Unable to connect to remote host." << endl;
    close(file_fd);
    Usage(argv[0]);
  }

  // Read from the input file, writing to the network socket.
  unsigned char readbuf[BUFSIZE];
  while (1) {
    int res = WrappedRead(file_fd, readbuf, BUFSIZE);
    if (res == 0)  // eof
      break;
    if (res < 0) {  // error
      close(socket_fd);
      close(file_fd);
      return EXIT_FAILURE;
    }

    int res2 = WrappedWrite(socket_fd, readbuf, res);
    if (res2 != res) {  // error
      close(socket_fd);
      close(file_fd);
      return EXIT_FAILURE;
    }
  }

  // Clean up.
  close(socket_fd);
  close(file_fd);
  return EXIT_SUCCESS;
}

void Usage(char* progname) {
  cerr << "usage: " << progname << " hostname port filename" << endl;
  exit(EXIT_FAILURE);
}