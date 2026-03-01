#pragma once

#include <chrono>
#include <string>

#include "GlobalTypes/GlobalTypes.h"

class TimeUtils {
 public:

  // ---------------
  // Interface
  // ---------------

  static std::string timeToStringHHMMSSMS(const TimeStamp& timestamp);
};
