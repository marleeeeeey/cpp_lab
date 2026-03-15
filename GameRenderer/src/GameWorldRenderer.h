#pragma once

#include <SDL3/SDL.h>

#include <optional>

#include "GameRenderer/IGameWorldRenderer.h"

class GameWorldRenderer : public IGameWorldRenderer {
 public:
  explicit GameWorldRenderer(SDL_Renderer* renderer);
  void render(float dt, float gameTime) override;
  void setDataForRenderer(std::shared_ptr<DataForRenderer> dataForRenderer) override;
  void debugServerPositionForLocalPlayer(bool enabled) override;

 private:
  SDL_Renderer* renderer_ = nullptr;
  std::shared_ptr<DataForRenderer> dataForRenderer_;
  bool debugServerPositionForLocalPlayer_ = false;
};
