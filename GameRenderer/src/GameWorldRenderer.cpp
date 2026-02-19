#include "GameWorldRenderer.h"

GameWorldRenderer::GameWorldRenderer(SDL_Renderer* renderer) {
  renderer_ = renderer;
}

void GameWorldRenderer::render() {
  // Set brush color to white with full alpha
  SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);

  for (const glm::vec2& point : points) {
    SDL_RenderPoint(renderer_, point.x, point.y);
  }
}