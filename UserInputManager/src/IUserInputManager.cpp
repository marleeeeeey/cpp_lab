#include "UserInputManager/IUserInputManager.h"

#include "UserInputManager.h"

std::unique_ptr<IUserInputManager> IUserInputManager::create(entt::dispatcher& dispatcher) {
  return std::make_unique<UserInputManager>(dispatcher);
}