#pragma once

#include <chrono>
#include <string>

class TimeUtils {
 public:
  // ----------------
  // Signatures
  // ----------------
  using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;

  // ---------------
  // Interface
  // ---------------

  static std::string timeToStringHHMMSSMS(const TimeStamp& timestamp);
};
