#pragma once

#include <SDL3/SDL.h>

#include "GameWorld.h"
#include "SceneRenderer.h"
#include "UserInputManger.h"

// The main application object. It manages the SDL window and renderer, and the game world.
// AppInstance is a bridge between GameWorld, SceneRenderer, and UserInputManger.
class AppInstance {
 private:  // Technical Data
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr; /* We will use this renderer to draw into this window every frame. */
  Uint64 last_time = 0;

 private:  // Game Domain Data
  GameWorld gameWorld;
  SceneRenderer sceneRenderer;
  UserInputManger userInputManger;

 public:
  SDL_AppResult init();
  SDL_AppResult onEvent(SDL_Event* event);
  SDL_AppResult iterate();
  void onQuit();

 private:
  SDL_AppResult initSDL();
  void initImGui();
};