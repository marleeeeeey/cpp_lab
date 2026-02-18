#include "ChatDataForRendering.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace {

std::string makeTimePrefixHHMMSS() {
  using namespace std::chrono;

  const auto now = system_clock::now();
  const std::time_t tt = system_clock::to_time_t(now);

  std::tm tm{};
#if defined(_WIN32)
  localtime_s(&tm, &tt);
#else
  localtime_r(&tt, &tm);
#endif

  std::ostringstream oss;
  oss << '[' << std::put_time(&tm, "%H:%M:%S") << "] ";
  return oss.str();
}
}  // namespace

void ChatDataForRendering::addMessage(const ChatMessage& message) {
  chatHistory_.push_back(message);  // TODO return back makeTimePrefixHHMMSS
}

const std::vector<ChatMessage>& ChatDataForRendering::getChatHistory() const {
  return chatHistory_;
}