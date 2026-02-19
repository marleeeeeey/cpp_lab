#pragma once
#include <memory>

#include "IChatRenderer.h"
#include "IGameWorldRenderer.h"

struct SDL_Renderer;

class Factory {
 public:
  std::unique_ptr<IGameWorldRenderer> createGameWorldRenderer(SDL_Renderer* renderer);
  std::unique_ptr<IChatRenderer> createChatRenderer(SDL_Renderer* renderer);
};
