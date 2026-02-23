#pragma once
#include <SDL3/SDL.h>

#include <memory>

#include "GameInputData.h"

// Reads SDL events and converts them into GameInputData.
// Also initiates callback calls.
class IGameInputManager {
 public:
  // ----------------------
  // Factory
  // ----------------------

  static std::unique_ptr<IGameInputManager> create();
  virtual ~IGameInputManager() = default;

  // --------------------
  // Virtual Interface
  // --------------------

  virtual SDL_AppResult applyEvent(SDL_Event* event) = 0;
  virtual const GameInputData& getGameInputData() const = 0;
  virtual void onFrameEnd() = 0;
};