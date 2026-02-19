#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "IGameWorldRenderer.h"
#include "UserInputData.h"

// Business logic for the game world.
class GameWorld {
  std::weak_ptr<IGameWorldRenderer> gameWorldRenderer_;
  std::vector<float> pointsSpeed_;
  glm::vec2 globalDirection_ = glm::normalize(glm::vec2(1.0f, 1.0f));
  float acceleration_ = 1.0f;
  int windowWidth_{};
  int windowHeight_{};

 public:
  void init(int width, int height, std::weak_ptr<IGameWorldRenderer> gameWorldRenderer);
  void iterate(double elapsed, const UserInputData& userInputData);
  void onWindowSizeChanged(int width, int height);

 private:
  void updateObjectsCount_(int width, int height);
};
