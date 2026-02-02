#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "GameDataForRendering.h"
#include "UserInputData.h"

// Business logic for the game world.
class GameWorld {
  GameDataForRendering gameDataForRendering_;
  std::vector<float> pointsSpeed_;
  glm::vec2 globalDirection_ = glm::normalize(glm::vec2(1.0f, 1.0f));
  float acceleration_ = 1.0f;

 public:
  void init();
  void iterate(double elapsed, const UserInputData& userInputData);
  const GameDataForRendering& getGameDataForRendering() const;
};
