#pragma once

#include <SDL3/SDL.h>

#include "GameWorld.h"

class AppInstance {
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr; /* We will use this renderer to draw into this window every frame. */
  Uint64 last_time = 0;

  GameWorld gameWorld;

 public:
  SDL_AppResult init();
  SDL_AppResult onEvent(SDL_Event* event);
  SDL_AppResult onTick();
  void onQuit();
};