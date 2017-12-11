#include <iostream>
#include "jjprotocol.hpp"
#define BUF_SIZE 1024

int main(int argc, char const* argv[]) {
  std::cout << "Please input the ip and the port" << std::endl;
  std::cout << "ip :";
  std::string ip;
  std::cin >> ip;
  std::cout << "port :";
  int port;
  std::cin >> port;

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
  // serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(port);

  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
  connect(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

  char rem[BUF_SIZE] = {0};
  char comm[BUF_SIZE] = {0};
  char report[BUF_SIZE] = {0};
  bool is_continue = true;

  jjprotocol::jrecv(serv_sock, report, BUF_SIZE, 0);
  printf("%s\n", report);
  if (!strncmp(report, "Connect_failed", 14)) return 1;
  memset(report, 0, BUF_SIZE);
  std::cout << std::endl;

  while (is_continue) {
    jjprotocol::jrecv(serv_sock, rem, BUF_SIZE, 0);
    printf("Instruction from server : %s\n", rem);
    printf("Input a command: ");
    scanf("%s", comm);
    if (*comm == 'q') is_continue = false;
    jjprotocol::jsend(serv_sock, comm, strlen(comm) + 1, 0);
    jjprotocol::jrecv(serv_sock, report, BUF_SIZE, 0);
    printf("Message from server: %s\n", report);

    memset(rem, 0, BUF_SIZE);
    memset(comm, 0, BUF_SIZE);
    memset(report, 0, BUF_SIZE);
  }

  close(serv_sock);
  return 0;
}
