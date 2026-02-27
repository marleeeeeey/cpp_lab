#pragma once
#include <SDL3/SDL.h>

#include <array>
#include <entt/signal/sigh.hpp>

#include "UserInputManager/IUserInputManager.h"
#include "UserInputManager/UserInputData.h"

class UserInputManager : public IUserInputManager {
 public:
  UserInputManager(entt::dispatcher& dispatcher);

  // ------------------------------
  // IUserInputManager overrides
  // ------------------------------

  SDL_AppResult applyEvent(SDL_Event* event) override;
  const UserInputData& getUserInputData() const override;
  void onFrameEnd() override;

 private:
  void checkKeyboardInputForDispatcher_(SDL_Event* event);
  void checkKeyboardInput_(SDL_Event* event);
  void checkMouseInput_(SDL_Event* event);

  SDL_Window* window_{};  // This class is not responsible for window lifecycle management
  UserInputData userInputData_;
  entt::dispatcher& dispatcher_;
  std::array<bool, SDL_SCANCODE_COUNT> heldScancodesForDispatcher_{};
};
