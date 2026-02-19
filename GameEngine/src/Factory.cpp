#include "Factory.h"

#include "ChatRenderer.h"
#include "GameWorldRenderer.h"

std::unique_ptr<IGameWorldRenderer> Factory::createGameWorldRenderer(SDL_Renderer* renderer) {
  auto gameWorldRenderer = std::make_unique<GameWorldRenderer>();
  gameWorldRenderer->init(renderer);
  return gameWorldRenderer;
}

std::unique_ptr<IChatRenderer> Factory::createChatRenderer(SDL_Renderer* renderer) {
  auto chatRenderer = std::make_unique<ChatRenderer>();
  return chatRenderer;
}