#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "ChatDataForRendering.h"
#include "GameWorld.h"
#include "NetworkManager/INetworkManager.h"
#include "SceneRenderer.h"
#include "UserInputManger.h"

// ------------------------------------
// Forward declarations
// ------------------------------------

class ITransport;

// -----------------------------
// The main application object.
// ----------------------------

// It manages the SDL window and renderer, and the game world.
// GameApp is a bridge between GameWorld, SceneRenderer,
// UserInputManger and Network.
class GameApp {
  // -------------------
  // Technical Data
  // -------------------

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  Uint64 lastTime_ = 0;

  // -------------------
  // Game Domain Data
  // -------------------

  GameWorld gameWorld_;
  SceneRenderer sceneRenderer_;
  UserInputManger userInputManger_;
  std::unique_ptr<INetworkManager> networkManager_;
  ChatDataForRendering chatDataForRendering_;

 public:
  // -------------------
  // Public Interface
  // -------------------

  SDL_AppResult init(int argc, char* argv[]);
  SDL_AppResult onEvent(SDL_Event* event);
  SDL_AppResult iterate();
  void onQuit();

 private:
  // ---------------------
  // Application Options
  // ---------------------

  struct Options {
    std::string url;
  };
  void initOptions(int argc, char* argv[]);
  Options appOptions_;

  // ------------------------------
  // Other Methods
  // ------------------------------

  SDL_AppResult initSDL();
  void initImGui();
};