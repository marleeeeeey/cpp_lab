#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "GameSharedObjects/Player.h"
#include "IRenderer.h"

// Interface for rendering the game world.
// Contains only data needed for rendering.
// This data may not be duplicated in GameWorld.
class IGameWorldRenderer : public IRenderer {
 public:
  // -------------
  // Factory
  // -------------

  static std::unique_ptr<IGameWorldRenderer> create(void* renderer);
  virtual ~IGameWorldRenderer() override = default;

  // -------------
  // Interface
  // -------------

  std::vector<glm::vec2> snowflakes;
  Player myPlayer;
  std::unordered_map<std::string, Player> otherPlayers;
};