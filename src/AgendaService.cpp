#include "AgendaService.hpp"
#include <algorithm>

AgendaService::AgendaService() : m_storage(nullptr) { startAgenda(); }

AgendaService::~AgendaService() {}

bool AgendaService::userLogIn(const std::string userName,
                              const std::string password) {
  auto query = m_storage->queryUser([&](const User& user) -> bool {
    return user.getName() == userName && user.getPassword() == password;
  });
  if (query.empty()) {
    return false;
  } else {
    return true;
  }
}

bool AgendaService::userRegister(const std::string userName,
                                 const std::string password,
                                 const std::string email,
                                 const std::string phone) {
  auto query = m_storage->queryUser([&userName](const User& user) -> bool {
    return user.getName() == userName;
  });
  if (query.empty()) {
    m_storage->createUser(User(userName, password, email, phone));
    return true;
  } else {
    return false;
  }
}

bool AgendaService::deleteUser(const std::string userName,
                               const std::string password) {
  int change = m_storage->deleteUser([&](const User& user) -> bool {
    return user.getName() == userName && user.getPassword() == password;
  });
  if (change != 0) {
    this->deleteAllMeetings(userName);
    auto query = this->listAllParticipateMeetings(userName);
    for (const auto& meeting : query) {
      this->deleteMeeting(meeting.getSponsor(), meeting.getTitle());
    }
  }
  return change != 0;
}

std::list<User> AgendaService::listAllUsers() const {
  return m_storage->queryUser([](const User& user) -> bool { return true; });
}

bool AgendaService::createMeeting(const std::string userName,
                                  const std::string title,
                                  const std::string startDate,
                                  const std::string endDate,
                                  const std::vector<std::string> participator) {
  auto q_sponsor = m_storage->queryUser([&userName](const User& user) -> bool {
    return user.getName() == userName;
  });
  if (q_sponsor.empty()) return false;
  auto q_title =
      m_storage->queryMeeting([&title](const Meeting& meeting) -> bool {
        return meeting.getTitle() == title;
      });
  if (!q_title.empty()) return false;
  if (participator.empty()) return false;

  for (const auto& person : participator) {
    if (person == userName) {
      return false;
    }
  }

  // whether the participator is an user;
  auto q_user = m_storage->queryUser([&participator](const User& user) -> bool {
    auto it = std::find_if(participator.begin(), participator.end(),
                           [&user](const std::string& par_name) -> bool {
                             return par_name == user.getName();
                           });
    return it != participator.end();
  });
  if (q_user.size() != participator.size()) return false;

  // judge the Date
  Date start = Date::stringToDate(startDate);
  Date end = Date::stringToDate(endDate);
  if (!Date::isValid(start) || !Date::isValid(end)) return false;
  if (start >= end) return false;

  //  ( =)
  auto q_meeting = m_storage->queryMeeting([&start, &end](
                                               const Meeting& meeting) -> bool {
    // bool jStart =
    //     (start >= meeting.getStartDate() && start <
    //     meeting.getEndDate());
    // bool jEnd =
    //     (end > meeting.getStartDate() && end <= meeting.getEndDate());
    // bool jCover =
    //     (start <= meeting.getStartDate() && end >= meeting.getEndDate());
    // return jStart || jEnd || jCover;
    return !(start >= meeting.getEndDate() || end <= meeting.getStartDate());
  });
  if (!q_meeting.empty()) return false;

  // finally create the meeting:
  m_storage->createMeeting(Meeting(userName, participator, start, end, title));
  return true;
}

std::list<Meeting> AgendaService::meetingQuery(const std::string userName,
                                               const std::string title) const {
  std::list<Meeting> result;
  auto q_sponsor = m_storage->queryUser([&userName](const User& user) -> bool {
    return user.getName() == userName;
  });
  if (q_sponsor.empty()) return result;
  return m_storage->queryMeeting([&](const Meeting& meeting) -> bool {
    return meeting.getSponsor() == userName && meeting.getTitle() == title;
  });
}

/**
* search a meeting by username, time interval
* @param uesrName the sponsor's userName
* @param startDate time interval's start date
* @param endDate time interval's end date
* @return a meeting list result
*/
//
std::list<Meeting> AgendaService::meetingQuery(
    const std::string userName, const std::string startDate,
    const std::string endDate) const {
  std::list<Meeting> result;
  auto q_sponsor = m_storage->queryUser([&userName](const User& user) -> bool {
    return user.getName() == userName;
  });
  if (q_sponsor.empty()) return result;

  return m_storage->queryMeeting([&](const Meeting& meeting) -> bool {
    Date start = Date::stringToDate(startDate);
    Date end = Date::stringToDate(endDate);
    if (start > end) return false;
    return !(start > meeting.getEndDate() || end < meeting.getStartDate());
  });
}

std::list<Meeting> AgendaService::listAllMeetings(
    const std::string userName) const {
  std::list<Meeting> result;
  auto q_sponsor = m_storage->queryUser([&userName](const User& user) -> bool {
    return user.getName() == userName;
  });
  if (q_sponsor.empty()) return result;
  return m_storage->queryMeeting([&userName](const Meeting& meeting) -> bool {
    return meeting.isParticipator(userName) || meeting.getSponsor() == userName;
  });
}

std::list<Meeting> AgendaService::listAllSponsorMeetings(
    const std::string userName) const {
  std::list<Meeting> result;
  auto q_sponsor = m_storage->queryUser([&userName](const User& user) -> bool {
    return user.getName() == userName;
  });
  if (q_sponsor.empty()) return result;
  return m_storage->queryMeeting([&userName](const Meeting& meeting) -> bool {
    return meeting.getSponsor() == userName;
  });
}

std::list<Meeting> AgendaService::listAllParticipateMeetings(
    const std::string userName) const {
  std::list<Meeting> result;
  auto q_sponsor = m_storage->queryUser([&userName](const User& user) -> bool {
    return user.getName() == userName;
  });
  if (q_sponsor.empty()) return result;
  return m_storage->queryMeeting([&userName](const Meeting& meeting) -> bool {
    return meeting.isParticipator(userName);
  });
}

bool AgendaService::deleteMeeting(const std::string userName,
                                  const std::string title) {
  auto q_sponsor = m_storage->queryUser([&userName](const User& user) -> bool {
    return user.getName() == userName;
  });
  if (q_sponsor.empty()) return false;
  int change = m_storage->deleteMeeting([&](const Meeting& meeting) -> bool {
    return meeting.getSponsor() == userName && meeting.getTitle() == title;
  });
  return change != 0;
}

bool AgendaService::deleteAllMeetings(const std::string userName) {
  auto q_sponsor = m_storage->queryUser([&userName](const User& user) -> bool {
    return user.getName() == userName;
  });
  if (q_sponsor.empty()) return false;
  int change = m_storage->deleteMeeting([&](const Meeting& meeting) -> bool {
    return meeting.getSponsor() == userName;
  });
  return change != 0;
}

void AgendaService::startAgenda() { m_storage = Storage::getInstance(); }
void AgendaService::quitAgenda() {}
