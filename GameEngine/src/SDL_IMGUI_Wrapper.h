#pragma once

#include <SDL3/SDL.h>

#include <functional>

// Itea that RAW pointers should not leave this class
// and should be wrapped in smart pointers if possible
class SDL_IMGUI_Wrapper {
  // -----------------------
  // Public Interface
  // -----------------------

 public:
  // throw an exception in case of failed SDL initialization
  SDL_IMGUI_Wrapper();

  bool onEvent(SDL_Event* event);
  float calculateDeltaTimeWhenFrameBegins();
  void render(std::function<void()> userRenderCallback);
  void onQuit();

  SDL_Window* getWindow() const { return window_; }
  SDL_Renderer* getRenderer() const { return sdlRenderer_; }

  // -----------------------
  // Private Methods
  // -----------------------
 private:
  SDL_AppResult initSDL_();
  void initImGui_();

  // -----------------------
  // Private State
  // -----------------------

  SDL_Renderer* sdlRenderer_ = nullptr;
  SDL_Window* window_ = nullptr;
  Uint64 beginFrameTime_ = 0;
};
