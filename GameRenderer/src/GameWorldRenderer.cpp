#include "GameWorldRenderer.h"

#include <ranges>

GameWorldRenderer::GameWorldRenderer(SDL_Renderer* renderer) {
  renderer_ = renderer;
}

void GameWorldRenderer::render() {
  if (!isVisible_) return;

  // ---------------------------
  // Draw snowflakes - White
  // ---------------------------

  SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);
  for (const glm::vec2& point : snowflakes) {
    SDL_RenderPoint(renderer_, point.x, point.y);
  }

  // -------------------------
  // Draw opponents first
  // -------------------------

  for (PlayerSnapshot& playerSnapshot : worldSnapshot.players) {
    if (playerSnapshot.id == myPlayerId) continue;

    SDL_SetRenderDrawColor(renderer_, 0, 0, 255, SDL_ALPHA_OPAQUE);  // BLUE - Opponents
    SDL_FRect rect = {playerSnapshot.position.x, playerSnapshot.position.y, 10, 10};
    SDL_RenderFillRect(renderer_, &rect);
  }

  // --------------------------------------
  // Draw my player above all opponents
  // --------------------------------------

  for (PlayerSnapshot& playerSnapshot : worldSnapshot.players) {
    if (playerSnapshot.id != myPlayerId) continue;

    SDL_SetRenderDrawColor(renderer_, 255, 0, 0, SDL_ALPHA_OPAQUE);  // RED - My Player
    SDL_FRect rect = {playerSnapshot.position.x, playerSnapshot.position.y, 10, 10};
    SDL_RenderFillRect(renderer_, &rect);
  }
}
