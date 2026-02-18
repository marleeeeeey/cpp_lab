#pragma once

#include "GameSharedObjects/Player.h"

// User data per socket
struct PerSocketData {
  Player player;
  std::string clientIp;
};
