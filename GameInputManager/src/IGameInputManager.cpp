#include "GameInputManager/IGameInputManager.h"

#include "GameInputManager.h"

std::unique_ptr<IGameInputManager> IGameInputManager::create(SDL_Window* window, int windowWidth, int windowHeight) {
  return std::make_unique<GameInputManager>(window, windowWidth, windowHeight);
}