#pragma once
#include <SDL3/SDL.h>

#include <array>
#include <entt/signal/sigh.hpp>

#include "GameInputManager/GameInputData.h"
#include "GameInputManager/IGameInputManager.h"

class GameInputManager : public IGameInputManager {
 public:
  GameInputManager(entt::dispatcher& dispatcher);

  // ------------------------------
  // IGameInputManager overrides
  // ------------------------------

  SDL_AppResult applyEvent(SDL_Event* event) override;
  const GameInputData& getGameInputData() const override;
  void onFrameEnd() override;

 private:
  void checkKeyboardInputForDispatcher_(SDL_Event* event);
  void checkKeyboardInput_(SDL_Event* event);
  void checkMouseInput_(SDL_Event* event);

  SDL_Window* window_{};  // This class is not responsible for window lifecycle management
  GameInputData userInputData_;
  entt::dispatcher& dispatcher_;
  std::array<bool, SDL_SCANCODE_COUNT> heldScancodesForDispatcher_{};
};
