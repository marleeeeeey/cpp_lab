#include "GameInputManager/IGameInputManager.h"

#include "GameInputManager.h"

std::unique_ptr<IGameInputManager> IGameInputManager::create(entt::dispatcher& dispatcher) {
  return std::make_unique<GameInputManager>(dispatcher);
}