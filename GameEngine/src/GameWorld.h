#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "GameRenderer/IGameWorldRenderer.h"
#include "UserInputManager/UserInputData.h"

// Business logic for the game world.
class GameWorld {
 public:
  GameWorld(std::weak_ptr<IGameWorldRenderer> gameWorldRenderer);
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
  int windowWidth_{};
  int windowHeight_{};
  glm::vec2 playerVelocity_{0.0f, 0.0f};  // px/s
};
