#include "AgendaSocket.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "jjprotocol.hpp"
#define BUF_SIZE 1024

// global various
std::stringstream ss;
bool socket_continue = true;
char operation[BUF_SIZE] = {0};
char buffer[BUF_SIZE] = {0};

inline void split() {
  ss << "--------------------------------------" << std::endl;
  std::cout << "----------------------------------------" << std::endl;
}

// functions definition
AgendaSocket::AgendaSocket() {}

void AgendaSocket::OperationLoop() {
  startAgenda();

  std::string t_ip;
  std::cout << "Please input the ip" << std::endl;
  std::cin >> t_ip;
  int t_port;
  std::cout << "Please input the port" << std::endl;
  std::cin >> t_port;

  int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(t_ip.c_str());
  // serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(t_port);
  bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

  listen(serv_sock, 30);

  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  std::cout << "Waiting for the connection..." << std::endl;
  int clnt_sock =
      accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
  try {
    jjprotocol::jsend(clnt_sock, "Connect_success",
                      strlen("Connet_success") + 1, 0);
    std::cout << "Connection Success" << std::endl;
  } catch (...) {
    jjprotocol::jsend(clnt_sock, "Connect_failed", strlen("Connect_failed") + 1,
                      0);
    std::cout << "Connection failed" << std::endl;
  }

  // sleep(1);
  while (socket_continue) {
    std::cout << "Waiting for the command..." << std::endl;

    jjprotocol::jsend(clnt_sock, "A new operation requrie :",
                      strlen("A new operation requrie :") + 1, 0);
    jjprotocol::jrecv(clnt_sock, operation, BUF_SIZE, 0);
    jjprotocol::jsend(clnt_sock, operation, strlen(operation) + 1, 0);

    std::cout << "The operation from the client is :" << operation << std::endl;
    executeOperation(operation, clnt_sock);

    std::cout << "The report is:" << std::endl;
    std::cout << ss.str() << std::endl;
    std::cout << "Finish the execution" << std::endl;

    jjprotocol::jsend(clnt_sock, "Finish, use \'get\' to get the result",
                      strlen("Finish, use \'get\' to get the result") + 1, 0);
    jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
    jjprotocol::jsend(clnt_sock, ss.str().c_str(),
                      ss.str().size() + sizeof(char), 0);
    ss.str("");

    memset(buffer, 0, BUF_SIZE);
    memset(operation, 0, BUF_SIZE);
  }
  close(clnt_sock);
  close(serv_sock);
  quitAgenda();
}

void shelp(const std::string judge) {
  ss << "====================Here is the shelp file ======================="
     << std::endl;
  if (judge.empty()) {
    ss << "You still not log in!" << std::endl;
    ss << "help -> print the help file" << std::endl;
    ss << "re -> register a new account" << std::endl;
    ss << "login -> log in your account" << std::endl;
    ss << "clear -> clear the screen" << std::endl;
    ss << "q -> quit the Agenda" << std::endl;
  } else {
    ss << "Be careful not to input wrong command :)" << std::endl;
    ss << "help -> print the help file" << std::endl;
    ss << "logout -> log out your account" << std::endl;
    ss << "q -> quit the Agenda" << std::endl;
    ss << "du -> delete your own account" << std::endl;
    ss << "lu -> list all users from storage" << std::endl;
    ss << "cm -> create a meeting with someone else" << std::endl;
    ss << "lam -> list all meetings from storage" << std::endl;
    ss << "lsm -> list all meetings that this user sponsored" << std::endl;
    ss << "lpm -> list all meetings that this user take part in" << std::endl;
    ss << "qbtitle -> search meetings bu title from storage" << std::endl;
    ss << "qbtime -> search meetings by timeinterval from sotrage" << std::endl;
    ss << "dbtitle -> delete meetings by title from storage" << std::endl;
    ss << "dall -> delete all meetings that this user sponsored" << std::endl;
  }
}

void AgendaSocket::startAgenda() { m_agendaService.startAgenda(); }

void AgendaSocket::quitAgenda() {
  std::cout << "Quit Agenda!" << std::endl;
  m_agendaService.quitAgenda();
}

bool AgendaSocket::executeOperation(const std::string t_operation,
                                    int& clnt_sock) {
  if (t_operation == "help") {
    shelp(m_userName);
  } else if (t_operation == "re") {
    userRegister(clnt_sock);
  } else if (t_operation == "login") {
    userLogIn(clnt_sock);
  } else if (t_operation == "q") {
    return false;
  } else if (t_operation == "logout") {
    userLogOut(clnt_sock);
  } else if (t_operation == "du") {
    deleteUser(clnt_sock);
  } else if (t_operation == "lu") {
    listAllUsers(clnt_sock);
  } else if (t_operation == "cm") {
    createMeeting(clnt_sock);
  } else if (t_operation == "lam") {
    listAllMeetings(clnt_sock);
  } else if (t_operation == "lsm") {
    listAllSponsorMeetings(clnt_sock);
  } else if (t_operation == "lpm") {
    listAllParticipateMeetings(clnt_sock);
  } else if (t_operation == "qbtitle") {
    queryMeetingByTitle(clnt_sock);
  } else if (t_operation == "qbtime") {
    queryMeetingByTimeInterval(clnt_sock);
  } else if (t_operation == "dbtitle") {
    deleteMeetingByTitle(clnt_sock);
  } else if (t_operation == "dall") {
    deleteAllMeetings(clnt_sock);
  }
  return true;
}

// under this ...
void AgendaSocket::userLogIn(int& clnt_sock) {
  std::vector<std::string> info;
  int count = 2;
  std::cout << "Login : Input Information..." << std::endl;
  while (count--) {
    jjprotocol::jsend(clnt_sock, "Input :", strlen("Input :") + 1, 0);
    jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
    jjprotocol::jsend(clnt_sock, buffer, strlen(buffer) + 1, 0);
    info.push_back(std::string(buffer));
    memset(buffer, 0, BUF_SIZE);
  }
  if (m_agendaService.userLogIn(info[0], info[1])) {
    m_userName = info[0];
    m_userPassword = info[1];
    // jjprotocol::jsend(clnt_sock, "success", sizeof("success"), 0);
    ss << "success" << std::endl;
  } else {
    // jjprotocol::jsend(clnt_sock, "fail", sizeof("fail"), 0);
    ss << "fail" << std::endl;
  }
}

void AgendaSocket::userRegister(int& clnt_sock) {
  // char buffer[BUF_SIZE] = {0};
  std::vector<std::string> info;
  int count = 4;
  std::cout << "Register : input information..." << std::endl;
  while (count--) {
    jjprotocol::jsend(clnt_sock, "Input :", strlen("Input :") + 1, 0);
    jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
    jjprotocol::jsend(clnt_sock, buffer, BUF_SIZE, 0);
    info.push_back(std::string(buffer));
    memset(buffer, 0, BUF_SIZE);
  }
  if (info.size() == 4 &&
      m_agendaService.userRegister(info[0], info[1], info[2], info[3])) {
    // jjprotocol::jsend(clnt_sock, "success", sizeof("success"), 0);
    ss << "success" << std::endl;
  } else {
    // jjprotocol::jsend(clnt_sock, "fail", sizeof("fail"), 0);
    ss << "fail" << std::endl;
  }
}

void AgendaSocket::userLogOut(int& clnt_sock) {
  // jjprotocol::jsend(clnt_sock, "Log out successfully", sizeof("Log out
  // successfully"),
  // 0);
  ss << "Log out successfully." << std::endl;
  m_userName = "";
  m_userPassword = "";
}

void AgendaSocket::deleteUser(int& clnt_sock) {
  std::cout << "Delete User: input your password." << std::endl;
  // char password[BUF_SIZE] = {0};
  jjprotocol::jsend(clnt_sock, "Input :", strlen("Input :") + 1, 0);
  jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
  jjprotocol::jsend(clnt_sock, buffer, BUF_SIZE, 0);
  if (m_agendaService.deleteUser(m_userName, buffer)) {
    ss << "Delete Successfully!" << std::endl;
    userLogOut(clnt_sock);
  } else {
    ss << "Delete Failed!" << std::endl;
  }
  memset(buffer, 0, BUF_SIZE);
}

void AgendaSocket::listAllUsers(int& clnt_sock) {
  std::cout << "List All Users:" << std::endl;
  ss << "All users in the Storage:" << std::endl;
  auto result = m_agendaService.listAllUsers();
  for (const auto& user : result) {
    ss << user.getName() << std::endl;
    ss << user.getEmail() << std::endl;
    ss << user.getPhone() << std::endl;
    split();
  }
}

void AgendaSocket::createMeeting(int& clnt_sock) {
  // char buffer[BUF_SIZE] = {0};
  std::vector<std::string> info;
  for (int i = 0; i < 3; ++i) {
    jjprotocol::jsend(clnt_sock, "Input :", strlen("Input :") + 1, 0);
    jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
    jjprotocol::jsend(clnt_sock, buffer, strlen(buffer) + 1, 0);
    info.push_back(std::string(buffer));
    memset(buffer, 0, BUF_SIZE);
  }
  std::cout << "Please input the number of paticipatior" << std::endl;
  jjprotocol::jsend(clnt_sock, "Please input the number of paticipatior",
                    strlen("Please input the number of paticipatior") + 1, 0);
  jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
  jjprotocol::jsend(clnt_sock, buffer, strlen(buffer) + 1, 0);
  int count = std::stoi(std::string(buffer));
  memset(buffer, 0, BUF_SIZE);
  std::vector<std::string> participator;
  while (count--) {
    jjprotocol::jsend(clnt_sock, "Input :", strlen("Input :") + 1, 0);
    jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
    jjprotocol::jsend(clnt_sock, buffer, strlen(buffer) + 1, 0);
    participator.push_back(std::string(buffer));
    memset(buffer, 0, BUF_SIZE);
  }
  if (m_agendaService.createMeeting(m_userName, info[0], info[1], info[2],
                                    participator)) {
    ss << "create meeting successfully!" << std::endl;
  } else {
    ss << "create meeting failed !" << std::endl;
  }
}

void AgendaSocket::listAllMeetings(int& clnt_sock) {
  ss << "list all meetings :" << std::endl;
  printMeetings(m_agendaService.listAllMeetings(m_userName));
}

void AgendaSocket::listAllSponsorMeetings(int& clnt_sock) {
  ss << "list all sponsor meetings" << std::endl;
  printMeetings(m_agendaService.listAllSponsorMeetings(m_userName));
}

void AgendaSocket::listAllParticipateMeetings(int& clnt_sock) {
  ss << "list all participate meetings" << std::endl;
  printMeetings(m_agendaService.listAllParticipateMeetings(m_userName));
}

void AgendaSocket::queryMeetingByTitle(int& clnt_sock) {
  ss << "Query Meeting by title:" << std::endl;
  // char buffer[BUF_SIZE] = {0};
  jjprotocol::jsend(clnt_sock, "Input :", strlen("Input :") + 1, 0);
  jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
  jjprotocol::jsend(clnt_sock, buffer, strlen(buffer) + 1, 0);
  std::string title(buffer);
  memset(buffer, 0, BUF_SIZE);
  ss << "Query Meeting by title:" << std::endl;
  printMeetings(m_agendaService.meetingQuery(m_userName, title));
}

void AgendaSocket::queryMeetingByTimeInterval(int& clnt_sock) {
  std::cout << "Please input the startDate and the endDate." << std::endl;
  // char buffer[BUF_SIZE] = {0};
  jjprotocol::jsend(clnt_sock, "Input :", strlen("Input :") + 1, 0);
  jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
  jjprotocol::jsend(clnt_sock, buffer, strlen(buffer) + 1, 0);
  std::string startDate(buffer);
  memset(buffer, 0, BUF_SIZE);

  jjprotocol::jsend(clnt_sock, "Input :", strlen("Input :") + 1, 0);
  jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
  jjprotocol::jsend(clnt_sock, buffer, strlen(buffer) + 1, 0);
  std::string endDate(buffer);
  memset(buffer, 0, BUF_SIZE);

  ss << "query meeting by time interval:" << std::endl;
  printMeetings(m_agendaService.meetingQuery(m_userName, startDate, endDate));
}

void AgendaSocket::deleteMeetingByTitle(int& clnt_sock) {
  std::cout << "Please input the title:" << std::endl;
  // char title[BUF_SIZE] = {0};
  jjprotocol::jsend(clnt_sock, "Input :", strlen("Input :") + 1, 0);
  jjprotocol::jrecv(clnt_sock, buffer, BUF_SIZE, 0);
  jjprotocol::jsend(clnt_sock, buffer, strlen(buffer) + 1, 0);
  if (m_agendaService.deleteMeeting(m_userName, buffer)) {
    ss << "success" << std::endl;
  } else {
    ss << "failed" << std::endl;
  }
  memset(buffer, 0, BUF_SIZE);
}

void AgendaSocket::deleteAllMeetings(int& clnt_sock) {
  if (m_agendaService.deleteAllMeetings(m_userName)) {
    ss << "success" << std::endl;
  } else {
    ss << "failed" << std::endl;
  }
}

void AgendaSocket::printMeetings(std::list<Meeting> t_meetings) {
  for (const auto& meeting : t_meetings) {
    ss << "Meeting Title: " << meeting.getTitle() << std::endl;
    ss << "Meeting Sponsor: " << meeting.getSponsor() << std::endl;
    ss << "Meeting participators:";
    for (const auto& par_name : meeting.getParticipator()) {
      ss << par_name << " ";
    }
    ss << "\nMeeting startDate: " << Date::dateToString(meeting.getStartDate())
       << std::endl;
    ss << "Meeting endDate: " << Date::dateToString(meeting.getEndDate())
       << std::endl;
    split();
  }
}
