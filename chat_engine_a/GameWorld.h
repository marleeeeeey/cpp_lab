#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "GameDataForRendering.h"
#include "UserInputData.h"

// Business logic for the game world.
class GameWorld {
  GameDataForRendering gameDataForRendering;
  std::vector<float> point_speeds;
  glm::vec2 global_direction = glm::normalize(glm::vec2(1.0f, 1.0f));
  float acceleration = 1.0f;

 public:
  void init();
  void iterate(double elapsed, const UserInputData& userInputData);
  const GameDataForRendering& getGameDataForRendering() const;
};
