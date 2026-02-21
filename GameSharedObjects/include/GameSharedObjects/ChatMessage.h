#pragma once

#include "GameSharedObjects/Player.h"
#include "GameSharedObjects/TimeStamp.h"

struct ChatMessage {
  Player sender;
  std::string message;
  TimeStamp sentTimestamp;
  TimeStamp receivedTimestamp;
};