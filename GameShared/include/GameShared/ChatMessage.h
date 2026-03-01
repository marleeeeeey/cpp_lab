#pragma once

#include "GameShared/Player.h"
#include "GlobalTypes/GlobalTypes.h"

struct ChatMessage {
  Player sender;
  std::string message;
  TimeStamp sentTimestamp;
  TimeStamp receivedTimestamp;
};