#pragma once
#include <SDL3/SDL.h>

#include <entt/signal/dispatcher.hpp>
#include <memory>

#include "UserInputData.h"

// Reads SDL events and converts them into UserInputData.
// Also initiates callback calls.
class IUserInputManager {
 public:
  // ----------------------
  // Factory
  // ----------------------

  static std::unique_ptr<IUserInputManager> create(entt::dispatcher& dispatcher);
  virtual ~IUserInputManager() = default;

  // --------------------
  // Virtual Interface
  // --------------------

  virtual SDL_AppResult applyEvent(SDL_Event* event) = 0;
  virtual const UserInputData& getUserInputData() const = 0;
  virtual void onFrameEnd() = 0;
};