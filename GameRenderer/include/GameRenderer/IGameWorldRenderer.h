#pragma once

#include <deque>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "GameShared/Player.h"
#include "GameShared/WorldSnapshot.h"
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

  // TODO: covert this fields to methods?
  std::vector<glm::vec2> snowflakes;
  PlayerId myPlayerId;
  WorldSnapshot interpolatedWorldSnapshot;
};