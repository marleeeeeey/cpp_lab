#include "TimeUtils.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

std::string TimeUtils::makeTimePrefixHHMMSS(const TimeStamp& timestamp) {
  using namespace std::chrono;
  const std::time_t tt = system_clock::to_time_t(timestamp);

  std::tm tm{};
#if defined(_WIN32)
  localtime_s(&tm, &tt);
#else
  localtime_r(&tt, &tm);
#endif

  std::ostringstream oss;
  oss << std::put_time(&tm, "%H:%M:%S");
  return oss.str();
}