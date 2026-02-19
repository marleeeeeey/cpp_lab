#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "IRenderer.h"

struct SDL_Renderer;

class IGameWorldRenderer : public IRenderer {
 public:
  virtual ~IGameWorldRenderer() = default;

  std::vector<glm::vec2> points;
};