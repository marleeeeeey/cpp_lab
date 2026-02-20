#pragma once
#include <SDL3/SDL.h>

#include <entt/signal/sigh.hpp>

#include "GameInputData.h"

// Reads SDL events and converts them into GameInputData.
// Also initiates callback calls.
class IGameInputManager {
 public:
  // ----------------------
  // Factory
  // ----------------------

  static std::unique_ptr<IGameInputManager> create(SDL_Window* window, int windowWidth, int windowHeight);
  virtual ~IGameInputManager() = default;

  // -------------
  // Signatures
  // -------------

  using OnWindowSizeChangedSignal = entt::sigh<void(int width, int height)>;

  // --------------------
  // Virtual Interface
  // --------------------

  virtual SDL_AppResult applyEvent(SDL_Event* event) = 0;
  virtual const GameInputData& getGameInputData() const = 0;
  virtual void onFrameEnd() = 0;
  virtual void onAppQuit() = 0;
  virtual entt::sink<OnWindowSizeChangedSignal> onWindowSizeChangedSink() = 0;
};