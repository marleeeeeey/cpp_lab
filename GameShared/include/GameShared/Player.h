#pragma once

#include <glm/glm.hpp>
#include <string>

struct PlayerState {
  glm::vec2 position{0};
  glm::vec2 velocity{0};
};

using PlayerId = uint32_t;

struct Player {
  PlayerId id;
  std::string name;
  int messagesSent{0};
  PlayerState state;
  glm::vec2 lastInput{0};
};
