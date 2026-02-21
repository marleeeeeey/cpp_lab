#include "GameWorldRenderer.h"

#include <ranges>

GameWorldRenderer::GameWorldRenderer(SDL_Renderer* renderer) {
  renderer_ = renderer;
}

void GameWorldRenderer::render() {
  // Set brush color to white with full alpha
  SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);

  for (const glm::vec2& point : snowflakes) {
    SDL_RenderPoint(renderer_, point.x, point.y);
  }

  // Draw Opponent Players with BLUE color
  for (const auto& opponent : otherPlayers | std::views::values) {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_FRect rect = {opponent.position.x, opponent.position.y, 10, 10};
    SDL_RenderFillRect(renderer_, &rect);
  }

  // Draw My Player with RED color over Oponents
  SDL_SetRenderDrawColor(renderer_, 255, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_FRect rect = {myPlayer.position.x, myPlayer.position.y, 10, 10};
  SDL_RenderFillRect(renderer_, &rect);
}