#include "GameWorldRenderer.h"

#include <GameUtils/GeomUtils.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <ranges>

#include "GameShared/Simulation.h"

GameWorldRenderer::GameWorldRenderer(SDL_Renderer* renderer) {
  renderer_ = renderer;
}

void GameWorldRenderer::render(float dt, float gameTime) {
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

  if (!dataForRenderer_) return;
  auto& snapshot = dataForRenderer_->snapshot;

  PlayerSnapshot* localPlayerSnapshotFromServer = nullptr;

  for (PlayerSnapshot& playerSnapshot : snapshot.players) {
    if (dataForRenderer_->localPlayer && playerSnapshot.id == dataForRenderer_->localPlayer->id) {
      localPlayerSnapshotFromServer = &playerSnapshot;
      continue;
    }

    SDL_SetRenderDrawColor(renderer_, 0, 0, 255, SDL_ALPHA_OPAQUE);  // BLUE - Opponents
    SDL_FRect rect = {playerSnapshot.position.x, playerSnapshot.position.y, 10, 10};
    SDL_RenderFillRect(renderer_, &rect);
  }

  // -----------------------------------------------------------
  // Interpolate My Player Position closer to Server Position
  // -----------------------------------------------------------

  if (!dataForRenderer_->localPlayer) return;
  if (!localPlayerSnapshotFromServer) return;

  auto& localPlayer = dataForRenderer_->localPlayer;
  auto& localPlayerState = localPlayer->state;

  auto& localPlayerPositionFromServer = localPlayerSnapshotFromServer->position;
  PlayerState localPlayerStateFromServer = {
      .position = localPlayerPositionFromServer,
      .velocity = glm::vec2(0.0f),
  };

  auto oldPosition = localPlayerState.position;
  localPlayerState = interpolatePlayerPosition(localPlayerState, localPlayerStateFromServer, dt * 2);
  auto newPosition = localPlayerState.position;

  auto interpolatedDistance = glm::distance(oldPosition, newPosition);
  safeStaticDebug("Interpolated Distance", std::format("Interpolated Distance Px: {:.2f}", interpolatedDistance));

  // -------------------------------------------------
  // Calculate and Display Server-Client Discrepancy
  // -------------------------------------------------

  auto discrepancy = glm::distance(localPlayerState.position, localPlayerStateFromServer.position);
  if (discrepancy < 1.0f) discrepancy = 0.0f;
  safeStaticDebug("Server-Client Discrepancy", std::format("Server-Client Discrepancy Px: {:.2f}", discrepancy));

  // --------------------------------------
  // Draw my player above all opponents
  // --------------------------------------

  SDL_SetRenderDrawColor(renderer_, 255, 0, 0, SDL_ALPHA_OPAQUE);  // RED - My Player
  SDL_FRect rect = {localPlayerState.position.x, localPlayerState.position.y, 10, 10};
  SDL_RenderFillRect(renderer_, &rect);

  // --------------------------------------
  // Draw my player from Server simulation
  // --------------------------------------

  if (debugServerPositionForLocalPlayer_) {
    SDL_SetRenderDrawColor(renderer_, 0, 255, 0, SDL_ALPHA_OPAQUE);  // GREEN - My Player From Server
    rect = {localPlayerStateFromServer.position.x, localPlayerStateFromServer.position.y, 10, 10};
    SDL_RenderFillRect(renderer_, &rect);
  }
}

void GameWorldRenderer::setDataForRenderer(std::shared_ptr<DataForRenderer> dataForRenderer) {
  dataForRenderer_ = dataForRenderer;
}

void GameWorldRenderer::debugServerPositionForLocalPlayer(bool enabled) {
  debugServerPositionForLocalPlayer_ = enabled;
}
