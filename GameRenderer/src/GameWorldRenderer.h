#pragma once

#include <SDL3/SDL.h>

#include <optional>

#include "GameRenderer/IGameWorldRenderer.h"

class GameWorldRenderer : public IGameWorldRenderer {
 public:
  explicit GameWorldRenderer(SDL_Renderer* renderer);
  void render() override;
  void setWorldSnapshot(const WorldSnapshot& worldSnapshot) override;
  void setMyPlayerId(const PlayerId& playerId) override;

 private:
  SDL_Renderer* renderer_ = nullptr;

  std::optional<PlayerId> myPlayerId_;
  WorldSnapshot worldSnapshot_;
};
