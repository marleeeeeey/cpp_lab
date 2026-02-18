#pragma once

#include <chrono>

#include "GameSharedObjects/Player.h"

using TimeStamp = std::chrono::time_point<std::chrono::system_clock>;

struct ChatMessage {
  Player sender;
  std::string message;
  TimeStamp sentTimestamp;
  TimeStamp receivedTimestamp;
};