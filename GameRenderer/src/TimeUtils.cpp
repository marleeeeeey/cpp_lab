#include "TimeUtils.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

std::string TimeUtils::timeToStringHHMMSSMS(const TimeStamp& timestamp) {
  using namespace std::chrono;
  const std::time_t tt = system_clock::to_time_t(timestamp);

  std::tm tm{};
#if defined(_WIN32)
  localtime_s(&tm, &tt);
#else
  localtime_r(&tt, &tm);
#endif

  auto ms = duration_cast<milliseconds>(timestamp.time_since_epoch());
  auto ms_part = ms % 1000;

  std::ostringstream oss;
  oss << std::put_time(&tm, "%H:%M:%S") << "."
      << std::setfill('0') << std::setw(3) << ms_part.count();
  return oss.str();
}