#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "../../GameRenderer/include/GameRenderer/IGameWorldRenderer.h"
#include "UserInputData.h"

// Business logic for the game world.
class GameWorld {
 public:
  GameWorld(int width, int height, std::weak_ptr<IGameWorldRenderer> gameWorldRenderer);
  void iterate(double elapsed, const UserInputData& userInputData);
  void onWindowSizeChanged(int width, int height);
  void setPlayerRandomPosition() const;

  using OnPlayerPositionChanged = std::function<void()>;
  OnPlayerPositionChanged onPlayerPositionChanged;

 private:
  void updateSnowflakesCount_(int width, int height);
  void impactOnSnowflakes_(double elapsed, const UserInputData& userInputData);
  void impactOnPlayer_(double elapsed, const UserInputData& userInputData);

  // -------------------------
  // Private state
  // -------------------------

  std::weak_ptr<IGameWorldRenderer> gameWorldRenderer_;
  std::vector<float> snowflakesSpeed_;
  glm::vec2 globalDirection_ = glm::normalize(glm::vec2(1.0f, 1.0f));
  float acceleration_ = 1.0f;
  int windowWidth_{};
  int windowHeight_{};
};
