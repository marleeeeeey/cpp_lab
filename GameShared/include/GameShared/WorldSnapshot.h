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
  // not possible to use std::unordered_map because of using Flatbuffers serialization
  std::vector<PlayerSnapshot> players;
};
