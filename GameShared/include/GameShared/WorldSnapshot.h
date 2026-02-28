#pragma once

#include <cstdint>
#include <vector>

#include "Player.h"

struct PlayerSnapshot {
  PlayerId id;
  glm::vec2 position;
};

struct WorldSnapshot {
  uint32_t serverTick;
  std::vector<PlayerSnapshot> players;
};
