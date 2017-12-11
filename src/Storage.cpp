#define DEBUG
#undef DEBUG
#include "Storage.hpp"
#include <algorithm>
#include <fstream>
#include <vector>
#include "Path.hpp"

#ifdef DEBUG
#include <iostream>
#endif

Storage::Storage() {}

bool Storage::readFromFile() {
  // read the users.csv
  std::ifstream in(Path::userPath);
  if (in.fail()) {
#ifdef DEBUG
    std::cout << "open the user.csv file failed." << std::endl;
#endif
    return false;
  }
  std::string line;
  while (getline(in, line)) {
    std::vector<std::string> person_information;
    std::vector<unsigned int> quote_index;
    for (unsigned int i = 0; i < line.size(); ++i) {
      if (line[i] == '\"') {
        quote_index.push_back(i);
      }
    }
    for (auto iter = quote_index.begin(); iter != quote_index.end(); ++iter) {
      unsigned int begin = *iter;
      unsigned int end = *(++iter);
      person_information.push_back(line.substr(begin + 1, end - begin - 1));
    }
    m_userList.push_back(User(person_information[0], person_information[1],
                              person_information[2], person_information[3]));
  }
  in.close();

  // read the meetings.csv
  in.open(Path::meetingPath);
  if (in.fail()) {
#ifdef DEBUG
    std::cout << "open the meetings.csv file failed." << std::endl;
#endif
    return false;
  }

  while (getline(in, line)) {
    std::vector<unsigned int> quote_index;
    for (unsigned int i = 0; i < line.size(); ++i) {
      if (line[i] == '\"') {
        quote_index.push_back(i);
      }
    }

    std::vector<std::string> information;
    for (auto iter = quote_index.begin(); iter != quote_index.end(); ++iter) {
      unsigned int begin = *iter;
      unsigned int end = *(++iter);
      information.push_back(line.substr(begin + 1, end - begin - 1));
    }

    std::vector<std::string> participator;
    auto lhs_it = information[1].begin();
    for (auto rhs_it = information[1].begin(); rhs_it != information[1].end();
         ++rhs_it) {
      if (*rhs_it == '&') {
        participator.push_back(std::string(lhs_it, rhs_it));
        lhs_it = rhs_it + 1;
      }
    }
    participator.push_back(std::string(lhs_it, information[1].end()));
#ifdef DEBUG
    std::cout << participator.empty() << std::endl;
#endif
    m_meetingList.push_back(Meeting(
        information[0], participator, Date::stringToDate(information[2]),
        Date::stringToDate(information[3]), information[4]));
  }
  in.close();
  return true;
}

bool Storage::writeToFile() {
  std::ofstream out(Path::userPath);
  if (out.fail()) {
#ifdef DEBUG
    std::cout << "open the user.csv file failed." << std::endl;
#endif
    return false;
  }
  // for_each(m_userList.begin(), m_userList.end(), [&out](const User &person) {
  //   out << "\"" << person.getName() << "\",\"" << person.getPassword()
  //       << "\",\"" << person.getEmail() << "\",\"" << person.getPhone()
  //       << "\"\n";
  // });
  for (auto it = m_userList.begin(); it != m_userList.end(); ++it) {
    out << "\"" << it->getName() << "\",\"" << it->getPassword() << "\",\""
        << it->getEmail() << "\",\"" << it->getPhone() << "\"\n";
  }
  out.close();

  // sync the meetings.csv
  out.open(Path::meetingPath);
  if (out.fail()) {
#ifdef DEBUG
    std::cout << "open the meetings.csv failed." << std::endl;
#endif
    return false;
  }
  for_each(m_meetingList.begin(), m_meetingList.end(),
           [&out](const Meeting &per) {
             out << "\"" << per.getSponsor() << "\",\"";

             const auto &participators = per.getParticipator();
             for (const auto &person : participators) {
               out << person;
               if (person != participators.back()) out << "&";
             }

             out << "\",\"" << Date::dateToString(per.getStartDate()) << "\",";
             out << "\"" << Date::dateToString(per.getEndDate()) << "\",";
             out << "\"" << per.getTitle() << "\"\n";
           });
  out.close();
  m_dirty = false;
  return true;
}

std::shared_ptr<Storage> Storage::m_instance(nullptr);

std::shared_ptr<Storage> Storage::getInstance() {
  if (m_instance == nullptr) {
    m_instance.reset(new Storage());
    m_instance->readFromFile();
    m_instance->m_dirty = false;
  }
  return m_instance;
}

Storage::~Storage() { writeToFile(); }

void Storage::createUser(const User &t_user) {
  m_userList.push_back(t_user);
  m_dirty = true;
}

std::list<User> Storage::queryUser(
    std::function<bool(const User &)> filter) const {
  std::list<User> result;
  for (const auto &person : m_userList) {
    if (filter(person)) result.push_back(person);
  }
  return result;
}

int Storage::updateUser(std::function<bool(const User &)> filter,
                        std::function<void(User &)> switcher) {
  int count = 0;
  for (auto &person : m_userList) {
    if (filter(person)) {
      ++count;
      switcher(person);
    }
  }
  if (count != 0) m_dirty = true;
  return count;
}

int Storage::deleteUser(std::function<bool(const User &)> filter) {
  // int begin = m_userList.size();
  // m_userList.remove_if(filter);
  // int end = m_userList.size();
  // if (begin - end) m_dirty = true;
  // return begin - end;
  // std::vector<decltype(m_userList.begin())> v;
  std::vector<std::list<User>::iterator> v;
  for (auto it = m_userList.begin(); it != m_userList.end(); ++it) {
    if (filter(*it)) v.push_back(it);
  }
  for (auto it = v.begin(); it != v.end(); ++it) {
    m_userList.erase(*it);
  }
  if (!v.empty()) m_dirty = true;
  return v.size();
}

void Storage::createMeeting(const Meeting &t_meeting) {
  m_meetingList.push_back(t_meeting);
  m_dirty = true;
}

std::list<Meeting> Storage::queryMeeting(
    std::function<bool(const Meeting &)> filter) const {
  std::list<Meeting> result;
  for (const auto &meeting : m_meetingList) {
    if (filter(meeting)) {
      result.push_back(meeting);
    }
  }
  return result;
}

int Storage::updateMeeting(std::function<bool(const Meeting &)> filter,
                           std::function<void(Meeting &)> switcher) {
  int count = 0;
  for (auto &meeting : m_meetingList) {
    if (filter(meeting)) {
      ++count;
      switcher(meeting);
    }
  }
  if (count != 0) m_dirty = true;
  return count;
}

int Storage::deleteMeeting(std::function<bool(const Meeting &)> filter) {
  // int begin = m_meetingList.size();
  // m_meetingList.remove_if(filter);
  // int end = m_meetingList.size();
  // if (begin - end) m_dirty = true;
  // return begin - end;
  std::vector<decltype(m_meetingList.begin())> v;
  for (auto it = m_meetingList.begin(); it != m_meetingList.end(); ++it) {
    if (filter(*it)) v.push_back(it);
  }
  for (auto it = v.begin(); it != v.end(); ++it) {
    m_meetingList.erase(*it);
  }
  if (!v.empty()) m_dirty = true;
  return v.size();
}

bool Storage::sync() {
  if (m_dirty) {
#ifdef DEBUG
    std::cout << "Begin to Sync !" << std::endl;
#endif
    return this->writeToFile();
  } else {
#ifdef DEBUG
    std::cout << "Don't need to sync !" << std::endl;
#endif
    return true;
  }
}
