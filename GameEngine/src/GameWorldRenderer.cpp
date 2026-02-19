#include "GameWorldRenderer.h"

void GameWorldRenderer::init(SDL_Renderer* renderer) {
  renderer_ = renderer;
}

void GameWorldRenderer::render() {
  // Set brush color to white with full alpha
  SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);

  for (int i = 0; i < points.size(); i++) {
    const glm::vec2& point = points[i];
    SDL_RenderPoint(renderer_, point.x, point.y);
  }
}