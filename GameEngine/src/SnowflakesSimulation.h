#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "GameRenderer/IGameWorldRenderer.h"
#include "GameShared/WorldSnapshot.h"
#include "UserInputManager/UserInputData.h"

// Business logic for the game world.
class SnowflakesSimulation {
 public:
  SnowflakesSimulation(std::weak_ptr<IGameWorldRenderer> gameWorldRenderer);
  void iterate(double elapsed, const UserInputData& userInputData);
  void onWindowSizeChanged(int width, int height);

 private:
  void updateSnowflakesCount_(int width, int height);
  void impactOnSnowflakes_(double elapsed, const UserInputData& userInputData);

  // -------------------------
  // Private state
  // -------------------------

  std::weak_ptr<IGameWorldRenderer> gameWorldRenderer_;
  std::vector<float> snowflakesSpeed_;
  int windowWidth_{};
  int windowHeight_{};
};
