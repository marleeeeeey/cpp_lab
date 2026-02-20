#pragma once
#include <SDL3/SDL.h>

#include <entt/signal/sigh.hpp>

#include "GameInputManager/GameInputData.h"
#include "GameInputManager/IGameInputManager.h"

class GameInputManager : public IGameInputManager {
 public:
  GameInputManager(SDL_Window* window, int windowWidth, int windowHeight);

  SDL_AppResult applyEvent(SDL_Event* event) override;
  const GameInputData& getGameInputData() const override;
  void onFrameEnd() override;
  void onAppQuit() override;
  entt::sink<OnWindowSizeChangedSignal> onWindowSizeChangedSink() override { return entt::sink{onWindowSizeChangedSignal_}; }

 private:
  SDL_Window* window_{};  // This class is not responsible for window lifecycle management
  int windowWidth_{};
  int windowHeight_{};
  GameInputData userInputData_;
  OnWindowSizeChangedSignal onWindowSizeChangedSignal_;
};
