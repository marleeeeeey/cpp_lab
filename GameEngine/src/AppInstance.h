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
// AppInstance is a bridge between GameWorld, SceneRenderer,
// UserInputManger and Network.
class AppInstance {
  // -------------------
  // Technical Data
  // -------------------

  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  Uint64 last_time = 0;
  double gameTimeSeconds = 0;
  double sendAccumSeconds = 0.0;

  // -------------------
  // Game Domain Data
  // -------------------

  GameWorld gameWorld;
  SceneRenderer sceneRenderer;
  UserInputManger userInputManger;
  std::unique_ptr<INetworkManager> networkManager;
  ChatDataForRendering chatDataForRendering;

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
    std::string url = "wss://echo.websocket.org";
  };
  void initOptions(int argc, char* argv[]);
  Options appOptions_;

  // ------------------------------
  // Other Methods
  // ------------------------------

  SDL_AppResult initSDL();
  void initImGui();
};