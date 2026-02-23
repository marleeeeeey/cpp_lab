#pragma once

#include <SDL3/SDL.h>

// Interface for SDL application
class ISdlApp {
 public:
  // IMPORTANT: must be implemented in client code
  static ISdlApp* create();

  virtual SDL_AppResult init(int argc, char* argv[]) = 0;
  virtual SDL_AppResult onEvent(SDL_Event* event) = 0;
  virtual SDL_AppResult iterate() = 0;
  virtual void onQuit() = 0;
};
