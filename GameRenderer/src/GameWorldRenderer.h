#pragma once

#include <SDL3/SDL.h>

#include "GameRenderer/IGameWorldRenderer.h"

class GameWorldRenderer : public IGameWorldRenderer {
 public:
  explicit GameWorldRenderer(SDL_Renderer* renderer);
  void render() override;

 private:
  SDL_Renderer* renderer_ = nullptr;
};
