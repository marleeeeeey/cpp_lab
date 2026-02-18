#pragma once

#include <chrono>
#include <string>

class TimeUtils {
 public:
  using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;
  static std::string timeToStringHHMMSSMS(const TimeStamp& timestamp);
};
