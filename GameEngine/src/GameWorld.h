#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "GameInputManager/GameInputData.h"
#include "GameRenderer/IGameWorldRenderer.h"

// Business logic for the game world.
class GameWorld {
 public:
  GameWorld(std::weak_ptr<IGameWorldRenderer> gameWorldRenderer);
  void iterate(double elapsed, const GameInputData& userInputData);
  void onWindowSizeChanged(int width, int height);
  void setPlayerRandomPosition() const;

  using OnPlayerPositionChanged = std::function<void()>;
  OnPlayerPositionChanged onPlayerPositionChanged;

 private:
  void updateSnowflakesCount_(int width, int height);
  void impactOnSnowflakes_(double elapsed, const GameInputData& userInputData);
  void impactOnPlayer_(double elapsed, const GameInputData& userInputData);

  // -------------------------
  // Private state
  // -------------------------

  std::weak_ptr<IGameWorldRenderer> gameWorldRenderer_;
  std::vector<float> snowflakesSpeed_;
  int windowWidth_{};
  int windowHeight_{};
};
