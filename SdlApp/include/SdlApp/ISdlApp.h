#pragma once

#include <SDL3/SDL.h>

#include <memory>

// Interface for SDL application
class ISdlApp {
 public:
  // IMPORTANT: "create" must be implemented in client code
  static std::unique_ptr<ISdlApp> create();

  virtual SDL_AppResult init(int argc, char* argv[]) = 0;  // SDL_AppInit
  virtual SDL_AppResult onEvent(SDL_Event* event) = 0;     // SDL_AppEvent
  virtual SDL_AppResult iterate() = 0;                     // SDL_AppIterate
  virtual ~ISdlApp() = default;                            // SDL_AppQuit
};
