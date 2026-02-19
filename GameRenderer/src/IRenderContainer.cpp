#include "GameRenderer/IRenderContainer.h"

#include "RenderContainer.h"

std::unique_ptr<IRenderContainer> IRenderContainer::create(void* renderer) {
  auto sdlRenderer = static_cast<SDL_Renderer*>(renderer);
  return std::make_unique<RenderContainer>(sdlRenderer);
}