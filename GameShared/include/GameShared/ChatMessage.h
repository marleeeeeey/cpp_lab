#pragma once

#include "GameShared/Player.h"
#include "GameShared/TimeStamp.h"

struct ChatMessage {
  Player sender;
  std::string message;
  TimeStamp sentTimestamp;
  TimeStamp receivedTimestamp;
};