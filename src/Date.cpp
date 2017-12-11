#include "Date.hpp"
#include <cstdio>
#include <regex>

Date::Date() : Date(0, 0, 0, 0, 0) {}

Date::Date(int t_year, int t_month, int t_day, int t_hour, int t_minute)
    : m_year(t_year),
      m_month(t_month),
      m_day(t_day),
      m_hour(t_hour),
      m_minute(t_minute) {}

Date::Date(std::string dateString) : Date(0, 0, 0, 0, 0) {
  Date temp = stringToDate(dateString);
  if (isValid(temp)) {
    (*this) = temp;
  }
}

int Date::getYear() const { return m_year; }
void Date::setYear(const int t_year) { m_year = t_year; }

int Date::getMonth() const { return m_month; }
void Date::setMonth(const int t_month) { m_month = t_month; }

int Date::getDay() const { return m_day; }
void Date::setDay(const int t_day) { m_day = t_day; }

int Date::getHour() const { return m_hour; }
void Date::setHour(const int t_hour) { m_hour = t_hour; }

int Date::getMinute() const { return m_minute; }
void Date::setMinute(const int t_minute) { m_minute = t_minute; }

bool Date::isValid(const Date t_date) {
  int year = t_date.getYear();
  if (year > 9999 || year < 1000) return false;

  // lambda function to judge the isRun
  auto isRun = [year]() -> bool {
    if (!(year % 100)) {
      if (!(year % 400)) {
        return true;
      } else {
        return false;
      }
    } else {
      if (!(year % 4)) {
        return true;
      } else {
        return false;
      }
    }
  };

  // v_date is an array to store the right day of each month.
  int v_date[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (isRun() && t_date.getMonth() == 2) {
    ++v_date[1];
  }
  if (t_date.getMonth() < 1 || t_date.getMonth() > 12) return false;
  if (t_date.getDay() > v_date[t_date.getMonth() - 1] || t_date.getDay() < 1)
    return false;
  if (t_date.getHour() < 0 || t_date.getHour() > 23) return false;
  if (t_date.getMinute() < 0 || t_date.getMinute() > 59) return false;
  return true;
}

Date Date::stringToDate(const std::string t_dateString) {
  auto isFormat = [](const std::string t_date) -> bool {
    const std::regex pattern("(\\d{4})-(\\d{2})-(\\d{2})/(\\d{2}):(\\d{2})");
    return std::regex_match(t_date, pattern);
  };
  if (isFormat(t_dateString)) {
    std::string year = t_dateString.substr(0, 4);
    std::string month = t_dateString.substr(5, 2);
    std::string day = t_dateString.substr(8, 2);
    std::string hour = t_dateString.substr(11, 2);
    std::string minute = t_dateString.substr(14, 2);
    if (month[0] == '0') month.erase(0, 1);
    if (day[0] == '0') day.erase(0, 1);
    if (hour[0] == '0') hour.erase(0, 1);
    if (minute[0] == '0') minute.erase(0, 1);
    return Date(std::stoi(year), std::stoi(month), std::stoi(day),
                std::stoi(hour), std::stoi(minute));
  } else {
    return Date(0, 0, 0, 0, 0);
  }
}

std::string Date::dateToString(Date t_date) {
  if (!isValid(t_date)) return std::string("0000-00-00/00:00");
  char buffer[50];
  sprintf(buffer, "%04d-%02d-%02d/%02d:%02d", t_date.getYear(),
          t_date.getMonth(), t_date.getDay(), t_date.getHour(),
          t_date.getMinute());
  return std::string(buffer);
}

Date& Date::operator=(const Date& t_date) {
  this->setYear(t_date.getYear());
  this->setMonth(t_date.getMonth());
  this->setDay(t_date.getDay());
  this->setHour(t_date.getHour());
  this->setMinute(t_date.getMinute());
  return (*this);
}

bool Date::operator==(const Date& t_date) const {
  return (m_year == t_date.getYear() && m_month == t_date.getMonth() &&
          m_day == t_date.getDay() && m_hour == t_date.getHour() &&
          m_minute == t_date.getMinute());
}

bool Date::operator>(const Date& t_date) const {
  if (m_year > t_date.getYear()) {
    return true;
  } else if (m_year < t_date.getYear()) {
    return false;
  } else {
    if (m_month > t_date.getMonth()) {
      return true;
    } else if (m_month < t_date.getMonth()) {
      return false;
    } else {
      if (m_day > t_date.getDay()) {
        return true;
      } else if (m_day < t_date.getDay()) {
        return false;
      } else {
        if (m_hour > t_date.getHour()) {
          return true;
        } else if (m_hour < t_date.getHour()) {
          return false;
        } else {
          if (m_minute > t_date.getMinute()) {
            return true;
          } else {
            return false;
          }
        }
      }
    }
  }
}

bool Date::operator<(const Date& t_date) const { return !((*this) >= t_date); }

bool Date::operator>=(const Date& t_date) const {
  return ((*this) == t_date) || ((*this) > t_date);
}

bool Date::operator<=(const Date& t_date) const {
  return ((*this) == t_date) || ((*this) < t_date);
}
