#pragma once

#include <deque>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "GameRenderer/DataForRenderer.h"
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
  // TODO: covert these fields to methods?
  // -------------

  std::vector<glm::vec2> snowflakes;
  virtual void setDataForRenderer(std::shared_ptr<DataForRenderer> dataForRenderer) = 0;

  // For debug purposes
  virtual void debugServerPositionForLocalPlayer(bool enabled) = 0;
};