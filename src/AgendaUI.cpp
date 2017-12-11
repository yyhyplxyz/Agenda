#include "AgendaUI.hpp"
#include <vector>

inline void date_attention() {
  std::cout << "Attention: the format must be \"0000-00-00/00:00\""
            << std::endl;
}

inline void newline() { std::cout << std::endl; }
inline void splitline() {
  std::cout << "-------------------------------------" << std::endl;
}
inline void newcommand() { std::cout << "Agenda:~$"; }

void welcome() {
  std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-Welcome to The Agenda!"
               "!-=-=-=-=-=-=-=-=-=-=-=-="
            << std::endl;
  std::cout << "You can use \'help\' option to get the help file!" << std::endl;
}

void help(const std::string judge) {
  system("clear");
  std::cout
      << "====================Here is the help file ======================="
      << std::endl;
  newline();
  if (judge.empty()) {
    std::cout << "You still not log in!" << std::endl;
    newline();
    std::cout << "help -> print the help file" << std::endl;
    std::cout << "re -> register a new account" << std::endl;
    std::cout << "login -> log in your account" << std::endl;
    std::cout << "clear -> clear the screen" << std::endl;
    std::cout << "q -> quit the Agenda" << std::endl;
  } else {
    std::cout << "Be careful not to input wrong command :)" << std::endl;
    newline();
    std::cout << "help -> print the help file" << std::endl;
    std::cout << "logout -> log out your account" << std::endl;
    std::cout << "q -> quit the Agenda" << std::endl;
    std::cout << "du -> delete your own account" << std::endl;
    std::cout << "lu -> list all users from storage" << std::endl;
    std::cout << "cm -> create a meeting with someone else" << std::endl;
    std::cout << "lam -> list all meetings from storage" << std::endl;
    std::cout << "lsm -> list all meetings that this user sponsored"
              << std::endl;
    std::cout << "lpm -> list all meetings that this user take part in"
              << std::endl;
    std::cout << "qbtitle -> search meetings bu title from storage"
              << std::endl;
    std::cout << "qbtime -> search meetings by timeinterval from sotrage"
              << std::endl;
    std::cout << "dbtitle -> delete meetings by title from storage"
              << std::endl;
    std::cout << "dall -> delete all meetings that this user sponsored"
              << std::endl;
  }
}

AgendaUI::AgendaUI() {}

bool is_continue = true;
void AgendaUI::OperationLoop() {
  startAgenda();
  system("clear");
  welcome();
  while (is_continue) {
    std::string operation = getOperation();
    executeOperation(operation);
  }
  quitAgenda();
}

void AgendaUI::startAgenda() { m_agendaService.startAgenda(); }

void AgendaUI::quitAgenda() {
  std::cout << "Quit the Agenda!" << std::endl;
  m_agendaService.quitAgenda();
}

std::string AgendaUI::getOperation() {
  newcommand();
  std::string operation;
  std::cin >> operation;
  return operation;
}

bool AgendaUI::executeOperation(std::string t_operation) {
  if (t_operation == "help") {
    help(m_userName);
  } else if (t_operation == "re") {
    userRegister();
  } else if (t_operation == "login") {
    userLogIn();
  } else if (t_operation == "q") {
    is_continue = false;
  } else if (t_operation == "logout") {
    userLogOut();
  } else if (t_operation == "du") {
    deleteUser();
  } else if (t_operation == "lu") {
    listAllUsers();
  } else if (t_operation == "cm") {
    createMeeting();
  } else if (t_operation == "lam") {
    listAllMeetings();
  } else if (t_operation == "lsm") {
    listAllSponsorMeetings();
  } else if (t_operation == "lpm") {
    listAllParticipateMeetings();
  } else if (t_operation == "qbtitle") {
    queryMeetingByTitle();
  } else if (t_operation == "qbtime") {
    queryMeetingByTimeInterval();
  } else if (t_operation == "dbtitle") {
    deleteMeetingByTitle();
  } else if (t_operation == "dall") {
    deleteAllMeetings();
  } else if (t_operation == "clear") {
    system("clear");
  }
  return true;
}

void AgendaUI::userLogIn() {
  std::cout << "LogIn : Please input your userName and password :)"
            << std::endl;
  std::string userName, password;
  std::cout << "Username:";
  std::cin >> userName;
  std::cout << "Password:";
  std::cin >> password;
  if (m_agendaService.userLogIn(userName, password)) {
    m_userName = userName;
    m_userPassword = password;
    splitline();
    std::cout << "Welcome , " << userName << std::endl;
    splitline();
  } else {
    std::cout << "Login Failed : no such a userName or wrong password."
              << std::endl;
  }
}

void AgendaUI::userRegister() {
  std::cout << "Register a new Account:" << std::endl;
  std::string userName, password, email, phone;
  newline();
  std::cout << "New userName : ";
  std::cin >> userName;
  std::cout << "password : ";
  std::cin >> password;
  std::cout << "email : ";
  std::cin >> email;
  std::cout << "phone : ";
  std::cin >> phone;
  if (m_agendaService.userRegister(userName, password, email, phone)) {
    std::cout << "Register success ! " << std::endl;
  } else {
    std::cout << "Failed : the userName has been used." << std::endl;
  }
}

void AgendaUI::userLogOut() {
  std::cout << "Log out successfully" << std::endl;
  m_userName = "";
  m_userPassword = "";
  welcome();
}

void AgendaUI::deleteUser() {
  std::cout << "Please input your password." << std::endl;
  std::string password;
  std::cout << "Password : " << std::endl;
  std::cin >> password;
  if (m_agendaService.deleteUser(m_userName, password)) {
    std::cout << "Delete Successfully!" << std::endl;
    userLogOut();
  } else {
    std::cout << "Delete Failed!" << std::endl;
  }
}

void AgendaUI::listAllUsers() {
  std::cout << "All users in the Storage:" << std::endl;
  splitline();
  auto result = m_agendaService.listAllUsers();
  for (const auto& user : result) {
    std::cout << user.getName() << std::endl;
    std::cout << user.getEmail() << std::endl;
    std::cout << user.getPhone() << std::endl;
    splitline();
  }
}

void AgendaUI::createMeeting() {
  std::string title, startDate, endDate;
  std::cout << "Please input the title :" << std::endl;
  std::cin >> title;
  std::cout << "Please input the startDate :" << std::endl;
  date_attention();
  std::cin >> startDate;
  std::cout << "Please input the endDate :" << std::endl;
  date_attention();
  std::cin >> endDate;
  std::vector<std::string> participator;
  std::cout << "Please input the number of paticipatior" << std::endl;
  int count;
  std::cin >> count;
  while (count--) {
    std::cout << "Participator :";
    std::string person;
    std::cin >> person;
    participator.push_back(person);
  }
  if (m_agendaService.createMeeting(m_userName, title, startDate, endDate,
                                    participator)) {
    std::cout << "create meeting successfully!" << std::endl;
  } else {
    std::cout << "creating failed !" << std::endl;
  }
}

void AgendaUI::listAllMeetings() {
  std::cout << "list all meetings :" << std::endl;
  splitline();
  printMeetings(m_agendaService.listAllMeetings(m_userName));
}

void AgendaUI::listAllSponsorMeetings() {
  std::cout << "list all sponsor meetings" << std::endl;
  splitline();
  printMeetings(m_agendaService.listAllSponsorMeetings(m_userName));
}

void AgendaUI::listAllParticipateMeetings() {
  std::cout << "list all participate meetings" << std::endl;
  splitline();
  printMeetings(m_agendaService.listAllParticipateMeetings(m_userName));
}

void AgendaUI::queryMeetingByTitle() {
  std::cout << "Please input the title:" << std::endl;
  std::string title;
  std::cin >> title;
  std::cout << "query meeting by title:" << std::endl;
  newline();
  printMeetings(m_agendaService.meetingQuery(m_userName, title));
}

void AgendaUI::queryMeetingByTimeInterval() {
  std::cout << "Please input the startDate and the endDate." << std::endl;
  date_attention();
  std::string startDate, endDate;
  std::cout << "startDate :" << std::endl;
  std::cin >> startDate;
  std::cout << "endDate :" << std::endl;
  std::cin >> endDate;
  std::cout << "query meeting by time interval:" << std::endl;
  newline();
  printMeetings(m_agendaService.meetingQuery(m_userName, startDate, endDate));
}

void AgendaUI::deleteMeetingByTitle() {
  std::cout << "Please input the title:" << std::endl;
  std::string title;
  std::cin >> title;
  if (m_agendaService.deleteMeeting(m_userName, title)) {
    std::cout << "success" << std::endl;
  } else {
    std::cout << "failed" << std::endl;
  }
}

void AgendaUI::deleteAllMeetings() {
  std::cout << "Are you sure to delete all meetings your sponsor ? y//n"
            << std::endl;
  char choice;
  std::cin >> choice;
  if (choice == 'y') {
    if (m_agendaService.deleteAllMeetings(m_userName)) {
      std::cout << "success" << std::endl;
    } else {
      std::cout << "failed" << std::endl;
    }
  } else {
    return;
  }
}

void AgendaUI::printMeetings(std::list<Meeting> t_meetings) {
  splitline();
  for (const auto& meeting : t_meetings) {
    std::cout << "Meeting Title: " << meeting.getTitle() << std::endl;
    std::cout << "Meeting Sponsor: " << meeting.getSponsor() << std::endl;
    std::cout << "Meeting participators:";
    for (const auto& par_name : meeting.getParticipator()) {
      std::cout << par_name << " ";
    }
    std::cout << "\nMeeting startDate: "
              << Date::dateToString(meeting.getStartDate()) << std::endl;
    std::cout << "Meeting endDate: " << Date::dateToString(meeting.getEndDate())
              << std::endl;
    splitline();
  }
}
