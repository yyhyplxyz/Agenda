#ifndef AGENDASOCKET_H
#define AGENDASOCKET_H

#include "AgendaService.hpp"

class AgendaSocket {
 public:
  AgendaSocket();
  void OperationLoop();

 private:
  /**
   * constructor
   */
  void startAgenda(void);

  /**
   * execute the operation
   * @return if the operationloop continue
   */
  bool executeOperation(const std::string t_operation, int& clnt_sock);

  /**
   * user Login
   */
  void userLogIn(int& clnt_sock);

  /**
   * user regist
   */
  void userRegister(int& clnt_sock);

  /**
   * quit the Agenda
   */
  void quitAgenda();

  /**
   * user logout
   */
  void userLogOut(int& clnt_sock);

  /**
   * delete a user from storage
   */
  void deleteUser(int& clnt_sock);

  /**
   * list all users from storage
   */
  void listAllUsers(int& clnt_sock);

  /**
   * user create a meeting with someone else
   */
  void createMeeting(int& clnt_sock);

  /**
   * list all meetings from storage
   */
  void listAllMeetings(int& clnt_sock);

  /**
   * list all meetings that this user sponsored
   */
  void listAllSponsorMeetings(int& clnt_sock);

  /**
   * list all meetings that this user take part in
   */
  void listAllParticipateMeetings(int& clnt_sock);

  /**
   * search meetings by title from storage
   */
  void queryMeetingByTitle(int& clnt_sock);

  /**
   * search meetings by timeinterval from storage
   */
  void queryMeetingByTimeInterval(int& clnt_sock);

  /**
   * delete meetings by title from storage
   */
  void deleteMeetingByTitle(int& clnt_sock);

  /**
   * delete all meetings that this user sponsored
   */
  void deleteAllMeetings(int& clnt_sock);

  /**
   * show the meetings in ss
   */
  void printMeetings(std::list<Meeting> t_meetings);
  // dates
  std::string m_userName;
  std::string m_userPassword;
  AgendaService m_agendaService;
};

#endif
