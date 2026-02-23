#include "GameInputManager/IGameInputManager.h"

#include "GameInputManager.h"

std::unique_ptr<IGameInputManager> IGameInputManager::create() {
  return std::make_unique<GameInputManager>();
}