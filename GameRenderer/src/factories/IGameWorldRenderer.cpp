#include "GameRenderer/IGameWorldRenderer.h"

#include <SDL3/SDL.h>

#include "GameWorldRenderer.h"

std::unique_ptr<IGameWorldRenderer> IGameWorldRenderer::create(void* renderer) {
  auto sdlRenderer = static_cast<SDL_Renderer*>(renderer);
  return std::make_unique<GameWorldRenderer>(sdlRenderer);
}