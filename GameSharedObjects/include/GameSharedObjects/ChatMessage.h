#pragma once

#include "GameSharedObjects/Player.h"

struct ChatMessage {
  Player sender;
  std::string message;
};