#ifndef JJPROTO_HPP_
#define JJPROTO_HPP_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// The namespace is just designed to avoid the socket problems easily
// the forward 4 char gets the len it will read.
namespace jjprotocol {
ssize_t jrecv(int sockfd, char* buff, size_t nbytes, int flags) {
  char lenbuff[4] = {0};
  recv(sockfd, lenbuff, 4, flags);
  auto len = (decltype(nbytes))atoi(lenbuff);
  return recv(sockfd, buff, len, flags);
}

ssize_t jsend(int sockfd, const char* buff, size_t nbytes, int flags) {
  int len = (int)nbytes;
  char lenbuff[4] = {0};
  sprintf(lenbuff, "%04d", len);
  send(sockfd, lenbuff, 4, flags);
  return send(sockfd, buff, nbytes, flags);
}
}

#endif
