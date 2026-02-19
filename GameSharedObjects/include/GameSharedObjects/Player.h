#pragma once

#include <glm/glm.hpp>
#include <string>

struct Player {
  std::string name;
  int messagesSent{0};
  glm::vec2 position;
};