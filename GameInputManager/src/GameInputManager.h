#pragma once
#include <SDL3/SDL.h>

#include <entt/signal/sigh.hpp>

#include "GameInputManager/GameInputData.h"
#include "GameInputManager/IGameInputManager.h"

class GameInputManager : public IGameInputManager {
 public:
  SDL_AppResult applyEvent(SDL_Event* event) override;
  const GameInputData& getGameInputData() const override;
  void onFrameEnd() override;

 private:
  void checkKeyboardInput_(SDL_Event* event);
  void checkMouseInput_(SDL_Event* event);

  SDL_Window* window_{};  // This class is not responsible for window lifecycle management
  GameInputData userInputData_;
};
