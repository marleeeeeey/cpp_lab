#pragma once

#include <memory>
#include <vector>

#include "GameRenderer/IGameWorldRenderer.h"

// Business logic for the game world.
class SnowflakesSimulation {
 public:
  SnowflakesSimulation(std::weak_ptr<IGameWorldRenderer> gameWorldRenderer);
  void iterate(double elapsed);
  void onWindowSizeChanged(int width, int height);

 private:
  void updateSnowflakesCount_(int width, int height);
  void impactOnSnowflakes_(double elapsed);

  // -------------------------
  // Private state
  // -------------------------

  std::weak_ptr<IGameWorldRenderer> gameWorldRenderer_;
  std::vector<float> snowflakesSpeed_;
  int windowWidth_{};
  int windowHeight_{};
};
