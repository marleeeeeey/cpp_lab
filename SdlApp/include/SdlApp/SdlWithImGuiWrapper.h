#pragma once

#include <SDL3/SDL.h>

#include <entt/signal/sigh.hpp>
#include <functional>

// Idea that RAW pointers should not leave this class
// and should be wrapped in smart pointers if possible
class SdlWithImGuiWrapper {
 public:
  // -------------
  // Signatures
  // -------------

  using OnWindowSizeChangedSignal = entt::sigh<void(int width, int height)>;

  // -----------------------
  // Public Interface
  // -----------------------

  // throw an exception in case of failed SDL initialization
  SdlWithImGuiWrapper();

  bool onEvent(SDL_Event* event);
  float calculateDeltaTimeWhenFrameBegins();
  void render(std::function<void()> userRenderCallback);
  void onQuit();
  entt::sink<OnWindowSizeChangedSignal> onWindowSizeChangedSink();

  // I decided not to use smart pointers here because SDL will manage its lifetime
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

  bool onWindowSizeChangedInitialCallbackCalled_{false};
  OnWindowSizeChangedSignal onWindowSizeChangedSignal_;
  SDL_Renderer* sdlRenderer_{};
  SDL_Window* window_{};
  int windowWidth_{};
  int windowHeight_{};
  Uint64 beginFrameTime_{};
};
